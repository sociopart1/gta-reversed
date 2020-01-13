#include "StdInc.h"
#include "detours.h"

#pragma comment(lib, "detours.lib")

//bool __thiscall CPhysical_ProcessCollisionSectorList(CPhysical* pThis)
auto OLD_CPhysical_ProcessCollisionSectorList = (bool(__thiscall*) (CPhysical * pThis, int sectorX, int sectorY))0x54BA60;
bool __fastcall CPhysical_ProcessCollisionSectorList(CPhysical* pThis, void* padding, int sectorX, int sectorY);

void __cdecl HOOK_THEFUNCTION();

void InjectHooksMain(void)
{
    /*CAnimManager::InjectHooks();
    pTaskManager->InjectHooks();
    std::printf("okay, only CAnimManager and CTaskManager hooks\n ");
    CStreaming::InjectHooks();
    CRenderer::InjectHooks();*/

    //HookInstall(0x6F86A0, &CTrain::ProcessControl_Reversed, 7);

    ///*
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    std::printf("GOING TO HOOK FUNC NOW\n");
    DetourAttach(&(PVOID&)OLD_CPhysical_ProcessCollisionSectorList, CPhysical_ProcessCollisionSectorList);
    DetourTransactionCommit();
    //*/
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

bool __fastcall CPhysical_ProcessCollisionSectorList(CPhysical* pThis, void* padding, int sectorX, int sectorY)
{
    printf(" calling CPhysical_ProcessCollisionSectorList \n");
    static CColPoint colPoints[32];

    bool bResult = false;

    bool bCollisionDisabled = 0;
    bool bCollidedEntityCollisionIgnored = 0;
    bool bCollidedEntityUnableToMove = 0;
    bool bThisOrCollidedEntityStuck = 0;

    float fThisDamageIntensity = -1.0;
    float fEntityDamageIntensity = -1.0;
    float fThisMaxDamageIntensity = 0.0;
    float fEntityMaxDamageIntensity = 0.0;

    CBaseModelInfo* pModelInfo = CModelInfo::ms_modelInfoPtrs[pThis->m_nModelIndex];
    float fBoundingSphereRadius = pModelInfo->m_pColModel->m_boundSphere.m_fRadius;

    CVector vecBoundCentre;
    pThis->GetBoundCentre(&vecBoundCentre);

    CSector* pSector = GetSector(sectorX, sectorY);
    CRepeatSector* pRepeatSector = GetRepeatSector(sectorX, sectorY);

    int scanListIndex = 4;
    do
    {
        CPtrListDoubleLink* pDoubleLinkList = nullptr; 
        --scanListIndex;
        switch (scanListIndex)
        {
        case 0:
            pDoubleLinkList = &pRepeatSector->m_lists[1];
            break;
        case 1:
            pDoubleLinkList = &pRepeatSector->m_lists[2];
            break;
        case 2:
            pDoubleLinkList = &pRepeatSector->m_lists[0];
            break;
        case 3:
            pDoubleLinkList = &pSector->m_buildings;
            break;
        }
        CPtrNodeDoubleLink* pNode = pDoubleLinkList->GetNode();
        if (pNode)
        {
            CEntity* pEntity = nullptr;

            CPhysical* pPhysicalEntity = nullptr;
            CObject* pEntityObject = nullptr;
            CPed* pEntityPed = nullptr;
            CVehicle* pEntityVehicle = nullptr;;

            CObject* pThisObject = nullptr;
            CPed* pThisPed = nullptr;
            CVehicle* pThisVehicle = nullptr;

            while (pNode)
            {
                pEntity = (CEntity*)pNode->pItem;
                pNode = pNode->pNext;

                pPhysicalEntity = static_cast<CPhysical*>(pEntity);
                pEntityObject = static_cast<CObject*>(pEntity);
                pEntityPed = static_cast<CPed*>(pEntity);
                pEntityVehicle = static_cast<CVehicle*>(pEntity);

                pThisObject = static_cast<CObject*>(pThis);
                pThisPed = static_cast<CPed*>(pThis);
                pThisVehicle = static_cast<CVehicle*>(pThis);

                if (!pEntity->m_bUsesCollision || pPhysicalEntity == pThis
                    || pEntity->m_nScanCode == CWorld::ms_nCurrentScanCode)
                {
                    goto LABEL_297;
                }
                if (!pEntity->GetIsTouching(&vecBoundCentre, fBoundingSphereRadius))
                {
                    if (pThis->m_pEntityIgnoredCollision == pEntity && pThis->m_pAttachedTo != pEntity)
                    {
                        pThis->m_pEntityIgnoredCollision = 0;
                    }
              
                    if (pEntity->m_nType > ENTITY_TYPE_BUILDING && pEntity->m_nType < ENTITY_TYPE_DUMMY
                        && pPhysicalEntity->m_pEntityIgnoredCollision == pThis && pPhysicalEntity->m_pAttachedTo != pThis)
                    {
                        pPhysicalEntity->m_pEntityIgnoredCollision = 0;
                    }
                    goto LABEL_297;
                }


                bCollisionDisabled = 0;
                bCollidedEntityCollisionIgnored = 0;
                bCollidedEntityUnableToMove = 0;
                bThisOrCollidedEntityStuck = 0;

                pThis->physicalFlags.b13 = 0;

                if (pEntity->m_nType == ENTITY_TYPE_BUILDING)
                {
                    bCollidedEntityCollisionIgnored = 0;
                    if (pThis->physicalFlags.bInfiniteMass && pThis->m_bIsStuck)
                    {
                        bThisOrCollidedEntityStuck = 1;
                    }

                    if (pThis->physicalFlags.bDisableCollisionForce
                        && (pThis->m_nType != ENTITY_TYPE_VEHICLE || pThisVehicle->m_nVehicleSubClass == VEHICLE_TRAIN))
                    {
                        bCollisionDisabled = 1;
                    }
                    else
                    {
                        if (pThis->m_pAttachedTo
                            && pThis->m_pAttachedTo->m_nType > ENTITY_TYPE_BUILDING && pThis->m_pAttachedTo->m_nType < ENTITY_TYPE_DUMMY
                            && pThis->m_pAttachedTo->physicalFlags.bDisableCollisionForce)
                        {
                            bCollisionDisabled = 1;
                        }
                        else if (pThis->m_pEntityIgnoredCollision == pEntity)
                        {
                            bCollisionDisabled = 1;
                        }
                         else if (!pThis->physicalFlags.bDisableZ || pThis->physicalFlags.bApplyGravity)
                        {
                            if (pThis->physicalFlags.b25)
                            {
                                if (pThis->m_nStatus)
                                {
                                    if (pThis->m_nStatus != STATUS_HELI && pEntity->DoesNotCollideWithFlyers())
                                    {
                                        bCollisionDisabled = 1;
                                    }
                                }
                            }
                        }
                        else
                        {
                            bCollisionDisabled = 1;
                        }
                    }
                }
                else
                {
                    pThis->SpecialEntityPreCollisionStuff(pEntity, false, &bCollisionDisabled, &bCollidedEntityCollisionIgnored, 
                                                         &bCollidedEntityUnableToMove, &bThisOrCollidedEntityStuck);
                }

                if (!pThis->m_bUsesCollision || bCollidedEntityCollisionIgnored || bCollisionDisabled)
                {
                    pEntity->m_nScanCode = CWorld::ms_nCurrentScanCode;
                    if (!bCollisionDisabled) // if collision is enabled then
                    {
                        int totalColPointsToProcess = pThis->ProcessEntityCollision(pEntity, &colPoints[0]);
                        if (pThis->physicalFlags.b17 && !bCollidedEntityCollisionIgnored && totalColPointsToProcess > 0) 
                        {
                            return 1;
                        }
                        if (!totalColPointsToProcess && pThis->m_pEntityIgnoredCollision == pEntity && pThis == FindPlayerPed(-1))
                        {
                            pThis->m_pEntityIgnoredCollision = 0;
                        }
                    }
                    goto LABEL_297;
                }

                if (pEntity->m_nType == ENTITY_TYPE_BUILDING || pPhysicalEntity->physicalFlags.bCollidable || bCollidedEntityUnableToMove)
                {
                    break;
                }

                pEntity->m_nScanCode = CWorld::ms_nCurrentScanCode;

                int totalAcceptableColPoints = 0;
                int totalColPointsToProcess = pThis->ProcessEntityCollision(pEntity, &colPoints[0]);
                if (totalColPointsToProcess > 0)
                {
                    fThisMaxDamageIntensity = 0.0;
                    fEntityMaxDamageIntensity = 0.0;
                    if (pThis->m_bHasContacted && pEntity->m_bHasContacted)
                    {
                        if (totalColPointsToProcess > 0)
                        {
                            for (int colPointIndex = 0; colPointIndex < totalColPointsToProcess; colPointIndex++)
                            {
                                CColPoint* pColPoint = &colPoints[colPointIndex];
                                if (bThisOrCollidedEntityStuck 
                                    || (pColPoint->m_nPieceTypeA >= 13 && pColPoint->m_nPieceTypeA <= 16) 
                                    || (pColPoint->m_nPieceTypeB >= 13 && pColPoint->m_nPieceTypeB <= 16))
                                {
                                    ++totalAcceptableColPoints;
                                    pThis->ApplySoftCollision(pPhysicalEntity, pColPoint, &fThisDamageIntensity, &fEntityDamageIntensity);
                                }
                                else
                                {
                                    if (pThis->ApplyCollision(pEntity, pColPoint, &fThisDamageIntensity, &fEntityDamageIntensity))
                                    {
                                        if (fThisDamageIntensity > fThisMaxDamageIntensity)
                                        {
                                            fThisMaxDamageIntensity = fThisDamageIntensity;
                                        }
                                        if (fEntityDamageIntensity > fEntityMaxDamageIntensity)
                                        {
                                            fEntityMaxDamageIntensity = fEntityDamageIntensity;
                                        }

                                        pThis->SetDamagedPieceRecord(fThisDamageIntensity, pPhysicalEntity, pColPoint, 1.0f);
                                        pPhysicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, pThis, pColPoint, -1.0f);
                                    }
                                }
                            }
                          
                            goto LABEL_155;
                        }
                    }
                    else if (pThis->m_bHasContacted)
                    {
                        pThis->m_bHasContacted = false;
                        CVector vecThisFrictionMoveSpeed = pThis->m_vecFrictionMoveSpeed;
                        CVector vecThisFrictionTurnSpeed = pThis->m_vecFrictionTurnSpeed;
                        pThis->m_vecFrictionMoveSpeed = CVector(0.0f, 0.0f, 0.0f);
                        pThis->m_vecFrictionTurnSpeed = CVector(0.0f, 0.0f, 0.0f);
                        if (totalColPointsToProcess > 0)
                        {
                            for (int colPointIndex = 0; colPointIndex < totalColPointsToProcess; colPointIndex++)
                            {
                                CColPoint* pColPoint = &colPoints[colPointIndex];
                                if (bThisOrCollidedEntityStuck
                                    || (pColPoint->m_nPieceTypeA >= 13 && pColPoint->m_nPieceTypeA <= 16)
                                    || (pColPoint->m_nPieceTypeB >= 13 && pColPoint->m_nPieceTypeB <= 16)
                                    )
                                {
                                    ++totalAcceptableColPoints;
                                    pThis->ApplySoftCollision(pPhysicalEntity, pColPoint, &fThisDamageIntensity, &fEntityDamageIntensity);
                                    if (pColPoint->m_nPieceTypeB >= 13 && pColPoint->m_nPieceTypeB <= 16)
                                    {
                                        pPhysicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, pThis, pColPoint, -1.0f);
                                    }
                                }
                                else if (pThis->ApplyCollision(pPhysicalEntity, pColPoint, &fThisDamageIntensity, &fEntityDamageIntensity))
                                {
                                    if (fThisDamageIntensity > fThisMaxDamageIntensity)
                                    {
                                        fThisMaxDamageIntensity = fThisDamageIntensity;
                                    }
                                    if (fEntityDamageIntensity > fEntityMaxDamageIntensity)
                                    {
                                        fEntityMaxDamageIntensity = fEntityDamageIntensity;
                                    }

                                    pThis->SetDamagedPieceRecord(fThisDamageIntensity, pPhysicalEntity, pColPoint, 1.0f);
                                    pPhysicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, pThis, pColPoint, -1.0f);

                                    float fSurfaceFriction = g_surfaceInfos->GetFriction(pColPoint);
                                    float fFriction = fSurfaceFriction / totalColPointsToProcess;
                                    if (pThis->m_nType == ENTITY_TYPE_VEHICLE && pEntity->m_nType == ENTITY_TYPE_VEHICLE
                                        && (pThis->m_vecMoveSpeed.Dot() > 0.02f || pThis->m_vecTurnSpeed.Dot() > 0.01f))
                                    {
                                        fFriction *= 1.0f * fThisDamageIntensity;
                                    }

                                    if (pEntity->m_bIsStatic || pEntity->m_bIsStaticWaitingForCollision)
                                    {
                                        if (pThis->ApplyFriction(fFriction, pColPoint))
                                        {
                                            pThis->m_bHasContacted = true;
                                        }
                                    }
                                    else if (pThis->ApplyFriction(pPhysicalEntity, fFriction, pColPoint))
                                    {
                                        pThis->m_bHasContacted = true;
                                        pEntity->m_bHasContacted = true;
                                    }
                                }
                            }
                        }

                        if (!pThis->m_bHasContacted)
                        {
                            pThis->m_bHasContacted = true;
                            pThis->m_vecFrictionMoveSpeed = vecThisFrictionMoveSpeed;
                            pThis->m_vecFrictionTurnSpeed = vecThisFrictionTurnSpeed;  
                        }
                    }
                    else
                    {
                        if (pEntity->m_bHasContacted)
                        {
                            pEntity->m_bHasContacted = false;
                            CVector vecEntityMoveSpeed = pPhysicalEntity->m_vecFrictionMoveSpeed;
                            CVector vecEntityFrictionTurnSpeed = pPhysicalEntity->m_vecFrictionTurnSpeed;
                            pPhysicalEntity->m_vecFrictionMoveSpeed = CVector(0.0f, 0.0f, 0.0f);
                            pPhysicalEntity->m_vecFrictionTurnSpeed = CVector(0.0f, 0.0f, 0.0f);

                            if (totalColPointsToProcess > 0)
                            {
                                for (int colPointIndex = 0; colPointIndex < totalColPointsToProcess; colPointIndex++)
                                {
                                    CColPoint* pColPoint = &colPoints[colPointIndex];
                                    if (bThisOrCollidedEntityStuck
                                        || (pColPoint->m_nPieceTypeA >= 13 && pColPoint->m_nPieceTypeA <= 16)
                                        || (pColPoint->m_nPieceTypeB >= 13 && pColPoint->m_nPieceTypeB <= 16)
                                        )
                                    {
                                        ++totalAcceptableColPoints;
                                        pThis->ApplySoftCollision(pPhysicalEntity, pColPoint, &fThisDamageIntensity, &fEntityDamageIntensity);
                                        if (pColPoint->m_nPieceTypeB >= 13 && pColPoint->m_nPieceTypeB <= 16)
                                        {
                                            pPhysicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, pThis, pColPoint, -1.0f);
                                        }
                                    }
                                    else if (pThis->ApplyCollision(pPhysicalEntity, pColPoint, &fThisDamageIntensity, &fEntityDamageIntensity))
                                    {
                                        if (fThisDamageIntensity > fThisMaxDamageIntensity)
                                        {
                                            fThisMaxDamageIntensity = fThisDamageIntensity;
                                        }

                                        if (fEntityDamageIntensity > fEntityMaxDamageIntensity)
                                        {
                                            fEntityMaxDamageIntensity = fEntityDamageIntensity;
                                        }

                                        pThis->SetDamagedPieceRecord(fThisDamageIntensity, pPhysicalEntity, pColPoint, 1.0f);
                                        pPhysicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, pThis, pColPoint,-1.0f);

                                        float fSurfaceFirction = g_surfaceInfos->GetFriction(pColPoint);
                                       
                                        float fFriction = fSurfaceFirction / totalColPointsToProcess;
                                        if (pThis->m_nType == ENTITY_TYPE_VEHICLE && pEntity->m_nType == ENTITY_TYPE_VEHICLE
                                            && (pThis->m_vecMoveSpeed.Dot() > 0.02f || pThis->m_vecTurnSpeed.Dot() > 0.01f))
                                        {
                                            fFriction *= 1.0f * fThisDamageIntensity;
                                        }

                                        if (pEntity->m_bIsStatic || pEntity->m_bIsStaticWaitingForCollision)
                                        {
                                            if (pThis->ApplyFriction(fFriction, pColPoint))
                                            {
                                                pThis->m_bHasContacted = true;
                                            }
                                        }
                                        else if (pThis->ApplyFriction(pPhysicalEntity, fFriction, pColPoint))
                                        {
                                            pThis->m_bHasContacted = true;
                                            pEntity->m_bHasContacted = true;
                                        }
                                    }
                                }
                            }

                            if (!pEntity->m_bHasContacted)
                            {
                                pEntity->m_bHasContacted = true;
                                pPhysicalEntity->m_vecFrictionMoveSpeed = vecEntityMoveSpeed;
                                pPhysicalEntity->m_vecFrictionTurnSpeed = vecEntityFrictionTurnSpeed;
                            }
                        }
                        else if (totalColPointsToProcess > 0)
                        {
                            for (int colPointIndex = 0; colPointIndex < totalColPointsToProcess; colPointIndex++)
                            {
                                CColPoint* pColPoint = &colPoints[colPointIndex];
                                if (bThisOrCollidedEntityStuck
                                    || (pColPoint->m_nPieceTypeA >= 13 && pColPoint->m_nPieceTypeA <= 16)
                                    || (pColPoint->m_nPieceTypeA >= 13 && pColPoint->m_nPieceTypeA <= 16) // BUG: I think it should be m_nPieceTypeB
                                    )
                                {
                                    ++totalAcceptableColPoints;
                                    pThis->ApplySoftCollision(pPhysicalEntity, pColPoint, &fThisDamageIntensity, &fEntityDamageIntensity);
                                    if (pColPoint->m_nPieceTypeB >= 13 && pColPoint->m_nPieceTypeB <= 16)
                                    {
                                        pPhysicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, pThis, pColPoint, -1.0f);
                                    }
                                }
                                else if (pThis->ApplyCollision(pPhysicalEntity, pColPoint, &fThisDamageIntensity, &fEntityDamageIntensity))
                                {
                                    if (fThisDamageIntensity > fThisMaxDamageIntensity)
                                    {
                                        fThisMaxDamageIntensity = fThisDamageIntensity;
                                    }

                                    if (fEntityDamageIntensity > fEntityMaxDamageIntensity)
                                    {
                                        fEntityMaxDamageIntensity = fEntityDamageIntensity;
                                    }

                                    pThis->SetDamagedPieceRecord(fThisDamageIntensity, pPhysicalEntity, pColPoint, 1.0f);
                                    pPhysicalEntity->SetDamagedPieceRecord(fEntityDamageIntensity, pThis, pColPoint, -1.0f);

                                    float fSurfaceFirction = g_surfaceInfos->GetFriction(pColPoint);
                                    float fFriction = fSurfaceFirction / totalColPointsToProcess;
                                    if (pThis->m_nType == ENTITY_TYPE_VEHICLE && pEntity->m_nType == ENTITY_TYPE_VEHICLE
                                        && (pThis->m_vecMoveSpeed.Dot() > 0.02f || pThis->m_vecTurnSpeed.Dot() > 0.01f))
                                    {
                                        fFriction *= 1.0f * fThisDamageIntensity;
                                    }

                                    if (pEntity->m_bIsStatic || pEntity->m_bIsStaticWaitingForCollision)
                                    {
                                        if (pThis->ApplyFriction(fFriction, pColPoint))
                                        {
                                            pThis->m_bHasContacted = true;
                                        }
                                    }
                                    else if (pThis->ApplyFriction(pPhysicalEntity, fFriction, pColPoint))
                                    {
                                        pThis->m_bHasContacted = true;
                                        pEntity->m_bHasContacted = true;
                                    }
                                }
                            }
                        }
                    }
                LABEL_155:
                    if (pEntity->m_nType == ENTITY_TYPE_PED && pThis->m_nType == ENTITY_TYPE_VEHICLE)
                    {
                        float fThisMoveSpeedDot = pThis->m_vecMoveSpeed.Dot();
                        if (!pEntityPed->IsPlayer() || pEntity->m_bIsStuck && pThis->m_vecMoveSpeed.Dot() > 0.0025f)
                        {
                            pEntityPed->KillPedWithCar(static_cast<CVehicle*>(pThis), fEntityMaxDamageIntensity, false);
                            goto LABEL_195;
                        }
                    }
                    
                    if (pThis->m_nType == ENTITY_TYPE_PED && pEntity->m_nType == ENTITY_TYPE_VEHICLE
                        && pEntityVehicle->m_nVehicleSubClass == VEHICLE_TRAIN
                        && (DotProduct(&pEntityVehicle->m_vecMoveSpeed, &pThis->m_vecLastCollisionImpactVelocity) > 0.2f
                            || pThisPed->bFallenDown && pEntityVehicle->m_vecMoveSpeed.Dot() > 0.0005f))
                    {
                        float fDamageIntensity = fThisMaxDamageIntensity + fThisMaxDamageIntensity;
                        pThisPed->KillPedWithCar(pEntityVehicle, fDamageIntensity, false);
                        goto LABEL_195;
                    }
                    if (pEntity->m_nType == ENTITY_TYPE_OBJECT && pThis->m_nType == ENTITY_TYPE_VEHICLE
                        && pEntity->m_bUsesCollision)
                    {
                        if (pEntityObject->m_nColDamageEffect && fEntityMaxDamageIntensity > 20.0)
                        {
                            pEntityObject->ObjectDamage(fEntityMaxDamageIntensity, &colPoints[0].m_vecPoint, &CPhysical::fxDirection, pThis, WEAPON_RUNOVERBYCAR);
                            goto LABEL_195;
                        }

                        if (pEntityObject->m_nColDamageEffect >= COL_DAMAGE_EFFECT_SMASH_COMPLETELY)
                        {
                            CVector vecResult;
                            CBaseModelInfo* pEntityModelInfo = CModelInfo::ms_modelInfoPtrs[pEntity->m_nModelIndex];
                            CColModel* pColModel = pEntityModelInfo->m_pColModel;

                            VectorSub(&vecResult, &pColModel->m_boundBox.m_vecMax, &pColModel->m_boundBox.m_vecMin);
                            vecResult = (*pEntity->m_matrix) * vecResult;
       
                            bool bObjectDamage = false;
                            if (pThis->GetPosition().z > vecResult.z)
                            {
                                bObjectDamage = true;
                            }
                            
                            CMatrix invertedMatrix;
                            if (Invert(pThis->m_matrix, &invertedMatrix))
                            {
                                 vecResult = invertedMatrix * vecResult;
                                 if(vecResult.z < 0.0f)
                                 {
                                     bObjectDamage = true;
                                 }
                            }

                            if (bObjectDamage)
                            {
                                pEntityObject->ObjectDamage(50.0f, &colPoints[0].m_vecPoint, &CPhysical::fxDirection, pThis, WEAPON_RUNOVERBYCAR); 
                            }
                            goto LABEL_195;
                        }
                    }
                    else if (pThis->m_nType == ENTITY_TYPE_OBJECT && pEntity->m_nType == ENTITY_TYPE_VEHICLE && pThis->m_bUsesCollision)
                    {
                        if (pThisObject->m_nColDamageEffect && fEntityMaxDamageIntensity > 20.0)
                        {
                            pThisObject->ObjectDamage(fEntityMaxDamageIntensity, &colPoints[0].m_vecPoint, &CPhysical::fxDirection, pEntity, WEAPON_RUNOVERBYCAR);
                            goto LABEL_195;
                        }

                        // BUG: pEntity is a vehicle here, but we are treating it as an object?
                        if (pEntityObject->m_nColDamageEffect >= COL_DAMAGE_EFFECT_SMASH_COMPLETELY)
                        {
                            CVector vecResult;
                            CColModel* pColModel = pModelInfo->m_pColModel;

                            VectorSub(&vecResult, &pColModel->m_boundBox.m_vecMax, &pColModel->m_boundBox.m_vecMin);
                            vecResult = (*pThis->m_matrix) * vecResult;

                            bool bObjectDamage = false;
                            if (vecResult.z < pEntity->GetPosition().z)
                            {
                                bObjectDamage = true;
                            }

                            CMatrix invertedMatrix;
                            if (Invert(pEntity->m_matrix, &invertedMatrix))
                            {
                                vecResult = invertedMatrix * vecResult;
                                if (vecResult.z < 0.0f)
                                {
                                    bObjectDamage = true;
                                }
                            }

                            if (bObjectDamage)
                            {
                                pThisObject->ObjectDamage(50.0f, &colPoints[0].m_vecPoint, &CPhysical::fxDirection, pEntity, WEAPON_RUNOVERBYCAR);
                            }
                            goto LABEL_195;
                        }
                    }
                LABEL_195:
                    if (pEntity->m_nStatus == STATUS_SIMPLE) 
                    {
                        pEntity->m_nStatus = STATUS_PHYSICS;
                        if (pEntity->m_nType == ENTITY_TYPE_VEHICLE)
                        {
                            CCarCtrl::SwitchVehicleToRealPhysics(pEntityVehicle);
                        }
                    }
                    if (CWorld::bSecondShift)
                    {
                        bResult = 1;
                    }
                    else if (totalColPointsToProcess > totalAcceptableColPoints)
                    {
                        return 1;
                    }
                }

                

            LABEL_297:
                if (!pNode)
                {
                    goto CONTINUE_WHILE_LOOP;
                }

            }

            pEntity->m_nScanCode = CWorld::ms_nCurrentScanCode;

            int totalColPointsToProcess = pThis->ProcessEntityCollision(pEntity, &colPoints[0]);
            if (totalColPointsToProcess <= 0)
            {
                goto LABEL_297;
            }

            int totalAcceptableColPoints = 0;
            float fThisMaxDamageIntensity = 0.0;
            CVector vecMoveSpeed = CVector(0.0f, 0.0f, 0.0f);
            CVector vecTurnSpeed = CVector(0.0f, 0.0f, 0.0f);
            if (pThis->m_bHasContacted)
            {
                if (totalColPointsToProcess > 0)
                {
                    for (int colPointIndex = 0; colPointIndex < totalColPointsToProcess; colPointIndex++)
                    {
                        CColPoint* pColPoint = &colPoints[colPointIndex];
                        if (bThisOrCollidedEntityStuck
                            || (pColPoint->m_nPieceTypeA >= 13 && pColPoint->m_nPieceTypeA <= 16))
                        {
                            pThis->ApplySoftCollision(pEntity, pColPoint, &fThisDamageIntensity);
                        }
                        else if (pThis->ApplyCollisionAlt(pPhysicalEntity, *pColPoint, fThisDamageIntensity, vecMoveSpeed, vecTurnSpeed))
                        {
                            ++totalAcceptableColPoints;
                            if (fThisDamageIntensity > fThisMaxDamageIntensity)
                            {
                                fThisMaxDamageIntensity = fThisDamageIntensity;
                            }

                            if (pThis->m_nType == ENTITY_TYPE_VEHICLE)
                            { 
                                if (pThisVehicle->m_nVehicleClass != VEHICLE_BOAT || pColPoint->m_nSurfaceTypeB != SURFACE_WOOD_SOLID)
                                {
                                    pThis->SetDamagedPieceRecord(fThisDamageIntensity, pPhysicalEntity, pColPoint, 1.0f);
                                }
                            }
                            else
                            {
                                pThis->SetDamagedPieceRecord(fThisDamageIntensity, pPhysicalEntity, pColPoint, 1.0f);
                            }
                        }
                    } 
                LABEL_263:
                    if (totalAcceptableColPoints)
                    {
                        float fSpeedFactor = 1.0f / totalAcceptableColPoints;
                        pThis->m_vecMoveSpeed += vecMoveSpeed * fSpeedFactor;
                        pThis->m_vecTurnSpeed += vecTurnSpeed * fSpeedFactor;
                        if (!CWorld::bNoMoreCollisionTorque)
                        {
                            if (!pThis->m_nStatus && pThis->m_nType == ENTITY_TYPE_VEHICLE)
                            {
                                float fThisMoveSpeedX = pThis->m_vecMoveSpeed.x;
                                if (pThis->m_vecMoveSpeed.x < 0.0f)
                                {
                                    fThisMoveSpeedX = -fThisMoveSpeedX;
                                }

                                if (fThisMoveSpeedX > 0.2f)
                                {
                                    float fThisMoveSpeedY = pThis->m_vecMoveSpeed.y;
                                    if (pThis->m_vecMoveSpeed.y < 0.0f)
                                    {
                                        fThisMoveSpeedY = -fThisMoveSpeedY;
                                    }
                                    if (fThisMoveSpeedY > 0.2f)
                                    {
                                        if (!pThis->physicalFlags.bSubmergedInWater)
                                        {
                                            pThis->m_vecFrictionMoveSpeed.x -= vecMoveSpeed.x * 0.3f / (float)totalColPointsToProcess;
                                            pThis->m_vecFrictionMoveSpeed.y -= vecMoveSpeed.y * 0.3f / (float)totalColPointsToProcess;
                                            pThis->m_vecFrictionTurnSpeed += (vecTurnSpeed * -0.3f) / (float)totalColPointsToProcess;
                                        }
                                    }
                                }
                            }
                        }
                    
                        if (pEntity->m_nType == ENTITY_TYPE_OBJECT && pEntityObject->m_nColDamageEffect && fThisMaxDamageIntensity > 20.0f)
                        {
                            pEntityObject->ObjectDamage(fThisMaxDamageIntensity, &colPoints[0].m_vecPoint, &CPhysical::fxDirection, pThis, WEAPON_UNIDENTIFIED);
                        }
                        if (!CWorld::bSecondShift)
                        {
                            return 1;
                        }
                      
                        bResult = 1;
                    }
                }
                goto LABEL_297;
            }
            if (totalColPointsToProcess <= 0)
            {
                goto LABEL_297;
            }

            for (int colPointIndex = 0; colPointIndex < totalColPointsToProcess; colPointIndex++)
            {
                CColPoint* pColPoint = &colPoints[colPointIndex];
                if (bThisOrCollidedEntityStuck
                    || (pColPoint->m_nPieceTypeA >= 13 && pColPoint->m_nPieceTypeA <= 16)
                    )
                {
                    if (pThis->ApplySoftCollision(pEntity, pColPoint, &fThisDamageIntensity)
                    && (pColPoint->m_nSurfaceTypeA != SURFACE_WHEELBASE || pColPoint->m_nSurfaceTypeB != SURFACE_WHEELBASE))
                    {
                        float fSurfaceFriction = g_surfaceInfos->GetFriction(pColPoint);
                        if (pThis->ApplyFriction(fSurfaceFriction, pColPoint))
                        {
                            pThis->m_bHasContacted = true;
                        }
                        continue;
                    }
                }
                else if (pThis->ApplyCollisionAlt(pEntity, *pColPoint, fThisDamageIntensity, vecMoveSpeed, vecTurnSpeed))
                {
                    ++totalAcceptableColPoints;
                    if (fThisDamageIntensity > fThisMaxDamageIntensity)
                    {
                        fThisMaxDamageIntensity = fThisDamageIntensity;
                    }

                    float fSurfaceFirction = g_surfaceInfos->GetFriction(pColPoint);
                    float fFriction = fSurfaceFirction / totalColPointsToProcess;
                    if (pThis->m_nType != ENTITY_TYPE_VEHICLE)
                    {
                        fFriction *= 150.0f * fThisDamageIntensity;
                        pThis->SetDamagedPieceRecord(fThisDamageIntensity, pEntity, pColPoint, 1.0f);
                        if (pThis->ApplyFriction(fFriction, pColPoint))
                        {
                            pThis->m_bHasContacted = true;
                        }
                        continue;
                    }
                    if (pThisVehicle->m_nVehicleClass != VEHICLE_BOAT || pColPoint->m_nSurfaceTypeB != SURFACE_WOOD_SOLID)
                    {
                        pThis->SetDamagedPieceRecord(fThisDamageIntensity, pEntity, pColPoint, 1.0f);
                    }
                    else
                    {
                        fFriction = 0.0f;
                    }

                    if (pThis->m_nModelIndex == MODEL_RCBANDIT)
                    {
                        fFriction *= 0.2f;
                    }
                    else
                    {
                        if (pThisVehicle->m_nVehicleClass == VEHICLE_BOAT)
                        {
                            if (pColPoint->m_vecNormal.z > 0.6f)
                            {
                                if (g_surfaceInfos->GetAdhesionGroup(pColPoint->m_nSurfaceTypeB) == ADHESION_GROUP_LOOSE
                                 || g_surfaceInfos->GetAdhesionGroup(pColPoint->m_nSurfaceTypeB) == ADHESION_GROUP_SAND)
                                {
                                    fFriction *= 3.0f;
                                }
                            }
                            else
                            {
                                fFriction = 0.0f;
                            }
                        }

                        if (pThisVehicle->m_nVehicleSubClass != VEHICLE_TRAIN)
                        {
                            if (pThis->m_nStatus == STATUS_WRECKED)
                            {
                                fFriction *= 3.0f;
                            }
                            else
                            {
                                if (pThis->m_matrix->at.z > 0.3f && pThis->m_vecMoveSpeed.Dot() < 0.02f && pThis->m_vecTurnSpeed.Dot() < 0.01f)
                                {
                                    fFriction = 0.0f;
                                }
                                else
                                {
                                    if (pThis->m_nStatus != STATUS_ABANDONED
                                        && DotProduct(&pColPoint->m_vecNormal, &pThis->m_matrix->at) >= 0.707f)
                                    {
                                    }
                                    else
                                    {
                                        fFriction = 150.0f / pThis->m_fMass * fFriction * fThisDamageIntensity;
                                    }
                                }
                            }
                        }
                    }

                    if (pThis->m_nType == ENTITY_TYPE_VEHICLE && pThisVehicle->m_nVehicleSubClass == VEHICLE_TRAIN)
                    {
                        fFriction = fFriction + fFriction;
                    }
                    if (pThis->ApplyFriction(fFriction, pColPoint))
                    {
                        pThis->m_bHasContacted = true;
                    }
                }
            }
            goto LABEL_263;
        }

    CONTINUE_WHILE_LOOP:
        const int i = 0;
    } while (scanListIndex);
    return bResult;
}
