#include "StdInc.h"
#include "detours.h"

#pragma comment(lib, "detours.lib")
//auto _rwStreamWriteVersionedChunkHeader = (RwStream * (__cdecl*)(RwStream * stream, int type, int size, int version, unsigned __int16 buildNum))0x7ED270;

auto OLD_CTaskManager_ManageTasks = (void (__thiscall*)(CTaskManager*pThis))0x681C10;
//void __fastcall CTaskManager_ManageTasks(CTaskManager* pThis, void* padding);

void __cdecl HOOK_THEFUNCTION();

void InjectHooksMain(void)
{
    /*CAnimManager::InjectHooks();
    CTaskManager::InjectHooks();
    std::printf("okay, only CAnimManager and CTaskManager hooks\n ");
    CStreaming::InjectHooks();
    CRenderer::InjectHooks();*/


    InjectHook(0x0681C10, &HOOK_THEFUNCTION, PATCH_JUMP);

   /*
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    std::printf("GOING TO HOOK FUNC NOW\n");
    DetourAttach(&(PVOID&)OLD_CRenderer_SetupMapEntityVisibility, CRenderer_SetupMapEntityVisibility);
    DetourTransactionCommit();
    */
}

enum eFunctionReturnValue 
{
    FUNCTION_RETURN = 0,
    FUNCTION_INSIDE_IF = 1,
    FUNCTION_OUTSIDE_IF = 2
};
/*
dwReturnLocation:
0 means that the function should return.
1 means continue the function and it is inside of the "if" condition
2 means continue the function and it is outside of the "if" condition
*/

signed int __cdecl CTaskManager_ManageTasks
(
    DWORD* pReturnLocation, int* piTaskIndex , CTaskManager* pThis
)
{
    *pReturnLocation = FUNCTION_RETURN;

    *piTaskIndex = 0;
    while (!pThis->m_aPrimaryTasks[*piTaskIndex])
    {
        *piTaskIndex = *piTaskIndex + 1;
        if (*piTaskIndex >= 5)
        {
            goto PROCESS_SECONDARY_TASKS;
        }
    }

    if (*piTaskIndex > -1)
    {
        printf("iTaskIndex: %d\n", *piTaskIndex);
        // Outside If
        *pReturnLocation = FUNCTION_OUTSIDE_IF;
        return 0;
    }

    PROCESS_SECONDARY_TASKS:
    // process secondary tasks
    CTask** pSecondaryTasks = (CTask * *)pThis->m_aSecondaryTasks;
    int totalSecondaryTasks = 6;
    do
    {
        CTask* pTheSecondaryTask = *pSecondaryTasks;
        if (*pSecondaryTasks)
        {
            while (1)
            {
                pThis->ParentsControlChildren(pTheSecondaryTask);
                CTask* pTheSecondaryTask3 = nullptr;
                CTask* pTheSecondaryTask2 = pTheSecondaryTask;
                do
                {
                    pTheSecondaryTask3 = pTheSecondaryTask2;
                    pTheSecondaryTask2 = (CTask*)pTheSecondaryTask2->GetSubTask();
                } while (pTheSecondaryTask2);

                if (!pTheSecondaryTask3->IsSimple())
                {
                    break;
                }
                CTaskSimple* pTheSecondaryTask5 = nullptr;
                CTask* pTheSecondaryTask4 = pTheSecondaryTask;
                do
                {
                    pTheSecondaryTask5 = static_cast<CTaskSimple*>(pTheSecondaryTask4);
                    pTheSecondaryTask4 = (CTask*)pTheSecondaryTask4->GetSubTask();
                } while (pTheSecondaryTask4);


                if (!pTheSecondaryTask5->ProcessPed(pThis->m_pPed))
                {
                    goto LABEL_44;
                }
                pThis->SetNextSubTask(pTheSecondaryTask5->m_pParentTask);
                if (!pTheSecondaryTask->GetSubTask())
                {
                    pTheSecondaryTask->DeletingDestructor(1);
                    goto LABEL_43;
                }
            }
            pTheSecondaryTask->DeletingDestructor(1);
        LABEL_43:
            *pSecondaryTasks = 0;
        }
    LABEL_44:
        ++pSecondaryTasks;
        totalSecondaryTasks--;
    } while (totalSecondaryTasks);

    *pReturnLocation = FUNCTION_RETURN;
    return 0;
}


//DWORD RETURN_THEFUNCTION = 0x554028;
//DWORD RETURN_THEFUNCTION_INSIDE_IF = 0x055412D;
DWORD RETURN_THEFUNCTION_OUTSIDE_IF = 0x681C31;
void _declspec(naked) HOOK_THEFUNCTION()
{
    _asm
    {
        push    ecx
        push    ebx
        push    ebp
        push    esi
        push    edi

        push    ebp
        mov     ebp, esp
        sub     esp, 4 * 3

        push    ecx         // save this

        mov     [ebp - 4], ecx
        push    ecx             // this
        lea     eax, [ebp - 8]  // iTaskIndex 
        push    eax 
        lea     eax, [ebp - 12] // dwReturnLocation
        push    eax
       //*
        call    CTaskManager_ManageTasks

        mov     ecx, [ebp - 12] // dwReturnLocation

        cmp     ecx, 0
        jne     CONTINUE_THEFUNCTION_CODE
        //*/

        RETURN_THEFUNCTION_CODE:
        add     esp, 12 // clear function parameters
        pop     ecx
        add     esp, 12 // clear local variables
        mov     esp, ebp
        pop     ebp

        pop     edi
        pop     esi
        pop     ebp
        pop     ebx
        pop     ecx
        retn

        CONTINUE_THEFUNCTION_CODE:
        // continue the function

        mov     esi, [ebp - 4] // this
        mov     edi, [ebp - 8] // iTaskIndex
        mov     ecx, [ebp - 12] // dwReturnLocation
        cmp     ecx, 1 // inside if statement?
        jne     RETURN_THEFUNCTION_OUTSIDE_IF_CODE
        
        // Execute inside If case code
        jmp     RETURN_THEFUNCTION_CODE

        RETURN_THEFUNCTION_OUTSIDE_IF_CODE:
        add     esp, 12 // clear function parameters
        pop     ecx
        add     esp, 12 // clear local variables
        mov     esp, ebp
        pop     ebp
        jmp     RETURN_THEFUNCTION_OUTSIDE_IF
    }
}
