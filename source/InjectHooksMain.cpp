#include "StdInc.h"
#include "detours.h"

#pragma comment(lib, "detours.lib")

typedef signed int (__cdecl *hCRenderer_SetupMapEntityVisibility)(CEntity *pEntity, CBaseModelInfo *pBaseModelInfo, float fDistance, bool bIsTimeInRange);
auto OLD_CRenderer_SetupMapEntityVisibility = (hCRenderer_SetupMapEntityVisibility)0x553F60;
signed int __cdecl CRenderer_SetupMapEntityVisibility(CEntity *pEntity, CBaseModelInfo *pBaseModelInfo, float fDistance, bool bIsTimeInRange);

void HOOK_CRenderer_SetupMapEntityVisibility_1();

void InjectHooksMain(void)
{
    /*CAnimManager::InjectHooks();
    CTaskManager::InjectHooks();
    std::printf("okay, only CAnimManager and CTaskManager hooks\n ");
    CStreaming::InjectHooks();
    CRenderer::InjectHooks();*/


    InjectHook(0x553F71, &HOOK_CRenderer_SetupMapEntityVisibility_1, PATCH_JUMP);

    /*DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    std::printf("GOING TO HOOK FUNC NOW\n");
    DetourAttach(&(PVOID&)OLD_CRenderer_SetupMapEntityVisibility, CRenderer_SetupMapEntityVisibility);
    DetourTransactionCommit();
    */
}


/*
dwReturnLocation:
0 means that return value should be checked of this function.
1 means continue the function and it is inside of the "if" condition
2 means continue the function and it is outside of the "if" condition
*/

signed int __cdecl SetupMapEntityVisibility_1
(
    DWORD * pReturnLocation, 
    CEntity ** ppEntityLod, RpClump **ppClump, unsigned int * pEntityFlags, 
    float * pEntityDrawDistanceMultiplied, float * pLodAndEntityDrawDistance2,

    CEntity *pEntity, CBaseModelInfo *pBaseModelInfo, float fDistance, bool bIsTimeInRange
)
{
    *pReturnLocation = 0;

    CEntity *pEntityLod; // ebp

    RpClump *pClump; // edx
    double farClipDistance; // st7
    double lodAndEntityDrawDistance; // st6
    unsigned int EntityFlags; // ecx
    double entityDrawDistance; // st7
    float entityDrawDistanceMultiplied; // [esp+Ch] [ebp+Ch]
    float lodAndEntityDrawDistance2;

    pEntityLod = pEntity->m_pLod;

    pClump = reinterpret_cast<RpClump *>(pBaseModelInfo->m_pRwObject);
    farClipDistance = pBaseModelInfo->m_pColModel->m_boundSphere.m_fRadius + CRenderer::ms_fFarClipPlane;
    entityDrawDistanceMultiplied = 20.0;
    lodAndEntityDrawDistance = TheCamera.m_fLODDistMultiplier * pBaseModelInfo->m_fDrawDistance;
    lodAndEntityDrawDistance2 = lodAndEntityDrawDistance;
    if (lodAndEntityDrawDistance >= farClipDistance)
        lodAndEntityDrawDistance2 = farClipDistance;
    EntityFlags = pEntity->m_nFlags;

    if ((signed int)EntityFlags >= 0
        && ((!CRenderer::ms_bRenderTunnels && pEntity->m_bTunnel) || (!CRenderer::ms_bRenderOutsideTunnels && !pEntity->m_bTunnel)))
    {
        return 0;
    }
    if (!pEntityLod)
    {
        entityDrawDistance = pBaseModelInfo->m_fDrawDistance;
        if (entityDrawDistance >= lodAndEntityDrawDistance2)
            entityDrawDistance = lodAndEntityDrawDistance2;
        if (entityDrawDistance > 150.0)
            entityDrawDistanceMultiplied = entityDrawDistance * 0.06666667 + 10.0;
        if (pEntity->m_nFlagsSecondByte & 1)
            lodAndEntityDrawDistance2 = CRenderer::ms_lowLodDistScale * lodAndEntityDrawDistance2;
    }

    *ppEntityLod = pEntityLod; *ppClump = pClump; *pEntityFlags = EntityFlags; 
    *pEntityDrawDistanceMultiplied = entityDrawDistanceMultiplied; *pLodAndEntityDrawDistance2 = lodAndEntityDrawDistance2;

  
    if (!pClump)
    {
        if (pEntityLod
            && pEntityLod->m_nNumLodChildren > 1u
            && entityDrawDistanceMultiplied + fDistance - 20.0 < lodAndEntityDrawDistance2)
        {
            CRenderer::AddToLodRenderList(pEntity, fDistance);
            return 3;
        }
        goto LABEL_39;
    }


    if (entityDrawDistanceMultiplied + fDistance - 20.0 >= lodAndEntityDrawDistance2)
    {
    LABEL_39:
        signed int result = 0;
        if (pEntity->m_bDontStream)//(EntityFlags & 0x80000)
        {
            return 0;
        }
        if (pClump && fDistance - 20.0 < lodAndEntityDrawDistance2)
        {
            if (!pEntity->m_pRwObject
                && (pEntity->CreateRwObject(), !pEntity->m_pRwObject))
            {
                return 0;
            }
            if ((int)pEntity->m_nFlagsUpperByte >= 0)
                return 0;
            if (!pEntity->GetIsOnScreen() || pEntity->IsEntityOccluded())
            {
                unsigned __int16 baseModelInfoFlags1 = pBaseModelInfo->m_nFlags;
                if (!(baseModelInfoFlags1 & 1))
                {
                    pBaseModelInfo->m_nAlpha = -1;
                }
                pBaseModelInfo->m_nFlags = baseModelInfoFlags1 & 0xFFFE;
                result = 0;
            }
            else
            {
                CEntity* pEntityLod1 = pEntity->m_pLod;
                pEntity->m_bDistanceFade = true; // |= 0x8000u;
                if (pEntityLod1 && pEntityLod1->m_nNumLodChildren > 1u)
                {
                    CRenderer::AddToLodRenderList(pEntity, fDistance);
                    result = 0;
                }
                else
                {
                    CRenderer::AddEntityToRenderList(pEntity, fDistance);
                    result = 0;
                }
            }
            return result;
        }
        if (fDistance - 50.0 >= lodAndEntityDrawDistance2 || !bIsTimeInRange || pEntity->m_bIsVisible == false) //(EntityFlags & 0x80u) == 0)
        {
            return 0;
        }
        if (!pEntity->m_pRwObject)
        {
            pEntity->CreateRwObject();
        }
        return 3;
    }
    

    *pReturnLocation = 2;
    return NULL;
}

DWORD RETURN_CRenderer_SetupMapEntityVisibility_1 = 0x55408F;
DWORD RETURN_CRenderer_SetupMapEntityVisibility_1_INSIDE_IF = 0x055412D;
void _declspec(naked) HOOK_CRenderer_SetupMapEntityVisibility_1()
{
    _asm
    {
        push    ebp
        mov     ebp, esp
        sub     esp, 24
        
        push    eax

        // call our function
        push    [ebp + 18h + 8] // bIsTimeInRange
        push    [ebp + 14h + 8] // distance
        push    [ebp + 10h + 8] // pBaseModelInfo
        push    [ebp + 0Ch + 8] // pEntity

        lea     eax, [ebp - 4] // lodAndEntityDrawDistance2
        push    eax
        lea     eax, [ebp - 8] // entityDrawDistanceMultiplied
        push    eax
        lea     eax, [ebp - 12] // EntityFlags
        push    eax
        lea     eax, [ebp - 16] // pClump
        push    eax
        lea     eax, [ebp - 20] // pEntityLod
        push    eax
        lea     eax, [ebp - 24] // dwReturnLocation
        push    eax
        call    SetupMapEntityVisibility_1
      
        mov     ecx, [ebp - 24] // dwReturnLocation

        cmp     ecx, 0
        jne     CONTINUE_CRenderer_SetupMapEntityVisibility_1

        add     esp, 40
        add     esp, 32
        pop     edi 
        pop     esi 
        pop     ebp
        retn

        CONTINUE_CRenderer_SetupMapEntityVisibility_1:
        // continue the function
        mov     esi, [ebp + 0Ch + 8] // pEntity
        mov     edi, [ebp + 10h + 8] // pBaseModelInfo

        mov     ecx, [ebp - 8] // entityDrawDistanceMultiplied
        mov     [ebp + 0Ch + 8], ecx // insert into pEntity a.k.a first parameter of the hooked function
        mov     ecx, [ebp - 4] // lodAndEntityDrawDistance2
        mov     [ebp + 10h + 8], ecx // insert into pBaseModelInfo a.k.a second parameter of the hooked function

        mov     edx, [ebp - 16] // pClump
    
        mov     ecx, [ebp - 24] // dwReturnLocation
        cmp     ecx, 1 // inside if statement?
        jne     OUTSIDE_IF_CRenderer_SetupMapEntityVisibility_1

        /*mov     ecx, [ebp - 12] // EntityFlags
        mov     ebp, [ebp - 20] // pEntityLod
        add     esp, 40
        pop     eax
        add     esp, 28
        jmp     RETURN_CRenderer_SetupMapEntityVisibility_1_INSIDE_IF*/

        OUTSIDE_IF_CRenderer_SetupMapEntityVisibility_1:
        mov     ecx, [ebp - 12] // EntityFlags
        mov     ebp, [ebp - 20] // pEntityLod
        add     esp, 40
        pop     eax
        add     esp, 28
        jmp     RETURN_CRenderer_SetupMapEntityVisibility_1
    }
}

/*
// WORKING HOOK.
DWORD RETURN_CRenderer_SetupMapEntityVisibility_1 = 0x0553F77;
void _declspec(naked) HOOK_CRenderer_SetupMapEntityVisibility_1()
{
    _asm
    {
        push    [esp + 4 + 14h]     // distance
        push    [esp + 8 + 10h]     // pBaseModelInfo
        push    [esp + 0Ch + 0Ch]      // pEntity
        call    SetupMapEntityVisibility_1
        add     esp, 12

        fld     dword ptr[eax + 24h]
        mov     edx, [edi + 1Ch]
        jmp     RETURN_CRenderer_SetupMapEntityVisibility_1
    }
}*/

/*
signed int __cdecl CRenderer_SetupMapEntityVisibility(CEntity *pEntity, CBaseModelInfo *pBaseModelInfo, float fDistance, bool bIsTimeInRange)
{

    CEntity *pEntityLod; // ebp

    RpClump *pClump; // edx
    double farClipDistance; // st7
    double lodAndEntityDrawDistance; // st6
    unsigned int EntityFlags; // ecx
    double entityDrawDistance; // st7
    signed int result; // eax
    unsigned int entityFlags1; // ecx
    unsigned int entityNewFlags; // ecx
    unsigned __int16 baseModelInfoFlags; // ax
    CEntity *pEntityLod1; // eax
    unsigned __int16 baseModelInfoFlags1; // ax
    float entityDrawDistanceMultiplied; // [esp+Ch] [ebp+Ch]
    float lodAndEntityDrawDistance2; // [esp+10h] [ebp+10h]


    pEntityLod = pEntity->m_pLod;

    pClump = reinterpret_cast<RpClump *>(pBaseModelInfo->m_pRwObject);
    farClipDistance = pBaseModelInfo->m_pColModel->m_boundSphere.m_fRadius + CRenderer::ms_fFarClipPlane;
    entityDrawDistanceMultiplied = 20.0;
    lodAndEntityDrawDistance = TheCamera.m_fLODDistMultiplier * pBaseModelInfo->m_fDrawDistance;
    lodAndEntityDrawDistance2 = lodAndEntityDrawDistance;
    if (lodAndEntityDrawDistance >= farClipDistance)
        lodAndEntityDrawDistance2 = farClipDistance;
    EntityFlags = pEntity->m_nFlags;

    if ((signed int)EntityFlags >= 0
        && ((!CRenderer::ms_bRenderTunnels && pEntity->m_bTunnel) || (!CRenderer::ms_bRenderOutsideTunnels && !pEntity->m_bTunnel)))
    {
        return 0;
    }
    if (!pEntityLod)
    {
        entityDrawDistance = pBaseModelInfo->m_fDrawDistance;
        if (entityDrawDistance >= lodAndEntityDrawDistance2)
            entityDrawDistance = lodAndEntityDrawDistance2;
        if (entityDrawDistance > 150.0)
            entityDrawDistanceMultiplied = entityDrawDistance * 0.06666667 + 10.0;
        if (pEntity->m_nFlagsSecondByte & 1)
            lodAndEntityDrawDistance2 = CRenderer::ms_lowLodDistScale * lodAndEntityDrawDistance2;
    }
    if (!pClump)
    {
        if (pEntityLod
            && pEntityLod->m_nNumLodChildren > 1u
            && entityDrawDistanceMultiplied + fDistance - 20.0 < lodAndEntityDrawDistance2)
        {
            CRenderer::AddToLodRenderList(pEntity, fDistance);
            return 3;
        }
        goto LABEL_39;
    }

    if (entityDrawDistanceMultiplied + fDistance - 20.0 >= lodAndEntityDrawDistance2)
    {
    LABEL_39:
        if (pEntity->m_bDontStream)//(EntityFlags & 0x80000)
        {
            return 0;
        }
        if (pClump && fDistance - 20.0 < lodAndEntityDrawDistance2)
        {
            if (!pEntity->m_pRwObject
                && (pEntity->CreateRwObject(), !pEntity->m_pRwObject))
            {
                return 0;
            }
            if ((int)pEntity->m_nFlagsUpperByte >= 0)
                return 0;
                if (!pEntity->GetIsOnScreen() || pEntity->IsEntityOccluded())
                {
                    baseModelInfoFlags1 = pBaseModelInfo->m_nFlags;
                    if (!(baseModelInfoFlags1 & 1))
                    {
                        pBaseModelInfo->m_nAlpha = -1;
                    }
                    pBaseModelInfo->m_nFlags = baseModelInfoFlags1 & 0xFFFE;
                    result = 0;
                }
                else
                {
                    pEntityLod1 = pEntity->m_pLod;
                    pEntity->m_bDistanceFade = true; // |= 0x8000u;
                    if (pEntityLod1 && pEntityLod1->m_nNumLodChildren > 1u)
                    {
                        CRenderer::AddToLodRenderList(pEntity, fDistance);
                        result = 0;
                    }
                    else
                    {
                        CRenderer::AddEntityToRenderList(pEntity, fDistance);
                        result = 0;
                    }
                }
            return result;
        }
        if (fDistance - 50.0 >= lodAndEntityDrawDistance2 || !bIsTimeInRange || pEntity->m_bIsVisible == false) //(EntityFlags & 0x80u) == 0)
        {
            return 0;
        }
        if (!pEntity->m_pRwObject)
        {
            pEntity->CreateRwObject();
        }
        return 3;
    }

    if (!pEntity->m_pRwObject)
    {
        pEntity->CreateRwObject();
        if (!pEntity->m_pRwObject)
        {
            return 0;
        }
    }

    std::printf("pEntity->m_nFlagsUpperByte : %d\n", pEntity->m_nFlagsUpperByte);

    if ((int)pEntity->m_nFlagsUpperByte >= 0)
    {
        return 0;
    }
    std::printf("okay\n");
    if (pEntity->GetIsOnScreen() && !pEntity->IsEntityOccluded())
    {
        std::printf("it is on screen\n");
        entityFlags1 = pEntity->m_nFlags;
        if (pBaseModelInfo->m_nAlpha == -1)
        {
            entityNewFlags = entityFlags1 & 0xFFFF7FFF;
        }
        else
        {
            entityNewFlags = entityFlags1 | 0x8000;
        }

        pEntity->m_nFlags = entityNewFlags;

        if (!pEntityLod)
        {
            return 1;
        }
        if (pBaseModelInfo->m_nAlpha == -1)
        {
            ++pEntityLod->m_nNumLodChildrenRendered;
        }
        if (pEntityLod->m_nNumLodChildren <= 1u)
        {
            return 1;
        }
        CRenderer::AddToLodRenderList(pEntity, fDistance);
        return 0;
    }

    baseModelInfoFlags = pBaseModelInfo->m_nFlags;

    if (!(baseModelInfoFlags & 1))
    {
        pBaseModelInfo->m_nAlpha = 0xFFu;
    }
    pBaseModelInfo->m_nFlags = baseModelInfoFlags & 0xFFFE;
    return 2;
}*/