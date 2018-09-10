#include "StdInc.h"
#include "detours.h"

#pragma comment(lib, "detours.lib")

typedef int (__cdecl *hCRenderer_ScanSectorList)(unsigned int uiSector_x, unsigned int uiSector_y);
auto OLD_CRenderer_ScanSectorList = (hCRenderer_ScanSectorList)0x554840;
void __cdecl CRenderer_ScanSectorList(unsigned int uiSector_x, unsigned int uiSector_y);

void InjectHooksMain(void)
{
    //CAnimManager::InjectHooks();
   // CTaskManager::InjectHooks();
    //std::printf("okay, only CAnimManager and CTaskManager hooks\n ");
    //CStreaming::InjectHooks();
    CRenderer::InjectHooks();

    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    std::printf("GOING TO HOOK FUNC NOW\n");
    DetourAttach(&(PVOID&)OLD_CRenderer_ScanSectorList, CRenderer_ScanSectorList);
    DetourTransactionCommit();
}

void __cdecl CRenderer_ScanSectorList(unsigned int uiSector_x, unsigned int uiSector_y)
{
    bool bRequestModel = false;
    float fCameraAndSectorX = (uiSector_x - 60) * 50.0 + 25.0 - CRenderer::ms_vecCameraPosition.x;
    float fCameraAndSectorY = (uiSector_y - 60) * 50.0 + 25.0 - CRenderer::ms_vecCameraPosition.y;
    float fAngleInRadians = atan2(-fCameraAndSectorX, fCameraAndSectorY) - CRenderer::ms_fCameraHeading;
    float fCameraAndSectorDistance = fCameraAndSectorY * fCameraAndSectorY + fCameraAndSectorX * fCameraAndSectorX;
    if (fCameraAndSectorDistance < 10000.0 || fabs(CGeneral::LimitRadianAngle(fAngleInRadians)) < 0.36000001)
    {
        bRequestModel = true;
    }

    CRenderer::SetupScanLists(uiSector_x, uiSector_y);
    CPtrListDoubleLink ** pScanLists = reinterpret_cast<CPtrListDoubleLink **>(&PC_Scratch);
    const int kiMaxScanLists = 5;
    for (int scanListIndex = 0; scanListIndex < kiMaxScanLists; scanListIndex++)
    {
        CPtrListDoubleLink * pDoubleLinkList = pScanLists[scanListIndex];
        if (pDoubleLinkList)
        {
            CPtrNodeDoubleLink * pDoubleLinkNode = pDoubleLinkList->GetNode();
            while (pDoubleLinkNode)
            {
                CEntity * pLodEntity = reinterpret_cast<CEntity *>(pDoubleLinkNode->pItem);
                pDoubleLinkNode = pDoubleLinkNode->pNext;
                if (pLodEntity->m_nScanCode != CWorld::ms_nCurrentScanCode)
                {
                    pLodEntity->m_nScanCode = CWorld::ms_nCurrentScanCode;
                    pLodEntity->m_nFlags &= 0xFFFDFFFF;
                    float outDistance;
                    switch (CRenderer::SetupEntityVisibility(pLodEntity, &outDistance))
                    {
                    case 0:
                        if (pLodEntity->m_nType == ENTITY_TYPE_OBJECT)
                        {
                            CBaseModelInfo * pBaseModelInfo = CModelInfo::ms_modelInfoPtrs[pLodEntity->m_nModelIndex]->AsAtomicModelInfoPtr();
                            if (pBaseModelInfo)
                            {
                                short modelInfoFlags = pBaseModelInfo->m_nFlags & 0x7800;
                                if (modelInfoFlags == 0x2000 || modelInfoFlags == 0x2800)
                                    goto LABEL_25;
                            }
                        }
                        break;
                    case 1:
                        CRenderer::AddEntityToRenderList(pLodEntity, outDistance);
                        break;
                    case 2:
                    {
                    LABEL_25:
                        pLodEntity->m_bOffscreen = true;
                        if (pLodEntity->m_bHasPreRenderEffects)
                        {
                            CMatrixLink * pEntityLodMatrix = pLodEntity->m_matrix;
                            CVector * vecEntityPosition = &pLodEntity->m_placement.m_vPosn;
                            if (pEntityLodMatrix)
                            {
                                vecEntityPosition = &pEntityLodMatrix->pos;
                            }

                            float fDrawDistance = 30.0;
                            float fCameraAndEntityX = CRenderer::ms_vecCameraPosition.x - vecEntityPosition->x;

                            if (pLodEntity->m_nType == ENTITY_TYPE_VEHICLE)
                            {
                                CVehicle * pVehicle = static_cast<CVehicle*>(pLodEntity);
                                if (pVehicle->m_nFlags.bAlwaysSkidMarks)
                                {
                                    fDrawDistance = 200.0;
                                }
                            }

                            float fNegativeDrawDistance = -fDrawDistance;
                            if (fCameraAndEntityX > fNegativeDrawDistance && fCameraAndEntityX < fDrawDistance)
                            {
                                float fCameraAndEntityY = CRenderer::ms_vecCameraPosition.y - vecEntityPosition->y;
                                if (fCameraAndEntityY > fNegativeDrawDistance && fCameraAndEntityY < fDrawDistance)
                                {
                                    if (CRenderer::ms_nNoOfInVisibleEntities < 149)
                                    {
                                        CRenderer::ms_aInVisibleEntityPtrs[CRenderer::ms_nNoOfInVisibleEntities] = pLodEntity;
                                        CRenderer::ms_nNoOfInVisibleEntities++;
                                    }
                                }
                            }
                        }
                        break;
                    }
                    case 3:
                    {
                        if (CStreaming::ms_disableStreaming || !pLodEntity->GetIsOnScreen() || CRenderer::ms_bInTheSky)
                        {
                            break;
                        }

                        if (bRequestModel)
                        {
                            CStreamingInfo * pStreamingInfo = &CStreaming::ms_aInfoForModel[pLodEntity->m_nModelIndex];
                            if (pStreamingInfo->m_nLoadState == LOADSTATE_LOADED)
                            {
                                CStreaming::RequestModel(pLodEntity->m_nModelIndex, 0);
                                break;
                            }
                            else
                            {
                                if (!pLodEntity->IsEntityOccluded())
                                {
                                    CRenderer::m_loadingPriority = 1;
                                    CStreaming::RequestModel(pLodEntity->m_nModelIndex, 0);
                                    break;
                                }
                            }
                        }
                        if (!CRenderer::m_loadingPriority || CStreaming::ms_numModelsRequested < 1)
                        {
                            CStreaming::RequestModel(pLodEntity->m_nModelIndex, 0);
                        }
                        break;
                    }
                    default:
                        break;
                    }
                }
            }
        }
    }
}