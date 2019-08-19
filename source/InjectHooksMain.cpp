#include "StdInc.h"
#include "detours.h"

#pragma comment(lib, "detours.lib")
//auto _rwStreamWriteVersionedChunkHeader = (RwStream * (__cdecl*)(RwStream * stream, int type, int size, int version, unsigned __int16 buildNum))0x7ED270;

auto OLD_CTaskManager_ManageTasks = (void (__thiscall*)(CTaskManager*pThis))0x681C10;
void __fastcall CTaskManager_ManageTasks(CTaskManager* pThis, void* padding);

void __cdecl HOOK_THEFUNCTION();

void InjectHooksMain(void)
{
    /*CAnimManager::InjectHooks();
    CTaskManager::InjectHooks();
    std::printf("okay, only CAnimManager and CTaskManager hooks\n ");
    CStreaming::InjectHooks();
    CRenderer::InjectHooks();*/


    //InjectHook(0x0681C10, &HOOK_THEFUNCTION, PATCH_JUMP);

   ///*
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    std::printf("GOING TO HOOK FUNC NOW\n");
    DetourAttach(&(PVOID&)OLD_CTaskManager_ManageTasks, CTaskManager_ManageTasks);
    DetourTransactionCommit();
    //*/
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

void __fastcall CTaskManager_ManageTasks(CTaskManager* pThis, void* padding)
{

    int iTaskIndex = 0;

    while (!pThis->m_aPrimaryTasks[iTaskIndex])
    {
        iTaskIndex = iTaskIndex + 1;
        if (iTaskIndex >= 5)
        {
            goto PROCESS_SECONDARY_TASKS;
        }
    }

    if (iTaskIndex > -1)
    {
        CTask* pTask = pThis->m_aPrimaryTasks[iTaskIndex];
        CTask* i = nullptr;
        for (i = 0; pTask; pTask = pTask->GetSubTask())
        {
            i = pTask;
        }

        if (!i->IsSimple())
        {
            CTask* pPrimaryTask = pThis->m_aPrimaryTasks[iTaskIndex];
            if (pPrimaryTask)
            {
                pPrimaryTask->DeletingDestructor(1);
            }
            pThis->m_aPrimaryTasks[iTaskIndex] = 0;
            return;
        }
        int loopCounter = 0;
        while (1)
        {
            pThis->ParentsControlChildren(pThis->m_aPrimaryTasks[iTaskIndex]);
            CTask* pTask2 = (CTask*)pThis->m_aPrimaryTasks[iTaskIndex];
            CTask* j = nullptr;
            for (j = 0; pTask2; pTask2 = pTask2->GetSubTask())
            {
                j = pTask2;
            }
            if (!j->IsSimple())
            {
                pThis->SetNextSubTask(j->m_pParentTask);
                CTask* pTask3 = pThis->m_aPrimaryTasks[iTaskIndex];
                CTask* k =  nullptr;
                for (k = 0; pTask3; pTask3 = pTask3->GetSubTask())
                {
                    k = pTask3;
                }
                if (!k->IsSimple())
                {
                    break;
                }
            }
            CTask* v13 = pThis->m_aPrimaryTasks[iTaskIndex];
            CTaskSimple* pSimpleTask = nullptr;
            for (pSimpleTask = 0; v13; v13 = v13->GetSubTask())
            {
                pSimpleTask = static_cast<CTaskSimple * >(v13);
            }
            if (!pSimpleTask->ProcessPed(pThis->m_pPed))
            {
                goto PROCESS_SECONDARY_TASKS;
            }
            pThis->SetNextSubTask (pSimpleTask->m_pParentTask);
            if (!pThis->m_aPrimaryTasks[iTaskIndex]->GetSubTask())
            {
                CTask* pTheTask = pThis->m_aPrimaryTasks[iTaskIndex];
                if (pTheTask)
                {
                    pTheTask->DeletingDestructor(1);
                }
                goto LABEL_32;
            }

            loopCounter++;
            if (loopCounter > 10)
            {
                goto PROCESS_SECONDARY_TASKS;
            }
        }

        CTask* pTheTask = pThis->m_aPrimaryTasks[iTaskIndex];
        if (pTheTask)
        {
            pTheTask->DeletingDestructor(1);
        }
    LABEL_32:
        pThis->m_aPrimaryTasks[iTaskIndex] = 0;
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

}

