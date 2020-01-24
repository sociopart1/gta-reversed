#include "StdInc.h"
#include "detours.h"

#pragma comment(lib, "detours.lib")

//bool __thiscall CPhysical_ApplyCollision(CPhysical* pThis)
auto OLD_CPhysical_ApplyCollision = (bool(__thiscall*) (CPhysical * pThis, CPhysical * pEntity, CColPoint * pColPoint, float* pThisDamageIntensity, float* pEntityDamageIntensity))0x548680;
bool __fastcall CPhysical_ApplyCollision(CPhysical* pThis, void* padding, CPhysical* pEntity, CColPoint* pColPoint, float* pThisDamageIntensity, float* pEntityDamageIntensity);

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
   ///*
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    std::printf("GOING TO HOOK FUNC NOW\n");
    DetourAttach(&(PVOID&)OLD_CPhysical_ApplyCollision, CPhysical_ApplyCollision);
    DetourTransactionCommit();
   // */
}
/*
enum eFunctionReturnValue
{
    FUNCTION_RETURN = 0,
    FUNCTION_INSIDE_IF = 1,
    FUNCTION_OUTSIDE_IF = 2
};
*/
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
    FUNCTION_SWITCH_CASE_2 = 3,
    FUNCTION_SOMELABEL = 4
};

bool __fastcall CPhysical_ApplyCollision(CPhysical* pThis, void* padding, CPhysical* pEntity, CColPoint* pColPoint, float* pThisDamageIntensity, float* pEntityDamageIntensity)
{
    printf(" calling CPhysical_ApplyCollision \n");

    auto pEntityObject = static_cast<CObject*>(pEntity);
    auto pEntityPed = static_cast<CPed*>(pEntity);
    auto pEntityVehicle = static_cast<CVehicle*>(pEntity);

    auto pThisObject = static_cast<CObject*>(pThis);
    auto pThisPed = static_cast<CPed*>(pThis);
    auto pThisVehicle = static_cast<CVehicle*>(pThis);

    bool bThisSomePedIsEntity = false;
    bool bEntitySomePedIsThis = false;
    bool bEntityCollisionForceDisabled = false;

    float fThisMassFactor = 0.0;
    float fEntityMassFactor = 0.0f;

    if (!pEntity->physicalFlags.bDisableTurnForce || pThis->physicalFlags.bDisableMoveForce)
    {
        fThisMassFactor = 2.0f;       
        if (!pThis->physicalFlags.b01)
        {
            fThisMassFactor = 1.0f;
        }
    }
    else
    {
        fThisMassFactor = 10.0f;
        if (pEntity->m_nType == ENTITY_TYPE_PED && pEntityPed->m_pSomePed == pThis)
        {
            bEntitySomePedIsThis = true;
        }
    }

    if (pThis->physicalFlags.bDisableTurnForce)
    {
        if (pThis->m_nType == ENTITY_TYPE_PED && pThisPed->IsPlayer()
            && pEntity->m_nType == ENTITY_TYPE_VEHICLE
            && (pEntity->m_nStatus == STATUS_ABANDONED || pEntity->m_nStatus == STATUS_WRECKED || pThis->m_bIsStuck))
        {
            float fTheEntityMass = pEntity->m_fMass - 2000.0f;
            if (fTheEntityMass < 0.0f)
            {
                fTheEntityMass = 0.0f;
            }
            fEntityMassFactor = 1.0f / (fTheEntityMass * 0.00019999999f + 1.0f);
        }
        else if (!pEntity->physicalFlags.bDisableMoveForce)
        {
            fEntityMassFactor = 10.0f;
        }
        if (pThis->m_nType == ENTITY_TYPE_PED && pThisPed->m_pSomePed == pEntity)
        {
            bThisSomePedIsEntity = true;
            fEntityMassFactor = 10.0f;
        }
    }
    else if (pThis->m_nType == ENTITY_TYPE_VEHICLE && pThisVehicle->m_pTrailer)
    {
        fEntityMassFactor = (pThisVehicle->m_pTrailer->m_fMass + pThis->m_fMass) / pThis->m_fMass;
    }
    else
    {
        fEntityMassFactor = 2.0f;
        if (!pEntity->physicalFlags.b01)
        {
            fEntityMassFactor = 1.0f;
        }
    }

    if (pEntity->physicalFlags.bDisableCollisionForce && !pEntity->physicalFlags.bCollidable
        || pEntity->m_pAttachedTo && !pEntity->physicalFlags.bInfiniteMass)
    {
        bEntityCollisionForceDisabled = true;
        bThisSomePedIsEntity = false;
    }

    CVector vecThisCentreOfMassMultiplied;
    CVector vecEntityCentreOfMassMultiplied;

    Multiply3x3(&vecThisCentreOfMassMultiplied, pThis->m_matrix, &pThis->m_vecCentreOfMass);
    Multiply3x3(&vecEntityCentreOfMassMultiplied, pEntity->m_matrix, &pEntity->m_vecCentreOfMass);

    if (pThis->physicalFlags.bInfiniteMass)
    {
        vecThisCentreOfMassMultiplied = CVector(0.0f, 0.0f, 0.0f);
    }

    if (pEntity->physicalFlags.bInfiniteMass)
    {
        vecEntityCentreOfMassMultiplied = CVector(0.0f, 0.0f, 0.0f);
    }

    if (pThis->m_nType == ENTITY_TYPE_VEHICLE && pEntity->m_nType == ENTITY_TYPE_OBJECT
        && pEntityObject->objectFlags.bIsLampPost
        || pEntity->m_nType == ENTITY_TYPE_VEHICLE && pThis->m_nType == ENTITY_TYPE_OBJECT &&
         pThisObject->objectFlags.bIsLampPost)
    {
        pColPoint->m_vecNormal.z = 0.0f;
    }

    if ((pEntity->m_bIsStatic || pEntity->m_bIsStaticWaitingForCollision) && !bEntityCollisionForceDisabled)
    {
        if (pThis->physicalFlags.bDisableTurnForce)
        {
            float fThisMoveSpeedSum = pThis->m_vecMoveSpeed.z * pColPoint->m_vecNormal.z
                                    + pThis->m_vecMoveSpeed.y * pColPoint->m_vecNormal.y
                                    + pColPoint->m_vecNormal.x * pThis->m_vecMoveSpeed.x;
            if (fThisMoveSpeedSum >= 0.0f)
            {
                goto LABEL_61;
            }
            if (pEntity->m_nType != ENTITY_TYPE_OBJECT)
            {
                if (pEntity->physicalFlags.bDisableCollisionForce)
                {
                    return pThis->ApplyCollision(pEntity, *pColPoint, *pThisDamageIntensity);
                }

                pEntity->SetIsStatic(false);
                goto LABEL_61;
            }

            *pThisDamageIntensity = -(fThisMoveSpeedSum * pThis->m_fMass);
            *pEntityDamageIntensity = *pThisDamageIntensity;

            if (pEntity->physicalFlags.bDisableCollisionForce)
            {
                return pThis->ApplyCollision(pEntity, *pColPoint, *pThisDamageIntensity);
            }

            CObjectInfo* pEntityObjectInfo = pEntityObject->m_pObjectInfo;
            if (pEntityObjectInfo->m_fUprootLimit >= 9999.0f || *pThisDamageIntensity <= pEntityObjectInfo->m_fUprootLimit)
            {
                return pThis->ApplyCollision(pEntity, *pColPoint, *pThisDamageIntensity);
            }

            if (IsGlassModel(pEntity))
            {
                CGlass::WindowRespondsToCollision(pEntity, *pThisDamageIntensity, pThis->m_vecMoveSpeed, pColPoint->m_vecPoint, false);
                goto LABEL_61;
            }
            if (pEntity->physicalFlags.bDisableCollisionForce)
            {
            LABEL_61:

                if (pEntity->m_bIsStatic || pEntity->m_bIsStaticWaitingForCollision)
                {
                    return false;
                }
                if (!pEntity->physicalFlags.bDisableCollisionForce)
                {
                    pEntity->AddToMovingList();
                }
                goto LABEL_65;
            }

            pEntity->SetIsStatic(false);
            CWorld::Players[CWorld::PlayerInFocus].m_nHavocCaused += 2;
        LABEL_60:
            CStats::IncrementStat(STAT_COST_OF_PROPERTY_DAMAGED, static_cast<float>(rand() % 30 + 30));
            goto LABEL_61;
        }

        CVector vecThisMovingDirection = pColPoint->m_vecPoint - pThis->m_matrix->pos;
        CVector vecThisSpeed;
        pThis->GetSpeed(&vecThisSpeed, vecThisMovingDirection);

        if (pThis->physicalFlags.b27 && pThis->m_nType == ENTITY_TYPE_VEHICLE && pColPoint->m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT)
        {
            CVector outSpeed;
            pThisVehicle->AddMovingCollisionSpeed(&outSpeed, vecThisMovingDirection);
            vecThisSpeed += outSpeed;
        }

        float fThisMoveSpeedSum = vecThisSpeed.y * pColPoint->m_vecNormal.y
                                + vecThisSpeed.z * pColPoint->m_vecNormal.z
                                + vecThisSpeed.x * pColPoint->m_vecNormal.x;
        if (fThisMoveSpeedSum >= 0.0f)
        {
            goto LABEL_61;
        }

        if (pEntity->m_nType != ENTITY_TYPE_OBJECT)
        {
            if (pEntity->physicalFlags.bDisableCollisionForce)
            {
                return pThis->ApplyCollision(pEntity, *pColPoint, *pThisDamageIntensity);
            }

            pEntity->SetIsStatic(false);
            CWorld::Players[CWorld::PlayerInFocus].m_nHavocCaused += 2;
            goto LABEL_60;
        }

        CVector vecThisDifference = (vecThisMovingDirection - vecThisCentreOfMassMultiplied);
        CVector vecThisCrossProduct;
        vecThisCrossProduct.Cross(vecThisDifference, pColPoint->m_vecNormal);
        float squaredMagnitude = vecThisCrossProduct.SquaredMagnitude();
        float fThisCollisionMass = 1.0f / (squaredMagnitude / pThis->m_fTurnMass + 1.0f / pThis->m_fMass);
        if (!pThis->m_bHasHitWall)
        {
            *pThisDamageIntensity = -((pThis->m_fElasticity + 1.0f) * fThisCollisionMass * fThisMoveSpeedSum);
        }
        else
        {
            *pThisDamageIntensity = fThisCollisionMass * fThisMoveSpeedSum * -1.0f;
        }

        *pEntityDamageIntensity = *pThisDamageIntensity;

        CObjectInfo* pEntityObjectInfo = pEntityObject->m_pObjectInfo;

        float fObjectDamageMultiplier = 1.0f;
        if (pThis->m_nType == ENTITY_TYPE_VEHICLE && pThisVehicle->m_nVehicleSubClass == VEHICLE_BIKE)
        {
            fObjectDamageMultiplier = 3.0f;
        }

        if (pEntityObject->m_nColDamageEffect)
        {
            float fObjectDamage = fObjectDamageMultiplier * *pThisDamageIntensity;
            if (fObjectDamage > 20.0f)
            {
                pEntityObject->ObjectDamage(fObjectDamage, &pColPoint->m_vecPoint, &pColPoint->m_vecNormal, pThis, WEAPON_UNIDENTIFIED);
                if (!pEntity->m_bUsesCollision)
                {
                    if (!pThis->physicalFlags.bDisableCollisionForce)
                    {
                        float fColDamageMultiplier = pEntityObjectInfo->m_fColDamageMultiplier;
                        float fCollisionDamage = fColDamageMultiplier + fColDamageMultiplier;
                        CVector vecMoveForce = (pColPoint->m_vecNormal * *pThisDamageIntensity) / fCollisionDamage;
                        pThis->ApplyForce(vecMoveForce, vecThisMovingDirection, true);
                    }

                    float fDamageIntensityMultiplier = pEntityObjectInfo->m_fColDamageMultiplier / fThisCollisionMass;
                    float fCollisionImpact1 = fDamageIntensityMultiplier * *pThisDamageIntensity + fDamageIntensityMultiplier * *pThisDamageIntensity;
                    
                    AudioEngine.ReportCollision(pThis, pEntity, pColPoint->m_nSurfaceTypeA, pColPoint->m_nSurfaceTypeB, pColPoint,
                        &pColPoint->m_vecNormal, fCollisionImpact1, 1.0f, false, false);
                    return false;
                }
            }
        }
        if (pEntity->physicalFlags.bDisableCollisionForce || pEntityObjectInfo->m_fUprootLimit >= 9999.0f
            || *pThisDamageIntensity <= pEntityObjectInfo->m_fUprootLimit && (!pThis->m_bIsStuck || !pThis->m_bHasHitWall ))
        {
            if (IsGlassModel(pEntity))
            {
                CGlass::WindowRespondsToSoftCollision(pEntityObject, *pThisDamageIntensity);
            }
            return pThis->ApplyCollision(pEntity, *pColPoint, *pThisDamageIntensity);
        }

        if (IsGlassModel(pEntity))
        {
            CGlass::WindowRespondsToCollision(pEntity, *pThisDamageIntensity, pThis->m_vecMoveSpeed, pColPoint->m_vecPoint, false);
        }
        else
        {
            pEntity->SetIsStatic(false);
        }

        int entityModelIndex = pEntity->m_nModelIndex;
        if (entityModelIndex != MI_FIRE_HYDRANT || pEntityObject->objectFlags.bIsExploded)
        {
            if (entityModelIndex != MI_PARKINGMETER && entityModelIndex != MI_PARKINGMETER2 || pEntityObject->objectFlags.bIsExploded)
            {
                if (pEntity->m_nType != ENTITY_TYPE_OBJECT || pEntityObjectInfo->m_bCausesExplosion)
                {
                LABEL_120:
                    if (!pThis->physicalFlags.bDisableCollisionForce && pEntityObjectInfo->m_fUprootLimit > 200.0f)
                    {
                        CVector vecMoveForce = (pColPoint->m_vecNormal * 0.2f) * *pThisDamageIntensity;
                        pThis->ApplyForce(vecMoveForce, vecThisMovingDirection, true);
                    }
                    goto LABEL_61;
                }
            }
            else
            {
                CPickups::CreateSomeMoney(pEntity->GetPosition(), rand() % 100);
            }
        }
        else
        {
            g_fx.TriggerWaterHydrant(pEntity->GetPosition());
        }

        pEntityObject->objectFlags.bIsExploded = true;
        goto LABEL_120;
    }
LABEL_65:
    if (pThis->physicalFlags.bDisableTurnForce)
    {
        if (pEntity->physicalFlags.bDisableTurnForce)
        {
            bool bApplyEntityCollisionForce = false;
            float fThisMass = pThis->m_fMass;
            float fEntityMass = pEntity->m_fMass;
            float fThisMoveSpeedSum = pThis->m_vecMoveSpeed.y * pColPoint->m_vecNormal.y
                                    + pThis->m_vecMoveSpeed.z * pColPoint->m_vecNormal.z
                                    + pThis->m_vecMoveSpeed.x * pColPoint->m_vecNormal.x;
            float fEntityMoveSpeedSum = pEntity->m_vecMoveSpeed.z * pColPoint->m_vecNormal.z
                                       + pEntity->m_vecMoveSpeed.y * pColPoint->m_vecNormal.y
                                       + pEntity->m_vecMoveSpeed.x * pColPoint->m_vecNormal.x;
            float fMoveSpeed = 0.0f;
            if (pThis->physicalFlags.bDisableCollisionForce || pThis->physicalFlags.bDontApplySpeed)
            {
                fMoveSpeed = fThisMoveSpeedSum;
            }
            else
            {
                if (pEntity->physicalFlags.bDisableCollisionForce || pEntity->physicalFlags.bDontApplySpeed)
                {
                    fMoveSpeed = fEntityMoveSpeedSum;
                    goto LABEL_132;
                }
           
                if (!pThisPed->bPedThirdFlags32) // Hmm?? Is this a bug?
                {
                    if (fEntityMoveSpeedSum >= 0.0f)
                    {
                        fMoveSpeed = 0.0f;
                    }
                    else
                    {
                        fMoveSpeed = fEntityMoveSpeedSum;
                    }
                LABEL_132:
                    float fThisMoveSpeedDifference = fThisMoveSpeedSum - fMoveSpeed;
                    if (fThisMoveSpeedDifference >= 0.0f)
                    {
                        return false;
                    }

                    float fThisMoveSpeedElasticity = 0.0f;
                    float fTheElasticity = (pEntity->m_fElasticity + pThis->m_fElasticity) * 0.5f;
                    if (pThis->m_bHasHitWall)
                    {
                        fThisMoveSpeedElasticity = fMoveSpeed;
                    }
                    else
                    {
                        fThisMoveSpeedElasticity = fMoveSpeed - fTheElasticity * fThisMoveSpeedDifference;
                    }

                    *pThisDamageIntensity = (fThisMoveSpeedElasticity - fThisMoveSpeedSum) * fThisMass;

                    CVector vecThisMoveForce = pColPoint->m_vecNormal * *pThisDamageIntensity;
                    if (!pThis->physicalFlags.bDisableCollisionForce && !pThis->physicalFlags.bDontApplySpeed)
                    {
                        pThis->ApplyMoveForce(vecThisMoveForce);

                        float fCollisionImpact1 = *pThisDamageIntensity / fThisMass;
                        AudioEngine.ReportCollision(pThis, pEntity, pColPoint->m_nSurfaceTypeA, pColPoint->m_nSurfaceTypeB, pColPoint,
                            &pColPoint->m_vecNormal, fCollisionImpact1, 1.0f, false, false);
                    }
                    if (bApplyEntityCollisionForce)
                    {
                        float fEntityMoveSpeedElasticity = 0.0f;
                        if(pEntity->m_bHasHitWall)
                        {
                            fEntityMoveSpeedElasticity = fMoveSpeed;
                        }
                        else
                        {
                            fEntityMoveSpeedElasticity = fMoveSpeed - (fEntityMoveSpeedSum - fMoveSpeed) * fTheElasticity;
                        }
                      
                        *pEntityDamageIntensity = -((fEntityMoveSpeedElasticity - fEntityMoveSpeedSum) * fEntityMass);
               
            
                        CVector vecEntityMoveForce = pColPoint->m_vecNormal * *pEntityDamageIntensity * -1.0f;
                        if (!pEntity->physicalFlags.bDisableCollisionForce && !pEntity->physicalFlags.bDontApplySpeed)
                        {
                            if (pEntity->m_bIsInSafePosition)
                            {
                                pEntity->UnsetIsInSafePosition();
                            }

                            pEntity->ApplyMoveForce(vecEntityMoveForce);

                            float fCollisionImpact1  = *pEntityDamageIntensity / fEntityMass;
                            AudioEngine.ReportCollision(pEntity, pThis, pColPoint->m_nSurfaceTypeB, pColPoint->m_nSurfaceTypeA, pColPoint,
                                &pColPoint->m_vecNormal, fCollisionImpact1, 1.0f, false, false);
                        }
                    }
                    return 1;
                }
                fMoveSpeed = (fThisMass * fThisMoveSpeedSum * 4.0f + fEntityMass * fEntityMoveSpeedSum) / (fThisMass * 4.0f + fEntityMass);
            }
            bApplyEntityCollisionForce = true;
            goto LABEL_132;
        }
    }

    if (pThis->physicalFlags.bDisableTurnForce)
    {
        CVector vecEntityMovingDirection = pColPoint->m_vecPoint - pEntity->m_matrix->pos;
        CVector vecEntitySpeed;
        pEntity->GetSpeed(&vecEntitySpeed, vecEntityMovingDirection);

        if (!pEntity->physicalFlags.b27 || pEntity->m_nType!= ENTITY_TYPE_VEHICLE || pColPoint->m_nSurfaceTypeB != SURFACE_CAR_MOVINGCOMPONENT)
        {
            // nothing
        }
        else
        {
            CVector outSpeed;
            pEntityVehicle->AddMovingCollisionSpeed(&outSpeed, vecEntityMovingDirection);
            vecEntitySpeed += outSpeed;
        }

        float fThisMoveSpeedSum = pThis->m_vecMoveSpeed.z * pColPoint->m_vecNormal.z
                                + pThis->m_vecMoveSpeed.y * pColPoint->m_vecNormal.y
                                + pThis->m_vecMoveSpeed.x * pColPoint->m_vecNormal.x;
        float fEntityMoveSpeedSum = vecEntitySpeed.z * pColPoint->m_vecNormal.z
                                  + vecEntitySpeed.y * pColPoint->m_vecNormal.y
                                  + vecEntitySpeed.x * pColPoint->m_vecNormal.x;

        float fThisMass = fThisMassFactor * pThis->m_fMass;

        CVector vecEntityDifference = (vecEntityMovingDirection - vecEntityCentreOfMassMultiplied);
        CVector vecEntityCrossProduct;
        vecEntityCrossProduct.Cross(vecEntityDifference, pColPoint->m_vecNormal);
        float squaredMagnitude = vecEntityCrossProduct.SquaredMagnitude();
        float fEntityCollisionMass = 0.0f;
        if (pEntity->physicalFlags.bDisableMoveForce)
        {
            fEntityCollisionMass = squaredMagnitude / (fEntityMassFactor * pEntity->m_fTurnMass);
        }
        else
        {
            fEntityCollisionMass = squaredMagnitude / (fEntityMassFactor * pEntity->m_fTurnMass) + 1.0f / (fEntityMassFactor * pEntity->m_fMass);
        }

        fEntityCollisionMass = 1.0f / fEntityCollisionMass;

        float fMoveSpeed = 0.0f;
        if (bEntityCollisionForceDisabled)
        {
            fMoveSpeed = fEntityMoveSpeedSum;
        }
        else
        {
            fMoveSpeed = (fEntityCollisionMass * fEntityMoveSpeedSum + fThisMass * fThisMoveSpeedSum) / (fEntityCollisionMass + fThisMass);
        }

        float fThisMoveSpeedDifference = fThisMoveSpeedSum - fMoveSpeed;
        if (fThisMoveSpeedDifference < 0.0f)
        {
            float fThisMoveSpeedElasticity = 0.0f;
            float fEntityMoveSpeedElasticity = 0.0f;

            float fTheElasticity = (pEntity->m_fElasticity + pThis->m_fElasticity) * 0.5f;
            if (pThis->m_bHasHitWall)
            {
                fThisMoveSpeedElasticity = fMoveSpeed;
            }
            else
            {
                fThisMoveSpeedElasticity = fMoveSpeed - fTheElasticity * fThisMoveSpeedDifference;
            }

            if (pEntity->m_bHasHitWall)
            {
                fEntityMoveSpeedElasticity = fMoveSpeed;
            }
            else
            {
                fEntityMoveSpeedElasticity = fMoveSpeed - (fEntityMoveSpeedSum - fMoveSpeed) * fTheElasticity;
            }

            *pThisDamageIntensity = (fThisMoveSpeedElasticity - fThisMoveSpeedSum) * fThisMass;
            *pEntityDamageIntensity = -((fEntityMoveSpeedElasticity - fEntityMoveSpeedSum) * fEntityCollisionMass);

            CVector vecThisMoveForce = (*pThisDamageIntensity / fThisMassFactor) * pColPoint->m_vecNormal;
            CVector vecEntityMoveForce = (pColPoint->m_vecNormal * (*pEntityDamageIntensity / fEntityMassFactor) * -1.0f);

            if (!pThis->physicalFlags.bDisableCollisionForce)
            {
                if (vecThisMoveForce.z < 0.0f)
                {
                    vecThisMoveForce.z = 0.0f;
                }
                if (bThisSomePedIsEntity)
                {
                    vecThisMoveForce.x = vecThisMoveForce.x + vecThisMoveForce.x;
                    vecThisMoveForce.y = vecThisMoveForce.y + vecThisMoveForce.y;
                }

                pThis->ApplyMoveForce(vecThisMoveForce);
            }
            if (!pEntity->physicalFlags.bDisableCollisionForce && !bThisSomePedIsEntity)
            {
                if (pEntity->m_bIsInSafePosition)
                {
                    pEntity->UnsetIsInSafePosition();
                }
                pEntity->ApplyForce(vecEntityMoveForce, vecEntityMovingDirection, true);
            }

            float fCollisionImpact1 = *pThisDamageIntensity / fThisMass;
            AudioEngine.ReportCollision(pThis, pEntity, pColPoint->m_nSurfaceTypeA, pColPoint->m_nSurfaceTypeB, pColPoint,
                &pColPoint->m_vecNormal, fCollisionImpact1, 1.0f, false, false);
         
            fCollisionImpact1 = *pEntityDamageIntensity / fEntityCollisionMass;
            AudioEngine.ReportCollision(pEntity, pThis, pColPoint->m_nSurfaceTypeB, pColPoint->m_nSurfaceTypeA, pColPoint,
                &pColPoint->m_vecNormal, fCollisionImpact1, 1.0f, false, false);
            return true;
        }
        return false;
    }

    if (pEntity->physicalFlags.bDisableTurnForce)
    {
        CVector vecThisMovingDirection = pColPoint->m_vecPoint - pThis->m_matrix->pos;
        CVector vecThisSpeed;
        pThis->GetSpeed(&vecThisSpeed, vecThisMovingDirection);

        if (!pThis->physicalFlags.b27 && pThis->m_nType == ENTITY_TYPE_VEHICLE && pColPoint->m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT)
        {
            CVector outSpeed;
            pThisVehicle->AddMovingCollisionSpeed(&outSpeed, vecThisMovingDirection);
            vecThisSpeed += outSpeed;
        }

        float fThisMoveSpeedSum = vecThisSpeed.z * pColPoint->m_vecNormal.z
            + vecThisSpeed.y * pColPoint->m_vecNormal.y
            + vecThisSpeed.x * pColPoint->m_vecNormal.x;
        float fEntityMoveSpeedSum = pEntity->m_vecMoveSpeed.z * pColPoint->m_vecNormal.z
            + pEntity->m_vecMoveSpeed.y * pColPoint->m_vecNormal.y
            + pColPoint->m_vecNormal.x * pEntity->m_vecMoveSpeed.x;

        CVector vecThisDifference = (vecThisMovingDirection - vecThisCentreOfMassMultiplied);
        CVector vecThisCrossProduct;
        vecThisCrossProduct.Cross(vecThisDifference, pColPoint->m_vecNormal);
        float squaredMagnitude = vecThisCrossProduct.SquaredMagnitude();
        float fThisCollisionMass = 0.0f;
        if (pThis->physicalFlags.bDisableMoveForce)
        {
            fThisCollisionMass = squaredMagnitude / (fThisMassFactor * pThis->m_fTurnMass);
        }
        else
        {
            fThisCollisionMass = squaredMagnitude / (fThisMassFactor * pThis->m_fTurnMass) + 1.0f / (fThisMassFactor * pThis->m_fMass);
        }

        fThisCollisionMass = 1.0f / fThisCollisionMass;

        float fEntityMass = fEntityMassFactor * pEntity->m_fMass;
        float fMoveSpeed = (fEntityMass * fEntityMoveSpeedSum + fThisCollisionMass * fThisMoveSpeedSum) / (fEntityMass + fThisCollisionMass);
        float fThisMoveSpeedDifference = fThisMoveSpeedSum - fMoveSpeed;
        if (fThisMoveSpeedDifference >= 0.0f)
        {
            return false;
        }

        float fThisMoveSpeedElasticity = 0.0f;
        float fEntityMoveSpeedElasticity = 0.0f;

        float fTheElasticity = (pEntity->m_fElasticity + pThis->m_fElasticity) * 0.5f;
        if (pThis->m_bHasHitWall)
        {
            fThisMoveSpeedElasticity = fMoveSpeed;
        }
        else
        {
            fThisMoveSpeedElasticity = fMoveSpeed - fTheElasticity * fThisMoveSpeedDifference;
        }

        if (pEntity->m_bHasHitWall)
        {
            fEntityMoveSpeedElasticity = fMoveSpeed;
        }
        else
        {
            fEntityMoveSpeedElasticity = fMoveSpeed - (fEntityMoveSpeedSum - fMoveSpeed) * fTheElasticity;
        }

        *pThisDamageIntensity = (fThisMoveSpeedElasticity - fThisMoveSpeedSum) * fThisCollisionMass;
        *pEntityDamageIntensity = -((fEntityMoveSpeedElasticity - fEntityMoveSpeedSum) * fEntityMass);

        CVector vecThisMoveForce = pColPoint->m_vecNormal * (*pThisDamageIntensity / fThisMassFactor);
        CVector vecEntityMoveForce = pColPoint->m_vecNormal * (*pEntityDamageIntensity / fEntityMassFactor) * -1.0f;

        if (!pThis->physicalFlags.bDisableCollisionForce && !bEntitySomePedIsThis)
        {
            if (vecThisMoveForce.z < 0.0)
            {
                vecThisMoveForce.z = 0.0;
            }
            pThis->ApplyForce(vecThisMoveForce, vecThisMovingDirection, true);
        }

        if (!pEntity->physicalFlags.bDisableCollisionForce)
        {
            if (vecEntityMoveForce.z < 0.0)
            {
                vecEntityMoveForce.z = 0.0;
                if (fabs(fThisMoveSpeedSum) < 0.01f)
                {
                    vecEntityMoveForce.z = 0.0f;
                    vecEntityMoveForce.x = vecEntityMoveForce.x * 0.5f;
                    vecEntityMoveForce.y = vecEntityMoveForce.y * 0.5f;
                }
            }
            if (bEntitySomePedIsThis)
            {
                vecEntityMoveForce.x = vecEntityMoveForce.x + vecEntityMoveForce.x;
                vecEntityMoveForce.y = vecEntityMoveForce.y + vecEntityMoveForce.y;
            }
            if (pEntity->m_bIsInSafePosition)
            {
                pEntity->UnsetIsInSafePosition();
            }

            pEntity->ApplyMoveForce(vecEntityMoveForce);
        }

        float fCollisionImpact1 = *pThisDamageIntensity / fThisCollisionMass;
        AudioEngine.ReportCollision(pThis, pEntity, pColPoint->m_nSurfaceTypeA, pColPoint->m_nSurfaceTypeB, pColPoint,
            &pColPoint->m_vecNormal, fCollisionImpact1, 1.0f, false, false);

        fCollisionImpact1 = *pEntityDamageIntensity / fEntityMass;
        AudioEngine.ReportCollision(pEntity, pThis, pColPoint->m_nSurfaceTypeB, pColPoint->m_nSurfaceTypeA, pColPoint,
            &pColPoint->m_vecNormal, fCollisionImpact1, 1.0f, false, false);
    }
    else
    {
        CVector vecThisMovingDirection = pColPoint->m_vecPoint - pThis->m_matrix->pos;
        CVector vecThisSpeed;
        pThis->GetSpeed(&vecThisSpeed, vecThisMovingDirection);

        if (pThis->physicalFlags.b27 && pThis->m_nType == ENTITY_TYPE_VEHICLE && pColPoint->m_nSurfaceTypeA == SURFACE_CAR_MOVINGCOMPONENT)
        {
            CVector outSpeed;
            pThisVehicle->AddMovingCollisionSpeed(&outSpeed, vecThisMovingDirection);
            vecThisSpeed += outSpeed;
        }

        CVector vecEntityMovingDirection = pColPoint->m_vecPoint - pEntity->m_matrix->pos;
        CVector vecEntitySpeed;
        pEntity->GetSpeed(&vecEntitySpeed, vecEntityMovingDirection);

        if (pEntity->physicalFlags.b27 && pEntity->m_nType == ENTITY_TYPE_VEHICLE && pColPoint->m_nSurfaceTypeB == SURFACE_CAR_MOVINGCOMPONENT)
        {
            CVector outSpeed;
            pEntityVehicle->AddMovingCollisionSpeed(&outSpeed, vecEntityMovingDirection);
            vecEntitySpeed += outSpeed;
        }

        float fThisMoveSpeedSum = vecThisSpeed.z * pColPoint->m_vecNormal.z
            + vecThisSpeed.y * pColPoint->m_vecNormal.y
            + vecThisSpeed.x * pColPoint->m_vecNormal.x;
        float fEntityMoveSpeedSum = vecEntitySpeed.z * pColPoint->m_vecNormal.z
            + vecEntitySpeed.y * pColPoint->m_vecNormal.y
            + vecEntitySpeed.x * pColPoint->m_vecNormal.x;

        CVector vecThisDifference = (vecThisMovingDirection - vecThisCentreOfMassMultiplied);
        CVector vecThisCrossProduct;
        vecThisCrossProduct.Cross(vecThisDifference, pColPoint->m_vecNormal);
        float squaredMagnitude = vecThisCrossProduct.SquaredMagnitude();
        
        float fThisCollisionMass = 0.0f;
        if (pThis->physicalFlags.bDisableMoveForce)
        {
            fThisCollisionMass = squaredMagnitude / (fThisMassFactor * pThis->m_fTurnMass);
        }
        else
        {
            fThisCollisionMass = squaredMagnitude / (fThisMassFactor * pThis->m_fTurnMass) + 1.0f / (fThisMassFactor * pThis->m_fMass);
        }

        fThisCollisionMass = 1.0f / fThisCollisionMass;

        CVector vecEntityDifference = (vecEntityMovingDirection - vecEntityCentreOfMassMultiplied);
        CVector vecEntityCrossProduct;
        vecEntityCrossProduct.Cross(vecEntityDifference, pColPoint->m_vecNormal);
        squaredMagnitude = vecEntityCrossProduct.SquaredMagnitude();

        float fEntityCollisionMass = 0.0f;
        if (pEntity->physicalFlags.bDisableMoveForce)
        {
            fEntityCollisionMass = squaredMagnitude / (fEntityMassFactor * pEntity->m_fTurnMass);
        }
        else
        {
            fEntityCollisionMass = squaredMagnitude / (fEntityMassFactor * pEntity->m_fTurnMass) + 1.0f / (fEntityMassFactor * pEntity->m_fMass);
        }

        fEntityCollisionMass = 1.0f / fEntityCollisionMass;

        float fMoveSpeed = (fEntityCollisionMass * fEntityMoveSpeedSum + fThisCollisionMass * fThisMoveSpeedSum) / (fEntityCollisionMass + fThisCollisionMass);
        float fThisMoveSpeedDifference = fThisMoveSpeedSum - fMoveSpeed;
        if (fThisMoveSpeedDifference >= 0.0f)
        {
            return false;
        }

        float fThisMoveSpeedElasticity = 0.0f;
        float fEntityMoveSpeedElasticity = 0.0f;
        float fTheElasticity = (pEntity->m_fElasticity + pThis->m_fElasticity) * 0.5f;
        if (pThis->m_bHasHitWall)
        {
            fThisMoveSpeedElasticity = fMoveSpeed;
        }
        else
        {
            fThisMoveSpeedElasticity = fMoveSpeed - fTheElasticity * fThisMoveSpeedDifference;
        }

        if (pEntity->m_bHasHitWall)
        {
            fEntityMoveSpeedElasticity = fMoveSpeed;
        }
        else
        {
            fEntityMoveSpeedElasticity = fMoveSpeed - (fEntityMoveSpeedSum - fMoveSpeed) * fTheElasticity;
        }

        *pThisDamageIntensity = (fThisMoveSpeedElasticity - fThisMoveSpeedSum) * fThisCollisionMass;
        *pEntityDamageIntensity = -((fEntityMoveSpeedElasticity - fEntityMoveSpeedSum) * fEntityCollisionMass);

        CVector vecThisMoveForce = pColPoint->m_vecNormal * (*pThisDamageIntensity / fThisMassFactor);
        CVector vecEntityMoveForce = pColPoint->m_vecNormal * (*pEntityDamageIntensity / fEntityMassFactor) * -1.0f;

        if (pThis->m_nType == ENTITY_TYPE_VEHICLE && !pThis->m_bHasHitWall && !pThis->physicalFlags.bDisableCollisionForce)
        {
            if (pColPoint->m_vecNormal.z < 0.69999999f)
            {
                vecThisMoveForce.z *= 0.30000001f;
            }

            if (!pThis->m_nStatus)
            {
                vecThisMovingDirection *= 0.80000001f;
            }

            if (CWorld::bNoMoreCollisionTorque)
            {
                CVector vecFrictionForce = vecThisMoveForce * -0.30000001f;
                pThis->ApplyFrictionForce(vecFrictionForce, vecThisMovingDirection);
            }
        }

        if (pEntity->m_nType == ENTITY_TYPE_VEHICLE && !pEntity->m_bHasHitWall && !pEntity->physicalFlags.bDisableCollisionForce)
        {
            if ((pColPoint->m_vecNormal.z * -1.0f) < 0.69999999f)
            {
                vecEntityMoveForce.z *= 0.30000001f;
            }

            if (!pEntity->m_nStatus)
            {
                vecEntityMovingDirection *= 0.80000001f;
            }

            if (CWorld::bNoMoreCollisionTorque)
            {
                CVector vecFrictionForce = vecEntityMoveForce * -0.30000001f;
                pEntity->ApplyFrictionForce(vecFrictionForce, vecEntityMovingDirection);
            }
        }

        if (CCheat::m_aCheatsActive[CHEAT_CARS_FLOAT_AWAY_WHEN_HIT])
        {
            if (FindPlayerVehicle(-1, false) == pThisVehicle 
                && pEntity->m_nType == ENTITY_TYPE_VEHICLE && pEntityVehicle->m_nCreatedBy != MISSION_VEHICLE)
            {
                pEntity->physicalFlags.bApplyGravity = false;
            }
            if (FindPlayerVehicle(-1, 0) == pEntityVehicle
                && pThis->m_nType == ENTITY_TYPE_VEHICLE && pThisVehicle->m_nCreatedBy != MISSION_VEHICLE)
            {
                pThis->physicalFlags.bApplyGravity = false;
            }
        }

        if (!pThis->physicalFlags.bDisableCollisionForce)
        {
            pThis->ApplyForce(vecThisMoveForce, vecThisMovingDirection, true);
        }

        if (!pEntity->physicalFlags.bDisableCollisionForce)
        {
            if (pEntity->m_bIsInSafePosition)
            {
                pEntity->UnsetIsInSafePosition();
            }
            pEntity->ApplyForce(vecEntityMoveForce, vecEntityMovingDirection, true);
        }

        float fCollisionImpact1 = *pThisDamageIntensity / fThisCollisionMass;
        AudioEngine.ReportCollision(pThis, pEntity, pColPoint->m_nSurfaceTypeA, pColPoint->m_nSurfaceTypeB, pColPoint,
            &pColPoint->m_vecNormal, fCollisionImpact1, 1.0f, false, false);

        fCollisionImpact1 = *pEntityDamageIntensity / fEntityCollisionMass;
        AudioEngine.ReportCollision(pEntity, pThis, pColPoint->m_nSurfaceTypeB, pColPoint->m_nSurfaceTypeA, pColPoint, 
            &pColPoint->m_vecNormal, fCollisionImpact1, 1.0f, false, false);
    }
    return true;
}