#include "StdInc.h"
#include "detours.h"

#pragma comment(lib, "detours.lib")

//bool __thiscall CPhysical_ProcessShiftSectorList(CPhysical* pThis)
auto OLD_CPhysical_ProcessShiftSectorList = (bool(__thiscall*) (CPhysical * pThis, void* padding, int sectorX, int sectorY))0x0546670;
bool __fastcall CPhysical_ProcessShiftSectorList(CPhysical* pThis, void* padding, int sectorX, int sectorY);

void __cdecl HOOK_THEFUNCTION();

void InjectHooksMain(void)
{
    /*CAnimManager::InjectHooks();
    pTaskManager->InjectHooks();
    std::printf("okay, only CAnimManager and CTaskManager hooks\n ");
    CStreaming::InjectHooks();
    CRenderer::InjectHooks();*/

    //HookInstall(0x54DB10, &CPhysical::ProcessShift_Reversed, 6);

    ///*
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    std::printf("GOING TO HOOK FUNC NOW\n");
    DetourAttach(&(PVOID&)OLD_CPhysical_ProcessShiftSectorList, CPhysical_ProcessShiftSectorList);
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


bool __fastcall CPhysical_ProcessShiftSectorList(CPhysical* pThis, void* padding, int sectorX, int sectorY)
{
    printf(" calling CPhysical_ProcessShiftSectorList \n");


    CBaseModelInfo* pModelInfo = CModelInfo::ms_modelInfoPtrs[pThis->m_nModelIndex];
    float fBoundingSphereRadius = pModelInfo->m_pColModel->m_boundSphere.m_fRadius;
    float fMaxColPointDepth = 0.0;
    CVector vecNormalizedShift (0.0, 0.0, 0.0);
    CVector vecShift (0.0, 0.0, 0.0);
    CColPoint colPoints[32];
    CVector vecBoundCentre;

    pThis->GetBoundCentre(&vecBoundCentre);

    CSector* pSector = GetSector(sectorX, sectorY);
    CRepeatSector* pRepeatSector = GetRepeatSector(sectorX, sectorY);

    int totalColPointsWithAcceptableSurfaces = 0;
    int scanListIndex = 4;
    do
    {
        CPtrListDoubleLink* pDoubleLinkList = nullptr;
        switch (--scanListIndex)
        {
        case 0: 
            pDoubleLinkList = &pSector->m_buildings;
            break;
        case 1:
            pDoubleLinkList = &pRepeatSector->m_lists[0];
            break;
        case 2:
            pDoubleLinkList = &pRepeatSector->m_lists[1];
            break;
        case 3:
            pDoubleLinkList = &pRepeatSector->m_lists[2];
            break;
        }
        CPtrNodeDoubleLink* pNode = pDoubleLinkList->GetNode();
        if (pDoubleLinkList->GetNode())
        {
            do
            {
                CPhysical* pEntity = reinterpret_cast<CPhysical*>(pNode->pItem);
                CPed* pPedEntity = static_cast<CPed*>(pEntity);
                CVehicle* pVehicleEntity = static_cast<CVehicle*>(pEntity);
                pNode = pNode->pNext;

                bool bCollisionDisabled = false;
                bool bProcessEntityCollision = true;
                if (pEntity->m_nType != ENTITY_TYPE_BUILDING
                    && (pEntity->m_nType != ENTITY_TYPE_OBJECT || !pEntity->physicalFlags.bDisableCollisionForce))
                {
                    if (pThis->m_nType != ENTITY_TYPE_PED || pThis->m_nType != ENTITY_TYPE_OBJECT
                        || (!pEntity->m_bIsStatic && !pEntity->m_bIsStaticWaitingForCollision)
                        || pPedEntity->m_pedAudio.m_tempSound.m_nBankSlotId & 0x40)
                    {
                        bProcessEntityCollision = false;
                    }
                }
                if (pEntity != pThis
                    && pEntity->m_nScanCode != CWorld::ms_nCurrentScanCode
                    && pEntity->m_bUsesCollision && (!pThis->m_bHasHitWall || bProcessEntityCollision))
                {
                    if (pEntity->GetIsTouching(&vecBoundCentre, fBoundingSphereRadius))
                    {
                      
                        bool bUnknown1 = false, bUnknown2 = false, bUnknown3 = false;
                        if (pEntity->m_nType == ENTITY_TYPE_BUILDING)
                        {
                            if (pThis->physicalFlags.bDisableCollisionForce           
                                && (pThis->m_nType != ENTITY_TYPE_VEHICLE || pVehicleEntity->m_nVehicleSubClass == VEHICLE_TRAIN))
                            {
                                bCollisionDisabled = true;
                            }
                            else
                            {
                                if (pThis->m_pAttachedTo)
                                {
                                    unsigned char attachedEntityType = pThis->m_pAttachedTo->m_nType;
                                    if(attachedEntityType > ENTITY_TYPE_BUILDING && attachedEntityType < ENTITY_TYPE_DUMMY
                                        && pThis->m_pAttachedTo->physicalFlags.bDisableCollisionForce)
                                    {
                                        bCollisionDisabled = true;
                                    }
                                }
                                else if (pThis->m_pEntityIgnoredCollision == pEntity)
                                {
                                    bCollisionDisabled = true;
                                }
                                
                                else if (!pThis->physicalFlags.bDisableZ || pThis->physicalFlags.bApplyGravity)
                                {
                                    if (pThis->physicalFlags.b25)
                                    {
                                        if (pThis->m_nStatus)
                                        {
                                            if (pThis->m_nStatus != STATUS_HELI && pEntity->DoesNotCollideWithFlyers())
                                            {
                                                bCollisionDisabled = true;
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    bCollisionDisabled = true;
                                }
                            }
                        }
                        else
                        {
                            pThis->SpecialEntityPreCollisionStuff(pEntity, true, &bCollisionDisabled, &bUnknown1, &bUnknown2, &bUnknown3);
                        }
                    
                        if (pThis->m_nType == ENTITY_TYPE_PED)
                        {
                            pThis->m_nPhysicalFlags |= 0x1000u;// pThis->physicalFlags.b13 = true;
                        }

                        ///*
                        if (!bUnknown1 && !bCollisionDisabled)
                        {
                          
                            pEntity->m_nScanCode = CWorld::ms_nCurrentScanCode;
                            int totalColPointsToProcess = pThis->ProcessEntityCollision(pEntity, colPoints);
                            if (totalColPointsToProcess > 0)
                            {
                                vecShift = vecNormalizedShift;
                                //fShiftZ = vecNormalizedShift.z;
                                //fShiftX = vecNormalizedShift.x;
                                //fShiftY = vecNormalizedShift.y;
                                //pColPoint = &colPoints[0].m_vecNormal.z;// v23 = ((uchar*)&ColPoints) + 24 or + 0x18
                                CColPoint* pColPoint = &colPoints[0];
                                int colpointIndex = totalColPointsToProcess;
                                while (1)
                                {
                                    if (pColPoint->m_fDepth > 0.0)// if (depth > 0.0)
                                    {
                                        unsigned char surfaceTypeB = pColPoint->m_nSurfaceTypeB;
                                        if (surfaceTypeB < SURFACE_GRASS_MEDIUM_DRY || surfaceTypeB > SURFACE_GOLFGRASS_SMOOTH)
                                        {
                                            totalColPointsWithAcceptableSurfaces++;
                                            if (pThis->m_nType == ENTITY_TYPE_VEHICLE && pEntity->m_nType == ENTITY_TYPE_PED
                                                && pColPoint->m_vecNormal.z < 0.0)  
                                            {
                                                vecShift.x += pColPoint->m_vecNormal.x;
                                                vecShift.y += pColPoint->m_vecNormal.y;
                                                vecShift.z += pColPoint->m_vecNormal.z * 0.0;
                                                goto LABEL_67;
                                            }
                                            if (pThis->m_nType != ENTITY_TYPE_PED || pEntity->m_nType != ENTITY_TYPE_OBJECT
                                                || !pEntity->physicalFlags.bDisableMoveForce
                                                || fabs(pColPoint->m_vecNormal.z) <= 0.1)
                                            {
                                                vecShift += pColPoint->m_vecNormal;
                                            LABEL_67:                                     
                                                if (pColPoint->m_fDepth > fMaxColPointDepth)
                                                {
                                                    fMaxColPointDepth = pColPoint->m_fDepth;
                                                }
                                                goto LABEL_69;
                                            }
                                        }
                                    }
                                LABEL_69:
                                    pColPoint++;
                                    if (!--colpointIndex)
                                    {
                                        vecNormalizedShift = vecShift;
                                        goto LABEL_72;
                                    }
                                }
                            }//*/
                        }
                    }
                   vecShift = vecNormalizedShift;
                }
            LABEL_72:
                int i = 0;
            } while (pNode);
        }
    } while (scanListIndex);

    if (totalColPointsWithAcceptableSurfaces == 0)
    {
        return false;
    }

    float shiftMagnitude = vecShift.Magnitude();
    if (shiftMagnitude > 1.0)
    {   // normalize the shift boi
        float shiftMultiplier = 1.0 / shiftMagnitude;
        vecShift *= shiftMultiplier;
    }

    CVector& vecEntityPosition = pThis->GetPosition();
    if (vecShift.z >= -0.5)
    {
        if (pThis->m_nType != ENTITY_TYPE_PED)
        {
            vecEntityPosition += vecShift * fMaxColPointDepth * 1.5f;
            goto LABEL_88;
        }
        float fMoveEntityByOffset = 1.5f * fMaxColPointDepth;
        if (fMoveEntityByOffset >= 0.0049999999)
        {
            if (fMoveEntityByOffset > 0.30000001)
            {
                vecEntityPosition += vecShift * fMaxColPointDepth * 0.3f;
            }
        }
        else
        {
            vecEntityPosition += vecShift * fMaxColPointDepth * 0.0049999999;
        }
        goto LABEL_88;
    }

    vecEntityPosition += vecShift * fMaxColPointDepth * 0.75f;

LABEL_88:
    if (pThis->m_nType != ENTITY_TYPE_VEHICLE || 1.5f <= 0.0)
    {
        return true;
    }

    if (vecShift.z < 0.0)
    {
        vecShift.z = 0.0;
    }
    /*
    vecMoveSpeed.x = fShiftX * 0.0080000004 * CTimer::ms_fTimeStep;
    vecMoveSpeed.y = fShiftY * 0.0080000004 * CTimer::ms_fTimeStep;
    vecMoveSpeed.z = fShiftZ * 0.0080000004 * CTimer::ms_fTimeStep;
    pThis->m_vecMoveSpeed.x = vecMoveSpeed.x + pThis->m_vecMoveSpeed.x;
    pThis->m_vecMoveSpeed.y = vecMoveSpeed.y + pThis->m_vecMoveSpeed.y;
    pThis->m_vecMoveSpeed.z = vecMoveSpeed.z + pThis->m_vecMoveSpeed.z; */

    pThis->m_vecMoveSpeed += vecShift * 0.0080000004 * CTimer::ms_fTimeStep;
    return true;
}
