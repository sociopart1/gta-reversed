#include "StdInc.h"
#include "detours.h"

#pragma comment(lib, "detours.lib")

typedef int(__cdecl *hCRenderer_ScanSectorList)(unsigned int uiSector_x, unsigned int uiSector_y);
auto OLD_CRenderer_ScanSectorList = (hCRenderer_ScanSectorList)0x554230;
int __cdecl CRenderer_ScanSectorList(unsigned int uiSector_x, unsigned int uiSector_y);

void InjectHooksMain(void)
{
    //CAnimManager::InjectHooks();
   // CTaskManager::InjectHooks();
    //std::printf("okay, only CAnimManager and CTaskManager hooks\n ");
    //CStreaming::InjectHooks();
    CRenderer::InjectHooks();

  /*  DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    std::printf("GOING TO HOOK FUNC NOW\n");
    DetourAttach(&(PVOID&)OLD_CRenderer_ScanSectorList, CRenderer_ScanSectorList);
    DetourTransactionCommit();*/
}

/*
int __cdecl CRenderer_ScanSectorList(unsigned int uiSector_x, unsigned int uiSector_y)
{
    struct CEntity__ {
        signed char pad20 [20];
        int32_t f20;
        signed char pad28 [4];
        uint32_t f28;
        signed char pad34[2];
        int16_t f34;
        signed char pad44[8];
        int16_t f44;
        signed char pad54[8];
        unsigned char f54;
        signed char pad1069[1014];
        unsigned char someFlags;
    };

    unsigned int uiSector_x2; // esi
    bool bRequestModel; // bl
    double fCameraAndSectorX; // st7
    long double fCameraAndSectorY; // st6
    CEntity ***entityPointer; // eax
    CEntity **ppEntity; // edi
    CEntity *pLodEntity; // esi
    bool iterationOver; // zf
    CBaseModelInfo *pBaseModelInfo; // eax
    __int16 modelInfoFlags; // ax
    __int32 entityFlags; // eax
    CMatrixLink *slodCoors; // ecx
    float *fPosX; // eax
    double fCameraAndEntityX; // st7
    CVector *vecEntityPosition; // eax
    double fCameraAndEntityY; // st7
    int nNoOfVisibleEntities; // eax
    int result; // eax
    float fNegativeDrawDistance; // [esp+Ch] [ebp-Ch]
    signed int maximumLoopIterations; // [esp+10h] [ebp-8h]
    float outDistance; // [esp+14h] [ebp-4h]
    float fAngleInRadians; // [esp+1Ch] [ebp+4h]
    float fDrawDistance; // [esp+1Ch] [ebp+4h]
    CEntity ***_PC_Scratch; // [esp+20h] [ebp+8h]

    uiSector_x2 = uiSector_x;
    bRequestModel = 0;
    fCameraAndSectorX = (uiSector_x - 60) * 50.0 + 25.0 - CRenderer::ms_vecCameraPosition.x;
    fCameraAndSectorY = (uiSector_y - 60) * 50.0 + 25.0 - CRenderer::ms_vecCameraPosition.y;
    if (fCameraAndSectorY * fCameraAndSectorY + fCameraAndSectorX * fCameraAndSectorX < 10000.0
        || (fAngleInRadians = atan2(-fCameraAndSectorX, fCameraAndSectorY) - CRenderer::ms_fCameraHeading,
            fabs(CGeneral::LimitRadianAngle(fAngleInRadians)) < 0.36000001))
    {
        bRequestModel = 1;
    }
    CRenderer::SetupScanLists(uiSector_x, uiSector_y);
    _PC_Scratch = PC_Scratch;
    maximumLoopIterations = 5;
    do
    {
        entityPointer = _PC_Scratch;
        if (*_PC_Scratch)
        {
            ppEntity = *entityPointer;
            while (ppEntity)
            {
                pLodEntity = *ppEntity;
                iterationOver = (*ppEntity)->m_nScanCode == CWorld::ms_nCurrentScanCode;
                ppEntity = (CEntity **)  ((int *)ppEntity)[1];
                if (!iterationOver)
                {
                    pLodEntity->m_nScanCode = CWorld::ms_nCurrentScanCode;
                    pLodEntity->m_nFlags &= 0xFFFDFFFF;
                    switch (CRenderer::SetupEntityVisibility(pLodEntity, &outDistance))
                    {
                    case 0:
                        if ((pLodEntity->m_nType & 7) == 4)
                        {
                            pBaseModelInfo = CModelInfo::ms_modelInfoPtrs[pLodEntity->m_nModelIndex]->AsAtomicModelInfoPtr();
                            if (pBaseModelInfo)
                            {
                                modelInfoFlags = pBaseModelInfo->m_nFlags & 0x7800;
                                if (modelInfoFlags == 0x2000 || modelInfoFlags == 0x2800)
                                    goto LABEL_25;
                            }
                        }
                        break;
                    case 1:
                        CRenderer::AddEntityToRenderList(pLodEntity, outDistance);
                        break;
                    case 2:
                    LABEL_25:
                        entityFlags = pLodEntity->m_nFlags | 0x20000;
                        pLodEntity->m_nFlags = entityFlags;
                        if (entityFlags & 0x200000)
                        {
                            slodCoors = pLodEntity->m_matrix;
                            fPosX = &slodCoors->pos.x;
                            if (!slodCoors)
                                fPosX = &pLodEntity->m_placement.m_vPosn.x;
                            fDrawDistance = 30.0;
                            fCameraAndEntityX = CRenderer::ms_vecCameraPosition.x - *fPosX;

                            // I don't know what this thing is, but I guess we need to find out later.
                            CEntity__ * pEntity__ = (CEntity__ *)pLodEntity;
                            if ((pLodEntity->m_nType & 7) == 2 && (pEntity__->someFlags & 1)) //  pLodEntity[19].m_placement.m_vPosn.x & 1))
                                fDrawDistance = 200.0;
                            fNegativeDrawDistance = -fDrawDistance;
                            if (fCameraAndEntityX > fNegativeDrawDistance && fCameraAndEntityX < fDrawDistance)
                            {
                                vecEntityPosition = &slodCoors->pos;
                                if (!slodCoors)
                                    vecEntityPosition = &pLodEntity->m_placement.m_vPosn;
                                fCameraAndEntityY = CRenderer::ms_vecCameraPosition.y - vecEntityPosition->y;
                                if (fCameraAndEntityY > fNegativeDrawDistance && fCameraAndEntityY < fDrawDistance)
                                {
                                    nNoOfVisibleEntities = CRenderer::ms_nNoOfInVisibleEntities;
                                    if (CRenderer::ms_nNoOfInVisibleEntities < 149)
                                    {
                                        CRenderer::ms_aInVisibleEntityPtrs[CRenderer::ms_nNoOfInVisibleEntities] = pLodEntity;
                                        CRenderer::ms_nNoOfInVisibleEntities = nNoOfVisibleEntities + 1;
                                    }
                                }
                            }
                        }
                        break;
                    case 3:
                        if (CStreaming::ms_disableStreaming || !pLodEntity->GetIsOnScreen() || CRenderer::ms_bInTheSky)
                            break;
                        if (CStreaming::ms_aInfoForModel[pLodEntity->m_nModelIndex].m_nLoadState == 1)
                            goto LABEL_16;
                        if (!pLodEntity->IsEntityOccluded() && bRequestModel)
                        {
                            CRenderer::m_loadingPriority = 1;
                        LABEL_16:
                            if (bRequestModel)
                                goto LABEL_43;
                        }
                        if (!CRenderer::m_loadingPriority || CStreaming::ms_numModelsRequested < 1)
                            LABEL_43:
                        CStreaming::RequestModel(pLodEntity->m_nModelIndex, 0);
                        break;
                    default:
                        break;
                    }
                }
            }
        }
        result = maximumLoopIterations - 1;
        iterationOver = maximumLoopIterations == 1;
        ++_PC_Scratch;
        --maximumLoopIterations;
    } while (!iterationOver);
    return result;
} */