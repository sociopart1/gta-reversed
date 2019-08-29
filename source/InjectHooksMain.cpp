#include "StdInc.h"
#include "detours.h"

#pragma comment(lib, "detours.lib")


auto OLD_CPedIntelligence_FlushImmediately = (void (__thiscall*)(CPedIntelligence *pThis, char bSetPrimaryDefaultTask))0x601640;
//auto OLD_CPedIntelligence_FlushIntelligence = (void (__thiscall*)(CPedIntelligence *pThis))0x0;
//auto OLD_CPedIntelligence_GetEffectInUse = (int (__thiscall*)(CPedIntelligence *pThis))0x0;
//auto OLD_CPedIntelligence_GetMoveStateFromGoToTask = (int (__thiscall*)(CPedIntelligence *pThis))0x0;
//auto OLD_CPedIntelligence_GetTaskClimb = (CTask *(__thiscall*)(CPedIntelligence *pThis);
//auto OLD_CPedIntelligence_GetTaskDuck = (CTask *(__thiscall*)(CPedIntelligence *pThis, char IgnoreCheckingForSimplestActiveTask))0x0;
//auto OLD_CPedIntelligence_GetTaskFighting = (CTaskSimpleFight *(__thiscall*)(CPedIntelligence *pThis))0x0;
//auto OLD_CPedIntelligence_GetTaskHold = (CTaskSimpleHoldEntity *(__thiscall*)(CPedIntelligence *pThis, char IgnoreCheckingForSimplestActiveTask))0x0;
//auto OLD_CPedIntelligence_GetTaskInAir = (CTask *(__thiscall*)(CPedIntelligence *pThis))0x0;
//auto OLD_CPedIntelligence_GetTaskJetPack = (CTask *(__thiscall*)(CPedIntelligence *pThis))0x0;
//auto OLD_CPedIntelligence_GetTaskSwim = (CTask *(__thiscall*)(CPedIntelligence *pThis))0x0;
//auto OLD_CPedIntelligence_GetTaskThrow = (CTask *(__thiscall*)(CPedIntelligence *pThis))0x0;
//auto OLD_CPedIntelligence_GetTaskUseGun = (CTaskSimpleUseGun *(__thiscall*)(CPedIntelligence *pThis))0x0;
//auto OLD_CPedIntelligence_GetUsingParachute = (bool (__thiscall*)(CPedIntelligence *pThis))0x0;
//auto OLD_CPedIntelligence_IsFriendlyWith = (bool (__thiscall*)(CPedIntelligence *pThis, CPed *pPed))0x0;
//auto OLD_CPedIntelligence_IsInSeeingRange = (bool (__thiscall*)(CPedIntelligence *pThis, CVector *pPosition))0x0;
//auto OLD_CPedIntelligence_IsInterestingEntity = (bool (__thiscall*)(CPedIntelligence *pThis, CEntity *pEntity))0x0;

void __fastcall CPedIntelligence_FlushImmediately(CPedIntelligence* pThis, void* padding, bool bSetPrimaryDefaultTask);

void __cdecl HOOK_THEFUNCTION();

void InjectHooksMain(void)
{
    /*CAnimManager::InjectHooks();
    pTaskManager->InjectHooks();
    std::printf("okay, only CAnimManager and CTaskManager hooks\n ");
    CStreaming::InjectHooks();
    CRenderer::InjectHooks();*/

    //InjectHook(0x0681C10, &HOOK_THEFUNCTION, PATCH_JUMP);


    ///*
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    std::printf("GOING TO HOOK FUNC NOW\n");
    DetourAttach(&(PVOID&)OLD_CPedIntelligence_FlushImmediately, CPedIntelligence_FlushImmediately);
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


void __fastcall CPedIntelligence_FlushImmediately(CPedIntelligence* pThis, void* padding, bool bSetPrimaryDefaultTask)
{
    printf(" calling CPedIntelligence_FlushImmediately\n");

    bool bIsEntityVisible = false; 

    CTask* pPrimaryTask = pThis->m_TaskMgr.m_aPrimaryTasks[3];
    CTaskSimpleHoldEntity* pTaskSimpleHoldEntityCloned = 0;
    CTaskComplex* pTaskComplexBeInGroup = nullptr;
    if (pPrimaryTask && pPrimaryTask->GetId() == TASK_COMPLEX_BE_IN_GROUP)
    {
        pTaskComplexBeInGroup = (CTaskComplex*)pPrimaryTask->Clone();
    }

    CTaskSimpleHoldEntity* pTaskSimpleHoldEntity = nullptr;
    CTaskManager* pTaskManager = &pThis->m_TaskMgr;
    CTask* pSecondaryTask = pTaskManager->GetTaskSecondary(4);
    if (pSecondaryTask && pSecondaryTask->GetId() == TASK_SIMPLE_HOLD_ENTITY)
    {
        pTaskSimpleHoldEntity = (CTaskSimpleHoldEntity*)pSecondaryTask;
    }

    int objectType = -1;
    if (!pTaskSimpleHoldEntity)
        goto LABEL_14;
    if (pTaskSimpleHoldEntity->GetId() != TASK_SIMPLE_HOLD_ENTITY)
        goto LABEL_14;
    CObject* pObjectToHold = pTaskSimpleHoldEntity->m_pObjectToHold;
    if (!pObjectToHold)
        goto LABEL_14;
    if (pObjectToHold->m_nType != ENTITY_TYPE_OBJECT)
    {
        pTaskSimpleHoldEntityCloned = (CTaskSimpleHoldEntity*)pTaskSimpleHoldEntity->Clone();
    LABEL_14:
        bIsEntityVisible = bSetPrimaryDefaultTask;
        goto LABEL_15;
    }
    objectType = pObjectToHold->m_nObjectType;
    bIsEntityVisible = pObjectToHold->m_bIsVisible;
    pTaskSimpleHoldEntityCloned = (CTaskSimpleHoldEntity*)pTaskSimpleHoldEntity->Clone();
LABEL_15:
    pSecondaryTask = pTaskManager->GetTaskSecondary(3);
    CTaskComplex* pTaskComplexFacial = nullptr;
    if (pSecondaryTask && pSecondaryTask->GetId() == TASK_COMPLEX_FACIAL)
    {
        pTaskComplexFacial = (CTaskComplex*)pSecondaryTask->Clone();
    }

    pThis->m_eventGroup.Flush(1);
    pThis->m_eventHandler.FlushImmediately();
    pTaskManager->FlushImmediately();
    CPedScriptedTaskRecord::Process();
    if (pTaskComplexBeInGroup)
    {
        CPedGroup* pPedGroup = CPedGroups::GetPedsGroup(pThis->m_pPed);
        if (!pPedGroup || pThis->m_pPed->IsPlayer())
        {
            pTaskComplexBeInGroup->DeletingDestructor(1);
        }
        else
        {
            pPedGroup->m_groupIntelligence.ComputeDefaultTasks(pThis->m_pPed);
            pTaskManager->SetTask(pTaskComplexBeInGroup, 3, 0);
        }
    }
    if (pTaskSimpleHoldEntityCloned)
    {
        if (objectType != -1)
        {
            pTaskSimpleHoldEntityCloned->m_pObjectToHold->m_nObjectType = objectType;
            if (bIsEntityVisible)
            {
                pTaskSimpleHoldEntityCloned->m_pObjectToHold->m_bIsVisible = 1;
            }
        }
        pTaskManager->SetTaskSecondary(pTaskSimpleHoldEntityCloned, 4);
        pTaskSimpleHoldEntityCloned->ProcessPed(pThis->m_pPed);
    }
    if (pTaskComplexFacial)
    {
        pTaskManager->SetTaskSecondary(pTaskComplexFacial, 3);
    }
    if (bSetPrimaryDefaultTask)
    {
        if (pThis->m_pPed->IsPlayer())
        {
            auto pTaskSimplePlayerOnFoot = (CTaskSimplePlayerOnFoot*)CTask::operator new(28);
            if (pTaskSimplePlayerOnFoot)
            {
                pTaskSimplePlayerOnFoot->Constructor();
                pTaskManager->SetTask(pTaskSimplePlayerOnFoot, 4, 0);
                return;
            }
        }
        else
        {
            if (pThis->m_pPed->m_nCreatedBy != 2)
            {
                auto pTaskComplexWander = CTaskComplexWander::GetWanderTaskByPedType(pThis->m_pPed);
                pTaskManager->SetTask(pTaskComplexWander, 4, 0);
                return;
            }

            auto pTaskSimpleStandStill = (CTaskSimpleStandStill*)CTask::operator new(32);
            if (pTaskSimpleStandStill)
            {
                pTaskSimpleStandStill->Constructor(0, 1, 0, 8.0);
                pTaskManager->SetTask(pTaskSimpleStandStill, 4, 0);
                return;
            }
        }
        pTaskManager->SetTask(0, 4, 0);
        return;
    }
 
}