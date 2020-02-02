#include "StdInc.h"
#include "detours.h"

#pragma comment(lib, "detours.lib")

auto OLD_CPhysical_ApplyCollisionAlt = (bool(__thiscall*) (CPhysical* pThis, CPhysical * pEntity, CColPoint * pColPoint, float* pDamageIntensity, CVector * pVecMoveSpeed, CVector * pVecTurnSpeed))0x544D50;
bool __fastcall CPhysical_ApplyCollisionAlt(CPhysical* pThis, void* padding, CPhysical* pEntity, CColPoint* pColPoint, float* pDamageIntensity, CVector* pVecMoveSpeed, CVector* pVecTurnSpeed);

void __cdecl HOOK_THEFUNCTION();

void InjectHooksMain(void)
{
    // CStreaming is unstable for now.
    //CStreaming::InjectHooks();

    /*
    CPhysical::InjectHooks();
    CRenderer::InjectHooks();
    CPedIntelligence::InjectHooks();
    CTrain::InjectHooks();
    CTaskManager::InjectHooks();
    CTaskComplexUseSequence::InjectHooks();
    CTaskComplexSequence::InjectHooks();
    CTaskComplexWander::InjectHooks();
    CTaskComplexWanderCop::InjectHooks();
    CTaskSimplePlayerOnFoot::InjectHooks();
    CTaskSimpleSwim::InjectHooks();
    */

   // /*
     DetourRestoreAfterWith();
     DetourTransactionBegin();
     DetourUpdateThread(GetCurrentThread());

     std::printf("GOING TO HOOK FUNC NOW\n");
     DetourAttach(&(PVOID&)OLD_CPhysical_ApplyCollisionAlt, CPhysical_ApplyCollisionAlt);
     DetourTransactionCommit();
     //*/
}

bool __fastcall CPhysical_ApplyCollisionAlt(CPhysical* pThis, void* padding, CPhysical* pEntity, CColPoint* pColPoint, float* pDamageIntensity, CVector* pVecMoveSpeed, CVector* pVecTurnSpeed)
{
    printf("CPhysical_ApplyCollisionAlt called!\n");

    if (pThis->m_pAttachedTo)
    {
        if (pThis->m_pAttachedTo->m_nType > ENTITY_TYPE_BUILDING && pThis->m_pAttachedTo->m_nType < ENTITY_TYPE_DUMMY
            && pThis->m_pAttachedTo->m_nType != ENTITY_TYPE_PED)
        {
            float fDamageIntensity = 0.0f;
            pThis->m_pAttachedTo->ApplySoftCollision(pEntity, pColPoint, &fDamageIntensity);
        }
    }

    if (pThis->physicalFlags.bDisableTurnForce)
    {
        float fSpeedDotProduct = DotProduct(&pThis->m_vecMoveSpeed, &pColPoint->m_vecNormal);
        if (fSpeedDotProduct < 0.0f)
        {
            *pDamageIntensity = -(fSpeedDotProduct * pThis->m_fMass);
            pThis->ApplyMoveForce(pColPoint->m_vecNormal * *pDamageIntensity);

            float fCollisionImpact1 = *pDamageIntensity / pThis->m_fMass;
            AudioEngine.ReportCollision(pThis, pEntity, pColPoint->m_nSurfaceTypeA, pColPoint->m_nSurfaceTypeB, pColPoint,
                &pColPoint->m_vecNormal, fCollisionImpact1, 1.0f, false, false);
            return true;
        }
        return false;
    }

    CVehicle* pVehicle = static_cast<CVehicle*>(pThis);
    CVector vecMovingDirection = pColPoint->m_vecPoint - pThis->m_matrix->pos;
    CVector vecSpeed;
    pThis->GetSpeed(&vecSpeed, vecMovingDirection);

    if (pThis->physicalFlags.b27 && pThis->m_nType == ENTITY_TYPE_VEHICLE && pColPoint->m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT)
    {
        CVector outSpeed;
        pVehicle->AddMovingCollisionSpeed(&outSpeed, vecMovingDirection);
        vecSpeed += outSpeed;
    }

    CVector vecMoveDirection = pColPoint->m_vecNormal;
    float fSpeedDotProduct = DotProduct(&vecMoveDirection, &vecSpeed);
    if (fSpeedDotProduct >= 0.0f)
    {
        return false;
    }

    CVector vecCentreOfMassMultiplied;
    Multiply3x3(&vecCentreOfMassMultiplied, pThis->m_matrix, &pThis->m_vecCentreOfMass);
    if (pThis->physicalFlags.bInfiniteMass)
    {
        vecCentreOfMassMultiplied = CVector(0.0f, 0.0f, 0.0f);
    }

    CVector vecDifference = vecMovingDirection - vecCentreOfMassMultiplied;
    CVector vecCrossProduct;
    vecCrossProduct.Cross(vecDifference, vecMoveDirection);
    float fSquaredMagnitude = vecCrossProduct.SquaredMagnitude();
    float fCollisionMass = 1.0f / (fSquaredMagnitude / pThis->m_fTurnMass + 1.0f / pThis->m_fMass);

    unsigned short entityAltCol = ALT_ENITY_COL_DEFAULT;
    float fMoveSpeedLimit = CTimer::ms_fTimeStep * 0.008f;
    float fMoveSpeedLimitMultiplier = 1.3f;
    if (pThis->m_nType == ENTITY_TYPE_OBJECT)
    {
        fMoveSpeedLimitMultiplier = 1.3f;
        entityAltCol = ALT_ENITY_COL_OBJECT;
        fMoveSpeedLimit = fMoveSpeedLimitMultiplier * fMoveSpeedLimit;
    }
    else
    {
        if (pThis->m_nType == ENTITY_TYPE_VEHICLE)
        {
            if (!pThis->physicalFlags.bSubmergedInWater)
            {
                unsigned int vehicleClass = pVehicle->m_nVehicleClass;
                if (vehicleClass != VEHICLE_BIKE || (pThis->m_nStatus != STATUS_ABANDONED) && pThis->m_nStatus != STATUS_WRECKED)
                {
                    if (vehicleClass == VEHICLE_BOAT)
                    {
                        fMoveSpeedLimitMultiplier = 1.5;
                        entityAltCol = ALT_ENITY_COL_BOAT;
                    }
                    else
                    {
                        if (pThis->m_matrix->at.z >= -0.3f)
                        {
                            goto LABEL_31;
                        }
                        else
                        {
                            fMoveSpeedLimitMultiplier = 1.4f;
                            entityAltCol = ALT_ENITY_COL_VEHICLE;
                        }
                    }
                }
                else
                {
                    fMoveSpeedLimitMultiplier = 1.7f;
                    entityAltCol = ALT_ENITY_COL_BIKE_WRECKED;
                }
                fMoveSpeedLimit = fMoveSpeedLimitMultiplier * fMoveSpeedLimit;
            }
        }
    }
LABEL_31:
    float fCollisionImpact2 = 1.0f;
    if (entityAltCol == ALT_ENITY_COL_OBJECT)
    {
        if (!pThis->m_bHasContacted
            && fabs(pThis->m_vecMoveSpeed.x) < fMoveSpeedLimit
            && fabs(pThis->m_vecMoveSpeed.y) < fMoveSpeedLimit
            && fMoveSpeedLimit + fMoveSpeedLimit > fabs(pThis->m_vecMoveSpeed.z))
        {
            fCollisionImpact2 = 0.0f;
            *pDamageIntensity = -0.98f * fCollisionMass * fSpeedDotProduct;
        }
        goto LABEL_37;
    }
    if (entityAltCol != ALT_ENITY_COL_BIKE_WRECKED)
    {
        if (entityAltCol == ALT_ENITY_COL_VEHICLE)
        {
            if (fabs(pThis->m_vecMoveSpeed.x) < fMoveSpeedLimit
                && fabs(pThis->m_vecMoveSpeed.y) < fMoveSpeedLimit
                && fMoveSpeedLimit + fMoveSpeedLimit > fabs(pThis->m_vecMoveSpeed.z))
            {
                pDamageIntensity = pDamageIntensity;
                fCollisionImpact2 = 0.0f;
                *pDamageIntensity = -0.95f * fCollisionMass * fSpeedDotProduct;
                goto LABEL_59;
            }
        }
        else if (entityAltCol == ALT_ENITY_COL_BOAT
            && fabs(pThis->m_vecMoveSpeed.x) < fMoveSpeedLimit
            && fabs(pThis->m_vecMoveSpeed.y) < fMoveSpeedLimit
            && fMoveSpeedLimit + fMoveSpeedLimit > fabs(pThis->m_vecMoveSpeed.z))
        {
            fCollisionImpact2 = 0.0f;
            *pDamageIntensity = -0.95f * fCollisionMass * fSpeedDotProduct;
            goto LABEL_59;
        }
    LABEL_37:
        float fElasticity = pThis->m_fElasticity + pThis->m_fElasticity;
        if (pThis->m_nType != ENTITY_TYPE_VEHICLE || pVehicle->m_nVehicleClass != VEHICLE_BOAT
            || pColPoint->m_nSurfaceTypeB != SURFACE_WOOD_SOLID && vecMoveDirection.z >= 0.5f)
        {
            fElasticity = pThis->m_fElasticity;
        }

        *pDamageIntensity = -((fElasticity + 1.0f) * fCollisionMass * fSpeedDotProduct);
        goto LABEL_59;
    }
    else
    {
        if (fabs(pThis->m_vecMoveSpeed.x) >= fMoveSpeedLimit
            || fabs(pThis->m_vecMoveSpeed.y) >= fMoveSpeedLimit
            || fMoveSpeedLimit + fMoveSpeedLimit <= fabs(pThis->m_vecMoveSpeed.z))
        {
            goto LABEL_37;
        }
        fCollisionImpact2 = 0.0f;
        *pDamageIntensity = -0.95f * fCollisionMass * fSpeedDotProduct;
    }
LABEL_59:
    CVector vecMoveSpeed = vecMoveDirection * *pDamageIntensity;

    if (pThis->physicalFlags.bDisableZ || pThis->physicalFlags.bInfiniteMass || pThis->physicalFlags.bDisableMoveForce)
    {
        pThis->ApplyForce(vecMoveSpeed, vecMovingDirection, true);
    }
    else
    {
        CVector vecSpeed = vecMoveSpeed / pThis->m_fMass;
        if (pThis->m_nType == ENTITY_TYPE_VEHICLE)
        {
            if (!pThis->m_bHasHitWall|| pThis->m_vecMoveSpeed.SquaredMagnitude() <= 0.1f
                && (pEntity->m_nType == ENTITY_TYPE_BUILDING || pEntity->physicalFlags.bDisableCollisionForce))
            {
                *pVecMoveSpeed += vecSpeed * 1.2f;
            }
            else
            {
                *pVecMoveSpeed += vecSpeed;
            }

            vecMoveSpeed *= 0.8f;
        }
        else
        {
            *pVecMoveSpeed += vecSpeed;
        }

        Multiply3x3(&vecCentreOfMassMultiplied, pThis->m_matrix, &pThis->m_vecCentreOfMass);
        float fTurnMass = pThis->m_fTurnMass;
        CVector vecDifference = vecMovingDirection - vecCentreOfMassMultiplied;
        CVector vecCrossProduct;
        vecCrossProduct.Cross(vecDifference, vecMoveSpeed);
        *pVecTurnSpeed += vecCrossProduct / fTurnMass;
    }

    float fCollisionImpact1 = *pDamageIntensity / fCollisionMass;
    AudioEngine.ReportCollision(pThis, pEntity, pColPoint->m_nSurfaceTypeA, pColPoint->m_nSurfaceTypeB, pColPoint,
        &pColPoint->m_vecNormal, fCollisionImpact1, fCollisionImpact2, false, false);
    return true;
}

/*
dwReturnLocation:
0 means that the function should return.
1 means continue the function and it is inside of the "if" condition
2 means continue the function and it is outside of the "if" condition
*/

enum eFunctionReturnValue
{
    FUNCTION_RETURN = 0,
    FUNCTION_INSIDE_IF = 1,
    FUNCTION_OUTSIDE_IF = 2,
    //FUNCTION_SWITCH_CASE_2 = 3,
    //FUNCTION_SOMELABEL = 4
};

DWORD RETURN_HOOK_INSIDE_IF = 0x0;
DWORD RETURN_HOOK_OUTSIDE_IF = 0x0554ADD;
DWORD RETURN_HOOK_EXIT_WITH_GRACE = 0x0;
void _declspec(naked) HOOK_THEFUNCTION()
{
    _asm
    {
    }
}
