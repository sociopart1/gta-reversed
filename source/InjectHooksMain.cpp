#include "StdInc.h"
#include "detours.h"

#pragma comment(lib, "detours.lib")

auto OLD_CPedIntelligence_TestForStealthKill = (bool (__thiscall*)(CPedIntelligence *pThis, CPed *pPed, char bFullTest))0x601E00;
//auto OLD_CPedIntelligence_SetTaskDuckSecondary = (void (__thiscall*)(CPedIntelligence *pThis, __int16 nLengthOfDuck))0x0;
// auto OLD_CPedIntelligence_SetPedDecisionMakerType = (void (__thiscall*)(CPedIntelligence *pThis, int newType))0x0;
// auto OLD_CPedIntelligence_Respects = (bool (__thiscall*)(CPedIntelligence *pThis, CPed *pPed))0x0;
// auto OLD_CPedIntelligence_RecordEventForScript = (void (__thiscall*)(CPedIntelligence *pThis, int eventId, int eventPriority))0x0;
// auto OLD_CPedIntelligence_ProcessStaticCounter = (void (__thiscall*)(CPedIntelligence *pThis))0x0;
// auto OLD_CPedIntelligence_ProcessFirst = (void (__thiscall*)(CPedIntelligence *pThis))0x0;
// auto OLD_CPedIntelligence_ProcessAfterProcCol = (void (__thiscall*)(CPedIntelligence *pThis))0x0;
// auto OLD_CPedIntelligence_ProcessAfterPreRender = (void (__thiscall*)(CPedIntelligence *pThis))0x0;
// auto OLD_CPedIntelligence_Process = (void (__thiscall*)(CPedIntelligence *pThis))0x0;
// auto OLD_CPedIntelligence_LookAtInterestingEntities = (void (__thiscall*)(void *pThis))0x0;
// auto OLD_CPedIntelligence_IsRespondingToEvent = (bool (__thiscall*)(CPedIntelligence *pThis, int eventType))0x0;
// auto OLD_CPedIntelligence_IsPedGoingForCarDoor = (bool (__thiscall*)(CPedIntelligence *pThis))0x0;

bool __fastcall CPedIntelligence_TestForStealthKill (CPedIntelligence* pThis, void* padding, CPed* pPed, char bFullTest);

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
    DetourAttach(&(PVOID&)OLD_CPedIntelligence_TestForStealthKill, CPedIntelligence_TestForStealthKill );
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

bool __fastcall CPedIntelligence_TestForStealthKill(CPedIntelligence* pThis, void* padding, CPed* pTarget, char bFullTest)
{
    printf(" calling CPedIntelligence_TestForStealthKill \n");

    bool result; // al
    int acquaintancesID4; // ebp
    int acquaintancesID3; // edi
    CPedGroup* pPedGroup; // eax
    CGroupEventHandler* pGroupEventHandler; // ecx
    CVector bonePosition; // [esp+8h] [ebp-18h]
    CVector vecOutput; // [esp+14h] [ebp-Ch]

    if (pTarget->bInVehicle)
    {
        return 0;
    }

    bonePosition.x = 0.0;
    bonePosition.y = 0.0;
    bonePosition.z = 0.0;

    pTarget->GetBonePosition((RwV3d &)bonePosition, BONE_HEAD, 0);

    if (pTarget->bIsDucking || pTarget->m_fHealth < 1.0)
    {
        goto RETURN_FALSE;
    }

    CMatrixLink* pTargetMatrix = pTarget->m_matrix;
    CVector* pTargetPos = &pTarget->m_placement.m_vPosn;
    if (pTargetMatrix)
    {
        pTargetPos = &pTargetMatrix->pos;
    }
    if (bonePosition.z < pTargetPos->z)
    {
        goto RETURN_FALSE;
    }
    if (bFullTest)
    {
        goto RETURN_TRUE;
    }

    if (pTarget->m_nMoveState >= PEDMOVE_RUN)
    {
        goto RETURN_FALSE;
    }

    CVector* pPedPos = &pThis->m_pPed->m_placement.m_vPosn;
    CMatrixLink* pPedMatrix = pThis->m_pPed->m_matrix;
    if (pPedMatrix)
    {
        pPedPos = &pPedMatrix->pos;
    }

    VectorSub(&vecOutput, pTargetPos, pPedPos);
    if (CPedIntelligence::STEALTH_KILL_RANGE * CPedIntelligence::STEALTH_KILL_RANGE < vecOutput.Dot())
    {
        goto RETURN_FALSE;
    }
    if (vecOutput.y * pTargetMatrix->up.y
        + vecOutput.z * pTargetMatrix->up.z
        + vecOutput.x * pTargetMatrix->up.x <= 0.0)
    {
        goto RETURN_FALSE;
    }

    CTask* pActiveTask = pTarget->m_pIntelligence->m_TaskMgr.GetActiveTask();
    if (pActiveTask)
    {
        if (pActiveTask->GetId() == TASK_COMPLEX_KILL_PED_ON_FOOT)
        {
            auto pTaskComplexKillPedOnFoot = (CTaskComplexKillPedOnFoot*)pActiveTask;
            if(pTaskComplexKillPedOnFoot->m_pTarget == pThis->m_pPed)
            {
                goto RETURN_FALSE;
            }
        }
    }

    CEvent* pCurrentEvent = pTarget->m_pIntelligence->m_eventHandler.m_history.GetCurrentEvent();
    if (pCurrentEvent && pCurrentEvent->GetSourceEntity() == (CEntity*)pThis->m_pPed
        && ((acquaintancesID4 = pTarget->m_acquaintance.GetAcquaintances(4),
            CPedType::GetPedFlag((ePedType)pThis->m_pPed->m_nPedType) & acquaintancesID4)
            || (acquaintancesID3 = pTarget->m_acquaintance.GetAcquaintances(3),
                CPedType::GetPedFlag((ePedType)pThis->m_pPed->m_nPedType) & acquaintancesID3))
        || (pPedGroup = CPedGroups::GetPedsGroup(pTarget)) != 0
        && (pGroupEventHandler = (CGroupEventHandler*)pPedGroup->m_groupIntelligence.m_pGroupEventHandler) != 0
        && (pGroupEventHandler->GetSourceEntity() == (CEntity*)pThis->m_pPed
        && ((acquaintancesID4 && CPedType::GetPedFlag((ePedType)pThis->m_pPed->m_nPedType) & acquaintancesID4)
            || (acquaintancesID3 && CPedType::GetPedFlag((ePedType)pThis->m_pPed->m_nPedType) & acquaintancesID3)))
        )
    {
    RETURN_FALSE:
        result = 0;
    }
    else
    {
    RETURN_TRUE:
        result = 1;
    }
    return result;
}