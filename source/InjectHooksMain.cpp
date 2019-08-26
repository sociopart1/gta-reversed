#include "StdInc.h"
#include "detours.h"

#pragma comment(lib, "detours.lib")


//void PlayIdleAnimations(CPed* pPed)
auto OLD_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon = (void(__thiscall*)(CTaskSimplePlayerOnFoot * pThis, CPed * pPed))0x6859A0;

void __fastcall CTaskSimplePlayerOnFoot_ProcessPlayerWeapon(CTaskSimplePlayerOnFoot* pThis, void* padding, CPlayerPed* pPlayerPed);

//CTask* __cdecl CTaskManager__GetSimplestTask(CTask* pTask);
void __cdecl HOOK_THEFUNCTION();

#include <ctime>
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
    DetourAttach(&(PVOID&)OLD_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon, CTaskSimplePlayerOnFoot_ProcessPlayerWeapon);
    DetourTransactionCommit();
    // */
    /**/
    clock_t OnePressTMR = clock();

    while (1)
    {
        if (clock() - OnePressTMR > 1000)
        {
            OnePressTMR = clock();

            CPed* pLocalPlayer = (CPed*)FindPlayerPed(-1);
            if (pLocalPlayer && pLocalPlayer->m_pRwClump)
            {
                auto pAnimAssociation = RpAnimBlendClumpGetFirstAssociation(pLocalPlayer->m_pRwClump);
                while (pAnimAssociation)
                {
                    printf("animation group ID: %d | animation ID: %d | %s\n",
                        (int)pAnimAssociation->m_nAnimGroup, (int)pAnimAssociation->m_nAnimId,
                        pAnimAssociation->m_bPartial ? "partial" : "primary");

                    pAnimAssociation = RpAnimBlendGetNextAssociation(pAnimAssociation);
                }
            }
        }
    }
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

void __fastcall CTaskSimplePlayerOnFoot_ProcessPlayerWeapon(CTaskSimplePlayerOnFoot* pThis, void* padding, CPlayerPed* pPlayerPed)
{
    printf(" calling CTaskSimplePlayerOnFoot__ProcessPlayerWeapon\n");

    CPlayerData* pPlayerData = pPlayerPed->m_pPlayerData;
    CPedIntelligence* pIntelligence = pPlayerPed->m_pIntelligence;
    CTaskManager* pTaskManager = &pIntelligence->m_TaskMgr;
    CPad* pPad = pPlayerPed->GetPadFromPlayer();

    eWeaponType weaponType = pPlayerPed->m_aWeapons[pPlayerPed->m_nActiveWeaponSlot].m_nType;
    unsigned char weaponSkill = pPlayerPed->GetWeaponSkill();
    CWeaponInfo* pWeaponInfo = CWeaponInfo::GetWeaponInfo(weaponType, weaponSkill);

    if (pWeaponInfo->m_nWeaponFire == WEAPON_FIRE_MELEE)
    {
        //return OLD_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon (pThis, pPlayerPed);
    }

    //return OLD_CTaskSimplePlayerOnFoot_ProcessPlayerWeapon(pThis, pPlayerPed);

    if (pPlayerData->m_bHaveTargetSelected && !pPlayerPed->m_pTargetedObject)
    {
        TheCamera.ClearPlayerWeaponMode();
        CWeaponEffects::ClearCrossHair(pPlayerPed->m_nPedType);
    }
    pPlayerPed->m_nWeaponAccuracy = pWeaponInfo->m_nFlags.bCanAim ? 95 : 100;
    if (pPad->WeaponJustDown(pPlayerPed)
        && (pPlayerPed->m_pTargetedObject || CCamera::m_bUseMouse3rdPerson && pPlayerPed->m_p3rdPersonMouseTarget))
    {
        pPlayerPed->PlayerHasJustAttackedSomeone();
    }
    if (pPlayerPed->m_pFire || !pWeaponInfo->m_nFlags.b1stPerson)
    {
        if (!pPad->GetTarget() && weaponType == WEAPON_RLAUNCHER_HS)
        {
            pPlayerData->m_nFireHSMissilePressedTime = 0;
            pPlayerData->m_LastHSMissileTarget = 0;
        }
    }
    else
    {
        if (pPad->GetEnterTargeting()
            || TheCamera.m_bJustJumpedOutOf1stPersonBecauseOfTarget
            || pPad->GetTarget() && pThis->m_nFrameCounter < (CTimer::m_FrameCounter - 1))
        {
            unsigned int weaponId = 0;
            switch (weaponType)
            {
                case WEAPON_RLAUNCHER:
                {
                    weaponId = MODE_ROCKETLAUNCHER;
                    break;
                }
                case WEAPON_RLAUNCHER_HS:
                {
                    pPlayerData->m_nFireHSMissilePressedTime = CTimer::m_snTimeInMilliseconds;
                    pPlayerData->m_LastHSMissileTarget = 0;
                    weaponId = MODE_ROCKETLAUNCHER_HS;
                    break;
                }
                case WEAPON_SNIPERRIFLE:
                {
                    weaponId = MODE_SNIPER;
                    break;
                }
                case WEAPON_CAMERA:
                {
                    weaponId = MODE_CAMERA;
                    break;
                }
                default:
                {
                    weaponId = MODE_M16_1STPERSON;
                    break;
                }
            }
            TheCamera.SetNewPlayerWeaponMode(weaponId, 0, 0);
            pPlayerPed->SetPedState(PEDSTATE_SNIPER_MODE);
            return;
        }
        if (!TheCamera.Using1stPersonWeaponMode())
        {
            weaponType = pPlayerPed->m_aWeapons[pPlayerPed->m_nActiveWeaponSlot].m_nType;
            if (weaponType == WEAPON_RLAUNCHER || weaponType == WEAPON_RLAUNCHER_HS
                || weaponType == WEAPON_SNIPERRIFLE || weaponType == WEAPON_CAMERA)
            {
                CTaskSimpleUseGun* pSimpleTaskUseGun = pIntelligence->GetTaskUseGun();
                if (pSimpleTaskUseGun)
                {
                    pSimpleTaskUseGun->PlayerPassiveControlGun();
                }
                pPlayerPed->m_pPlayerData->m_bHaveTargetSelected = 0;
                return;
            }
        }
    }

    CTaskSimpleUseGun* pNewSimpleUseGunTask = nullptr;
    int gunCommand[4] = { 0 };

    if (pWeaponInfo->m_nWeaponFire == WEAPON_FIRE_MELEE)
    {
       // /*
        int fightCommand = 0;
        gunCommand[0] = 0;

        if (!pPlayerPed->m_pTargetedObject && !pPad->GetTarget() && !pTaskManager->GetTaskSecondary(0))
        {
            if (pPad->MeleeAttackJustDown(0))
            {
                fightCommand = 11;
                gunCommand[0] = 11;
            }

            CAnimBlendAssociation* pAnimAssoc = RpAnimBlendClumpGetAssociation(pPlayerPed->m_pRwClump, STEALTH_KN_KILL_PARTIAL);
            if (pAnimAssoc)
            {
                pAnimAssoc->m_fBlendAmount = -2.0;
            }
                
            if (fightCommand == 0)
            {
            HANDLE_FIGHTING_TASK_IF_EXISTS:
                if (pIntelligence->GetTaskFighting())
                {
                    auto pTaskSimpleFight = static_cast<CTaskSimpleFight*>(pTaskManager->GetTaskSecondary(0));
                    if (pPlayerPed->m_nMoveState == PEDMOVE_STILL && pPad->GetSprint())
                    {
                        pTaskSimpleFight->ControlFight(pPlayerPed->m_pTargetedObject, 15);
                    }
                    else
                    {
                        if (pPlayerData->m_nChosenWeapon == pPlayerPed->m_nActiveWeaponSlot)
                        {
                            pTaskSimpleFight->ControlFight(pPlayerPed->m_pTargetedObject, 0);
                        }
                        else
                        {
                            pTaskSimpleFight->ControlFight(pPlayerPed->m_pTargetedObject, 1);
                        }
                    }
                }
                goto PED_WEAPON_AIMING_CODE;
            }

            // fightCommand cannot be 19 here, so we don't need the code here.
            if (fightCommand == 19) 
            {
                // Just in case, if this executes somehow, then we probably need to add the code.
                // But It won't.
                assert(fightCommand != 19);
                //LAB_00685c62:
            } 
            
            goto EXECUTE_MELEE_ATTACK;
        }
        else
        {
            CPed* pTargetEntity = nullptr;
            if(!pPlayerPed->m_pTargetedObject)
            {
                if (CCamera::m_bUseMouse3rdPerson && pPlayerPed->m_p3rdPersonMouseTarget)
                {
                    pTargetEntity = pPlayerPed->m_p3rdPersonMouseTarget;
                }
            }
            else
            {
                if (pPlayerPed->m_pTargetedObject->m_nType == ENTITY_TYPE_PED)
                {
                    pTargetEntity = reinterpret_cast<CPed*>(pPlayerPed->m_pTargetedObject);
                }
            }

            CAnimBlendAssociation* pAnimAssociation = nullptr;
            int animGroupID = pWeaponInfo->m_dwAnimGroup;
            if (pTargetEntity && pPad->GetTarget() 
                && pPlayerData->m_fMoveBlendRatio < 1.9
                && pPlayerPed->m_nMoveState != PEDMOVE_SPRINT
                && !pTaskManager->GetTaskSecondary(0)
                && animGroupID != 0
                && CAnimManager::ms_aAnimAssocGroups[animGroupID].m_pAnimBlock
                && CAnimManager::ms_aAnimAssocGroups[animGroupID].m_pAnimBlock->bLoaded
                && pIntelligence->TestForStealthKill(pTargetEntity, 0))
            {
                if (pPlayerPed->bIsDucking)
                {
                    CTaskSimpleDuck* pDuckTask = pIntelligence->GetTaskDuck(1);
                    if (pDuckTask && pDuckTask->IsTaskInUseByOtherTasks())
                    {
                        pAnimAssociation = RpAnimBlendClumpGetFirstAssociation(pPlayerPed->m_pRwClump);
                    }
                    else
                    {
                        pAnimAssociation = CAnimManager::BlendAnimation(pPlayerPed->m_pRwClump, pWeaponInfo->m_dwAnimGroup, STEALTH_KN_KILL_PARTIAL, 8.0);
                    }
                }
                else
                {
                    pAnimAssociation = CAnimManager::BlendAnimation(pPlayerPed->m_pRwClump, pWeaponInfo->m_dwAnimGroup, STEALTH_KN_KILL_PARTIAL, 8.0);
                }
            }
            else
            {
                pAnimAssociation = RpAnimBlendClumpGetAssociation(pPlayerPed->m_pRwClump, STEALTH_KN_KILL_PARTIAL);
                if (pAnimAssociation)
                {
                    pAnimAssociation->m_fBlendAmount = -2.0;
                }
            }

            bool bCheckButtonCircleStateOnly = 0;
            if (pPlayerPed->m_pTargetedObject || pPad->GetTarget())
            {
                bCheckButtonCircleStateOnly = 1;
            }
            unsigned char meleeAttackJustDown = pPad->MeleeAttackJustDown(bCheckButtonCircleStateOnly);
            if (meleeAttackJustDown && pAnimAssociation && pAnimAssociation->m_fBlendAmount > 0.5
                && pTargetEntity && pIntelligence->TestForStealthKill(pTargetEntity, 1))
            {
                CTask* pNewTask = static_cast<CTask*>(CTask::operator new(32));
                CTaskSimpleStealthKill* pTaskSimpleStealthKill = nullptr;
                if (pNewTask)
                {
                    pTaskSimpleStealthKill = static_cast<CTaskSimpleStealthKill*>(pNewTask);
                    pTaskSimpleStealthKill->Constructor(1, pTargetEntity, pWeaponInfo->m_dwAnimGroup);
                }
                 
                pTaskManager->SetTask(pTaskSimpleStealthKill, 3, 0);

                eWeaponType activeWeaponType = pPlayerPed->m_aWeapons[pPlayerPed->m_nActiveWeaponSlot].m_nType;
                CPedDamageResponseCalculator pedDamageResponseCalculator;
                pedDamageResponseCalculator.Constructor1(pPlayerPed, 0.0, activeWeaponType, PED_PIECE_TORSO, 0);

                      
                bool bPedVehicle = pTargetEntity->m_nPedFlags >> 8 & 0xFFFFFF01;

                CEventDamage eventDamage;
                eventDamage.Constructor1(pPlayerPed, CTimer::m_snTimeInMilliseconds, activeWeaponType, PED_PIECE_TORSO, 0, 0, bPedVehicle);
                CPedDamageResponse damageResponseInfo;
                if (eventDamage.AffectsPed(pTargetEntity))
                {
                    pedDamageResponseCalculator.ComputeDamageResponse(pTargetEntity, &damageResponseInfo, 0);
                    CEvent& theEvent = reinterpret_cast<CEvent&>(eventDamage);
                    pIntelligence->m_eventGroup.Add(theEvent, 0);
                    CCrime::ReportCrime(18, pTargetEntity, pPlayerPed);
                    pPlayerPed->m_weaponAudio.AddAudioEvent(156);
                }
                pPlayerPed->ClearWeaponTarget();
                eventDamage.Destructor1();
                pedDamageResponseCalculator.Destructor1();
            }
            else
            {
                CWeapon* pActiveWeapon = &pPlayerPed->m_aWeapons[pPlayerPed->m_nActiveWeaponSlot];
                switch (meleeAttackJustDown)
                {
                    case 1:
                    {
                        fightCommand = 11;
                        gunCommand[0] = fightCommand;
                        break;
                    }
                    case 4:
                    {
                        if (!CWeaponInfo::GetWeaponInfo(pActiveWeapon->m_nType, 1)->m_nFlags.bHeavy)
                        {
                            fightCommand = 12;
                            gunCommand[0] = 12;
                        }
                        else
                        {
                            fightCommand = 11;
                            gunCommand[0] = fightCommand;
                        }
                        break;
                    }
                    case 3:
                    {
                        fightCommand = 2;
                        gunCommand[0] = fightCommand;
                        break;
                    }
                    default:
                    {
                        if (pPad->GetMeleeAttack(0) && pActiveWeapon->m_nType == WEAPON_CHAINSAW && pTaskManager->GetTaskSecondary(0))
                        {
                            fightCommand = 11;
                            gunCommand[0] = fightCommand;
                        }
                        else
                        {
                            goto HANDLE_FIGHTING_TASK_IF_EXISTS;
                        }
                    }
                };

            EXECUTE_MELEE_ATTACK:
                if (pTaskManager->GetTaskSecondary(0))
                {
                    if (pIntelligence->GetTaskFighting())
                    {
                        auto pTaskSimpleFight = static_cast<CTaskSimpleFight*>(pTaskManager->GetTaskSecondary(0));
                        pTaskSimpleFight->ControlFight(pPlayerPed->m_pTargetedObject, gunCommand[0]);
                    }
                }
                else
                {
                    CTask* pNewTask = static_cast<CTask*>(CTask::operator new(40));
                    CTaskSimpleFight* pTaskSimpleFight = nullptr;                  
                    if (pNewTask)
                    {
                        pTaskSimpleFight = static_cast<CTaskSimpleFight *>(pNewTask);
                        pTaskSimpleFight->Constructor(pPlayerPed->m_pTargetedObject, fightCommand, 2000u);
                    }

                    pTaskManager->SetTaskSecondary(pTaskSimpleFight, 0);
                }
            }
        }   
        //*/
    }
    else 
    {
        if (pWeaponInfo->m_nWeaponFire == WEAPON_FIRE_USE)
        {
            if (pPad->WeaponJustDown(0))
            {
                unsigned char activeWeaponSlot = pPlayerPed->m_nActiveWeaponSlot;
                weaponType = pPlayerPed->m_aWeapons[activeWeaponSlot].m_nType;
                CWeapon* pPlayerWeapon = &pPlayerPed->m_aWeapons[activeWeaponSlot];
                if (weaponType == WEAPON_DETONATOR)
                {
                    CMatrixLink* pPlayerMatrix = pPlayerPed->m_matrix;
                    if (pPlayerMatrix)
                    {
                        pPlayerWeapon->Fire(pPlayerPed, &pPlayerMatrix->pos, &pPlayerMatrix->pos, 0, 0, 0);
                    }
                    else
                    {
                        pPlayerWeapon->Fire(pPlayerPed, &pPlayerPed->m_placement.m_vPosn, &pPlayerPed->m_placement.m_vPosn, 0, 0, 0);
                    }
                }
                else if (weaponType > WEAPON_CAMERA && weaponType <= WEAPON_INFRARED && !pTaskManager->m_aPrimaryTasks[3])
                {
                    CTaskComplexUseGoggles* pCTaskComplexUseGoggles = nullptr;
                    CTask* pNewTask = static_cast<CTask*>(CTask::operator new(12));
                    if (pNewTask)
                    {
                        pCTaskComplexUseGoggles = static_cast<CTaskComplexUseGoggles*>(pNewTask);
                        pCTaskComplexUseGoggles->Constructor();
                    }
   
                    pTaskManager->SetTask(pCTaskComplexUseGoggles, 3, 0);
                    pPlayerPed->m_pPlayerData->m_bDontAllowWeaponChange = 1;
                }
            }
        }
        else 
        {
            if (!pPad->GetWeapon(pPlayerPed))
            {
                if (pIntelligence->GetTaskThrow())
                {
                    auto pTaskSimpleThrowProjectile = static_cast<CTaskSimpleThrowProjectile*>(pTaskManager->GetTaskSecondary(0));
                    pTaskSimpleThrowProjectile->ControlThrow(1, 0, 0);
                }
            }
            else
            {
                unsigned int nWeaponFire = pWeaponInfo->m_nWeaponFire;
                if ( pPlayerPed->m_nMoveState != PEDMOVE_SPRINT && pPlayerData->m_nChosenWeapon == pPlayerPed->m_nActiveWeaponSlot && (nWeaponFire - 1) < 4)
                {
                    switch (nWeaponFire)
                    {
                    case WEAPON_FIRE_INSTANT_HIT:
                    case WEAPON_FIRE_AREA_EFFECT:
                    {
                        CEntity* pTargetedObject = pPlayerPed->m_pTargetedObject;
                        gunCommand[0] = 2;
                        if (CTaskSimpleUseGun::RequirePistolWhip(pPlayerPed, pTargetedObject))
                        {
                            gunCommand[0] = 5;
                        }
                        else if (pPlayerPed->m_aWeapons[pPlayerPed->m_nActiveWeaponSlot].m_nState == 2)
                        {
                            if (!pPad->GetTarget() && !pTargetedObject && !pPlayerData->m_bFreeAiming)
                            {
                                break;
                            }
                            gunCommand[0] = 1;
                        }
                        CTask* pSecondaryTask = pTaskManager->GetTaskSecondary(0);
                        if (pSecondaryTask)
                        {
                            if (pSecondaryTask->GetId() == TASK_SIMPLE_USE_GUN)
                            {
                                CTaskSimpleUseGun* pTaskUseGun = pIntelligence->GetTaskUseGun();
                                if (pTaskUseGun)
                                {
                                    pTaskUseGun->ControlGun(pPlayerPed, pTargetedObject, gunCommand[0]);
                                }
                            }
                            else
                            {
                                pSecondaryTask->MakeAbortable(pPlayerPed, ABORT_PRIORITY_URGENT, 0);
                            }
                        }
                        else
                        {
                            CTask* pNewTask = static_cast<CTask*>(CTask::operator new(60));
                            CTaskSimpleUseGun* pTaskUseGun = nullptr;
                            if (pNewTask)
                            {
                                pTaskUseGun = static_cast<CTaskSimpleUseGun*>(pNewTask);
                                pTaskUseGun->Constructor(pTargetedObject, CVector(0.0, 0.0, 0.0), gunCommand[0], 1, 0);
                            }

                            pTaskManager->SetTaskSecondary(pTaskUseGun, 0);
                            pPlayerPed->m_pPlayerData->m_fAttackButtonCounter = 0;
                        }
                        if (!pPad->GetTarget())
                        {
                            if (pPlayerPed->m_aWeapons[pPlayerPed->m_nActiveWeaponSlot].m_nType == WEAPON_EXTINGUISHER)
                            {
                                pPlayerData->m_fLookPitch = -CWeapon::ms_fExtinguisherAimAngle;
                            }
                            else
                            {
                                pPlayerData->m_fLookPitch = 0.0;
                            }
                        }
                        break;
                    }
                    case WEAPON_FIRE_PROJECTILE:
                    {
                        unsigned char activeWeaponSlot = pPlayerPed->m_nActiveWeaponSlot;
                        CWeapon* pActiveWeapon = &pPlayerPed->m_aWeapons[activeWeaponSlot];
                        if (pActiveWeapon->m_nType == WEAPON_RLAUNCHER || pActiveWeapon->m_nType == WEAPON_RLAUNCHER_HS)
                        {
                            gunCommand[0] = 2;
                            if (pActiveWeapon->m_nState == 2)
                            {
                                gunCommand[0] = 1;
                            }
                            CTask* pTaskSecondary = pTaskManager->GetTaskSecondary(0);
                            if (pTaskSecondary)
                            {
                                if (pTaskSecondary->GetId() == TASK_SIMPLE_USE_GUN)
                                {
                                    if (pIntelligence->GetTaskUseGun())
                                    {
                                        CTaskSimpleUseGun* pTaskUseGun = static_cast<CTaskSimpleUseGun*>(pTaskSecondary);
                                        pTaskUseGun->ControlGun(pPlayerPed, pPlayerPed->m_pTargetedObject, gunCommand[0]);
                                    }
                                }
                                else
                                {
                                    CTask* pTaskSecondary = pTaskManager->GetTaskSecondary(0);
                                    pTaskSecondary->MakeAbortable(pPlayerPed, ABORT_PRIORITY_URGENT, 0);
                                }
                            }
                            else 
                            {
                                CTask* pNewTask = static_cast<CTask*>(CTask::operator new(60));
                                CTaskSimpleUseGun* pTaskUseGun = nullptr;
                                if (pNewTask)
                                {
                                    pTaskUseGun = static_cast<CTaskSimpleUseGun*>(pNewTask);
                                    pTaskUseGun->Constructor(pPlayerPed->m_pTargetedObject, CVector(0.0, 0.0, 0.0), gunCommand[0], 1, 0);
                                    pTaskManager->SetTaskSecondary(pTaskUseGun, 0);
                                }
                            }
                        }
                        else
                        {
                            if (pTaskManager->GetTaskSecondary(0) || !pPad->WeaponJustDown(pPlayerPed))
                            {
                                CTask* pTaskSecondary = pTaskManager->GetTaskSecondary(0);
                                if (pTaskSecondary && pTaskSecondary->GetId() != TASK_SIMPLE_THROW)
                                {
                                    pTaskSecondary->MakeAbortable(pPlayerPed, ABORT_PRIORITY_URGENT, 0);
                                }
                                else if (pIntelligence->GetTaskThrow())
                                {
                                    auto pTaskSimpleThrowProjectile = static_cast<CTaskSimpleThrowProjectile*>(pTaskSecondary);
                                    pTaskSimpleThrowProjectile->ControlThrow(pPad->WeaponJustDown(pPlayerPed), 0, 0);
                                }
                            }
                            else
                            {
                                CTask* pNewTask = static_cast<CTask*>(CTask::operator new(36));
                                CTaskSimpleThrowProjectile* pTaskSimpleThrowProjectile = nullptr;
                                if (pNewTask)
                                {
                                    pTaskSimpleThrowProjectile = static_cast<CTaskSimpleThrowProjectile*>(pNewTask);
                                    pTaskSimpleThrowProjectile->Constructor(0, CVector(0, 0, 0));
                                    pTaskManager->SetTaskSecondary(pTaskSimpleThrowProjectile, 0);
                                }
                            }
                        }
                        break;
                    }
                    case WEAPON_FIRE_CAMERA:
                    {
                        unsigned char activeWeaponSlot = pPlayerPed->m_nActiveWeaponSlot;
                        CWeapon* pActiveWeapon = &pPlayerPed->m_aWeapons[activeWeaponSlot];
                        if (TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode == MODE_CAMERA && CTimer::m_snTimeInMilliseconds > pActiveWeapon->m_nTimeForNextShot)
                        {
                            CVector firingPoint(0.0, 0.0, 0.60000002);

                            CVector outputFiringPoint(0.0, 0.0, 0.0);
                            MultiplyMatrixWithVector(&outputFiringPoint, pPlayerPed->m_matrix, &firingPoint);
                            pActiveWeapon->Fire(pPlayerPed, &outputFiringPoint, 0, 0, 0, 0);
                        }
                        break;
                    }
                    };
                }
            }
        }
    }

PED_WEAPON_AIMING_CODE:

    CVector firingPoint(0.0, 0.0, 0.0);
    CVector upVector(0.0, 0.0, 0.0);

    if (pPlayerPed->m_aWeapons[pPlayerPed->m_nActiveWeaponSlot].m_nState != WEAPONSTATE_RELOADING || !pWeaponInfo->m_nFlags.bReload)
    //if (pPlayerPed->m_aWeapons[pPlayerPed->m_nActiveWeaponSlot].m_nState != WEAPONSTATE_RELOADING 
    //    || ((pWeaponInfo->m_nFlags4Bytes >> 12 & 1) != 0))
    {
        goto LABEL_187;
    }
    if (pIntelligence->GetTaskUseGun())
    {
        auto pTaskUseGun = (CTaskSimpleUseGun*)pTaskManager->GetTaskSecondary(0);
        pTaskUseGun->ControlGun(pPlayerPed, pPlayerPed->m_pTargetedObject, 4);
        goto LABEL_187;
    }

    int animGroupId = pWeaponInfo->m_dwAnimGroup;
    int crouchReloadAnimID = pWeaponInfo->m_nFlags.bReload != 0 ? 226 : 0;
    if (pPlayerPed->bIsDucking)
    {
        if (!pWeaponInfo->GetCrouchReloadAnimationID())
            goto LABEL_187;
        if (!gbUnknown_8D2FE8)  // this bool is always true
            goto LABEL_187;
        if (!pIntelligence->GetTaskDuck(1))
            goto LABEL_187;
        auto pDuckTask = (CTaskSimpleDuck*)pIntelligence->GetTaskDuck(1);
        if (pDuckTask->IsTaskInUseByOtherTasks())
            goto LABEL_187;
        crouchReloadAnimID = pWeaponInfo->GetCrouchReloadAnimationID();
        if (RpAnimBlendClumpGetAssociation(pPlayerPed->m_pRwClump, crouchReloadAnimID))
            goto LABEL_187;
        crouchReloadAnimID = pWeaponInfo->GetCrouchReloadAnimationID();
    }
    else
    {
        if (RpAnimBlendClumpGetAssociation(pPlayerPed->m_pRwClump, crouchReloadAnimID))
        {
            goto LABEL_187;
        }
    }
    CAnimManager::BlendAnimation(pPlayerPed->m_pRwClump, animGroupId, crouchReloadAnimID, 4.0);
LABEL_187:
    int fightCommand = 0;
    if (!pPad->GetTarget()
        || pPlayerPed->m_pPlayerData->m_nChosenWeapon != pPlayerPed->m_nActiveWeaponSlot
        || pPlayerPed->m_nMoveState == PEDMOVE_SPRINT && pWeaponInfo->m_nWeaponFire
        || TheCamera.Using1stPersonWeaponMode())
    {
        if (!pPad->GetTarget() && !pPlayerPed->m_pAttachedTo
            || pPlayerPed->m_pPlayerData->m_nChosenWeapon != pPlayerPed->m_nActiveWeaponSlot
            || pPlayerPed->m_nMoveState == PEDMOVE_SPRINT
            || !TheCamera.Using1stPersonWeaponMode())
        {
            if ((pPlayerPed->m_pTargetedObject || pPlayerPed->m_pPlayerData->m_bFreeAiming)
                && pIntelligence->GetTaskFighting())
            {
                if (pPlayerData->m_vecFightMovement.y >= -0.5)
                {
                    fightCommand = 15;
                }
                else
                {
                    fightCommand = 16;
                }
                CTaskSimpleFight* pTaskSimpleFight = pIntelligence->GetTaskFighting();
                pTaskSimpleFight->ControlFight(0, fightCommand);
            }
            if (pIntelligence->GetTaskUseGun())
            {
                if (pPad->GetWeapon(0)
                    || (double)pPad->GetPedWalkUpDown() <= 50.0
                    && (double)pPad->GetPedWalkUpDown() >= -50.0
                    && (double)pPad->GetPedWalkLeftRight() <= 50.0
                    && (double)pPad->GetPedWalkLeftRight() >= -50.0)
                {
                    CTaskSimpleUseGun* pTaskUseGun = pIntelligence->GetTaskUseGun();
                    pTaskUseGun->PlayerPassiveControlGun();
                }
                else
                {
                    CTaskSimpleUseGun* pTaskUseGun = pIntelligence->GetTaskUseGun();
                    pTaskUseGun->ControlGun(pPlayerPed, pPlayerPed->m_pTargetedObject, 7);
                }
                CCam* pCam = &TheCamera.m_aCams[TheCamera.m_nActiveCam];
                if (!pWeaponInfo->m_nFlags.bAimWithArm && pCam->m_nMode == MODE_FOLLOWPED)
                {
                    pPlayerPed->m_fAimingRotation = atan2(-pCam->m_vecFront.x, pCam->m_vecFront.y);
                }
                if (pPlayerPed->m_pTargetedObject || pPlayerData->m_bFreeAiming)
                {
                    CTaskSimpleUseGun* pTaskUseGun = pIntelligence->GetTaskUseGun();
                    pTaskUseGun->SkipAim(pPlayerPed);
                }
            }
            if (pPlayerPed->m_pTargetedObject)
            {
                pPlayerPed->ClearWeaponTarget();
            }
            pPlayerPed->Clear3rdPersonMouseTarget();
            pPlayerData->m_bFreeAiming = 0; 
            goto LABEL_316;
        }
        if (!pTaskManager->GetTaskSecondary(0))
        {
            pNewSimpleUseGunTask = (CTaskSimpleUseGun*)CTask::operator new(60);
            goto LABEL_279;
        }
    LABEL_281:
        if (pIntelligence->GetTaskUseGun())
        {
            auto pTaskUseGun = (CTaskSimpleUseGun*)pTaskManager->GetTaskSecondary(0);
            pTaskUseGun->ControlGun(pPlayerPed, pPlayerPed->m_pTargetedObject, 1);
        }
        goto LABEL_316;
    }
    if (!pWeaponInfo->m_nFlags.bCanAim || pPlayerData->m_bFreeAiming)
    {
        if (pPlayerData->m_bFreeAiming && pWeaponInfo->m_nFlags.bCanAim
            && (pPad->ShiftTargetLeftJustDown() || pPad->ShiftTargetRightJustDown())
            && !CCamera::m_bUseMouse3rdPerson)
        {
            if (pPlayerPed->m_pTargetedObject)
            {
                if (pPad->ShiftTargetLeftJustDown())
                {
                    pPlayerPed->FindNextWeaponLockOnTarget(pPlayerPed->m_pTargetedObject, 1);
                }
                if (pPad->ShiftTargetRightJustDown())
                {
                    pPlayerPed->FindNextWeaponLockOnTarget(pPlayerPed->m_pTargetedObject, 0);
                }
            }
            else
            {
                bool shiftTargetLeftJustDown = pPad->ShiftTargetLeftJustDown();
                pPlayerPed->FindNextWeaponLockOnTarget(0, shiftTargetLeftJustDown);
            }
        }
        else if (!pWeaponInfo->m_nFlags.bOnlyFreeAim || pPlayerPed->m_pTargetedObject || pPlayerData->m_bFreeAiming)
        {
            if (CCamera::m_bUseMouse3rdPerson && pPlayerData->m_bFreeAiming)
            {
                bool bWeaponIsNotMelee = 1;
                if (!pWeaponInfo->m_nWeaponFire)
                {
                    bWeaponIsNotMelee = 0;
                }
                pPlayerPed->Compute3rdPersonMouseTarget(bWeaponIsNotMelee);
            }
        }
        else
        {
            pPlayerData->m_bFreeAiming = 1;
        }
    }
    else
    {
        if (pPlayerPed->m_pTargetedObject)
        {
            CPed* pTargetedEntity = (CPed *)pPlayerPed->m_pTargetedObject;
            CWeapon* pActiveWeapon = &pPlayerPed->m_aWeapons[pPlayerPed->m_nActiveWeaponSlot];
            char weaponSkill = 0;
            int pedState = 0;
            if ((fabs((double)pPad->AimWeaponLeftRight(pPlayerPed)) > 100.0
                || fabs((double)pPad->AimWeaponUpDown(pPlayerPed)) > 100.0)
                && !CGameLogic::IsCoopGameGoingOn()
                || pTargetedEntity == (CPed*)0
                || CCamera::m_bUseMouse3rdPerson == 1
                || pTargetedEntity
                && pTargetedEntity->m_nType == ENTITY_TYPE_PED
                && (!CPlayerPed::PedCanBeTargettedVehicleWise(pTargetedEntity)
                    || !CLocalisation::KickingWhenDown()
                    && ((pedState = pTargetedEntity->m_nPedState, pedState == PEDSTATE_DIE) || pedState == PEDSTATE_DEAD))
                || pPlayerPed->DoesTargetHaveToBeBroken(pPlayerPed->m_pTargetedObject, pActiveWeapon)
                || !pPlayerPed->bCanPointGunAtTarget
                && (pActiveWeapon->m_nType, weaponSkill = pPlayerPed->GetWeaponSkill(),
                    !(CWeaponInfo::GetWeaponInfo(pActiveWeapon->m_nType, weaponSkill)->m_nFlags.bCanAim))
                )
            {
                /*
                char weaponSkill = pPlayerPed->GetWeaponSkill();
                if (pActiveWeapon->m_nType && weaponSkill && !(CWeaponInfo::GetWeaponInfo(pActiveWeapon->m_nType, weaponSkill)->m_nFlags.bCanAim))
                {
                */
                    pPlayerPed->ClearWeaponTarget();
                    pPlayerData->m_bFreeAiming = 1;
                //}
            }
            if (pPlayerPed->m_pTargetedObject)
            {
                if (pPad->ShiftTargetLeftJustDown())
                {
                    pPlayerPed->FindNextWeaponLockOnTarget(pPlayerPed->m_pTargetedObject, 1);
                }
                if (pPad->ShiftTargetRightJustDown())
                {
                    pPlayerPed->FindNextWeaponLockOnTarget(pPlayerPed->m_pTargetedObject, 0);
                }
            }
            if (CWeaponInfo::GetWeaponInfo(pActiveWeapon->m_nType, 1)->m_nWeaponFire == WEAPON_FIRE_INSTANT_HIT)
            {
                pTargetedEntity = (CPed*)pPlayerPed->m_pTargetedObject;
                if (pTargetedEntity)
                {
                    if (pTargetedEntity->m_nType == ENTITY_TYPE_PED)
                    {
                        CMatrix* pPlayerMatrix = pPlayerPed->m_matrix;
                        CVector* pPlayerPos = pPlayerMatrix ? &pPlayerMatrix->pos : &pPlayerPed->m_placement.m_vPosn; 
                        if (pIntelligence->IsInSeeingRange(pPlayerPos))
                        {
                            CTask* pActivePrimaryTask = pIntelligence->GetActivePrimaryTask();
                            if (!pActivePrimaryTask || pActivePrimaryTask->GetId() != TASK_COMPLEX_REACT_TO_GUN_AIMED_AT)
                            {
                                if (pActiveWeapon->m_nType != WEAPON_PISTOL_SILENCED)
                                {
                                    pPlayerPed->Say(176, 0, 1.0, 0, 0, 0);
                                }
                                CPedGroup* pPedGroup = CPedGroups::GetPedsGroup(pTargetedEntity);
                                if (pPedGroup)
                                {
                                    if (!CPedGroups::AreInSameGroup(pTargetedEntity, pPlayerPed))
                                    {
                                        CEvent* pEvent = (CEvent*)CEvent::operator_new();
                                        CEventGunAimedAt* pEventGunAimedAt = (CEventGunAimedAt*)pEvent;
                                        if (pEvent)
                                        {
                                            pEventGunAimedAt->Constructor(pPlayerPed);
                                        }
                                        CEventGroupEvent eventGroupEvent;
                                        eventGroupEvent.Constructor(pTargetedEntity, pEvent);
                                        pPedGroup->m_groupIntelligence.AddEvent((CEvent*)&eventGroupEvent);
                                        eventGroupEvent.Destructor();
                                    }
                                }
                                else
                                {
                                    CEventGunAimedAt eventGunAimedAt;
                                    eventGunAimedAt.Constructor(pPlayerPed);
                                    CEvent& theEvent = reinterpret_cast<CEvent&>(eventGunAimedAt);
                                    pIntelligence->m_eventGroup.Add(theEvent, 0);
                                    eventGunAimedAt.Destructor();
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (CCamera::m_bUseMouse3rdPerson)
        {
            pPlayerPed->ClearWeaponTarget();
        }
        else if (pPad->GetEnterTargeting()
            || TheCamera.m_bJustJumpedOutOf1stPersonBecauseOfTarget
            || pThis->m_nFrameCounter < (unsigned int)(CTimer::m_FrameCounter - 1))
        {
            pPlayerPed->FindWeaponLockOnTarget();
        }
        if (!pPlayerPed->m_pTargetedObject)
        {
            pPlayerData->m_bFreeAiming = 1;
        }
    }

    if (pPlayerPed->m_pTargetedObject)
    {
        if (pWeaponInfo->m_nWeaponFire || pPlayerPed->bIsDucking)
        {
            pPlayerData->m_bFreeAiming = 0;
        }
    }
    else
    {
        unsigned int nWeaponFire = pWeaponInfo->m_nWeaponFire;
        if (!nWeaponFire || nWeaponFire == WEAPON_FIRE_PROJECTILE || nWeaponFire == WEAPON_FIRE_USE)
        {
            goto LABEL_316;
        }
    }

    TheCamera.SetNewPlayerWeaponMode(MODE_AIMWEAPON, 0, 0);

    CVector* pTargetedObjectPos = nullptr;
    CEntity* pTargetedEntity = pPlayerPed->m_pTargetedObject;
    if (pTargetedEntity)
    {
        CMatrix* pTargetedObjectMatrix = pTargetedEntity->m_matrix;
        if (pTargetedObjectMatrix)
        {
            pTargetedObjectPos = &pTargetedObjectMatrix->pos;
        }
        else
        {
            pTargetedObjectPos = &pTargetedEntity->m_placement.m_vPosn;
        }
    }
    else
    {
        CMatrix* pPlayerMatrix = pPlayerPed->m_matrix;
        firingPoint.x = pPlayerMatrix->up.x;
        firingPoint.y = pPlayerMatrix->up.y;
        firingPoint.z = pPlayerMatrix->up.z;
        firingPoint.x = firingPoint.x * 5.0;
        firingPoint.y = firingPoint.y * 5.0;
        firingPoint.z = (sin(pPlayerPed->m_pPlayerData->m_fLookPitch) + firingPoint.z) * 5.0;
        CVector* pPlayerPos = &pPlayerPed->m_placement.m_vPosn;
        if (pPlayerMatrix)
        {
            pPlayerPos = &pPlayerMatrix->pos;
        }
        firingPoint += *pPlayerPos;
        pTargetedObjectPos = &firingPoint;
    }

    TheCamera.UpdateAimingCoors(pTargetedObjectPos);
    if (pTaskManager->GetTaskSecondary(0))
    {
        goto LABEL_281;
    }
    pNewSimpleUseGunTask = (CTaskSimpleUseGun*)CTask::operator new(60);
LABEL_279:
    if (pNewSimpleUseGunTask)
    {
        firingPoint.x = 0.0;
        firingPoint.y = 0.0;
        firingPoint.z = 0.0;
        pNewSimpleUseGunTask->Constructor(pPlayerPed->m_pTargetedObject, CVector(0.0, 0.0, 0.0), 1, 1, 0);
    }

    pTaskManager->SetTaskSecondary(pNewSimpleUseGunTask, 0);
LABEL_316:

    if (pPlayerPed->m_pTargetedObject)
    {
        pPlayerData->m_bHaveTargetSelected = 1;
    }

    CPed* pTargetedObject = (CPed *)pPlayerPed->m_pTargetedObject;
    bool bTargetedPedDead = 0;
    if (!pTargetedObject)
    {
        goto LABEL_346;
    }
    if (pTargetedObject->m_nType == ENTITY_TYPE_PED && (pTargetedObject->bFallenDown || pTargetedObject->m_nPedState == PEDSTATE_DEAD))
    {
        bTargetedPedDead = 1;
    }

    if ((pWeaponInfo->m_nWeaponFire || pIntelligence->GetTaskFighting() && !bTargetedPedDead)
       && (pPlayerPed->bIsDucking || !pWeaponInfo->m_nFlags.bAimWithArm))
    {
        goto LABEL_346;
    }
    CMatrix* pPlayerMatrix = pPlayerPed->m_matrix;
    CVector* pPlayerPos = &pPlayerMatrix->pos;
    if (!pPlayerMatrix)
    {
        pPlayerPos = &pPlayerPed->m_placement.m_vPosn;
    }
    CMatrix* pTargetedObjectMatrix = pTargetedObject->m_matrix;
    pTargetedObjectPos = pTargetedObjectMatrix ? &pTargetedObjectMatrix->pos : &pTargetedObject->m_placement.m_vPosn;
    firingPoint.x = pTargetedObjectPos->x - pPlayerPos->x;
    firingPoint.y = pTargetedObjectPos->y - pPlayerPos->y;
    firingPoint.z = pTargetedObjectPos->z - pPlayerPos->z;
    CVector* pUpVector = pPlayerPed->GetTopDirection(&upVector);
    if ((firingPoint.z * pUpVector->z + firingPoint.y * pUpVector->y + firingPoint.x * pUpVector->x) <= 0.0)
    {
    LABEL_346:
        if (pThis->m_pLookingAtEntity
            && g_ikChainMan->IsLooking(pPlayerPed)
            && g_ikChainMan->GetLookAtEntity(pPlayerPed) == pThis->m_pLookingAtEntity)
        {
            g_ikChainMan->AbortLookAt(pPlayerPed, 250);
        }
    }
    else if (!g_ikChainMan->IsLooking(pPlayerPed)
        || (g_ikChainMan->GetLookAtEntity(pPlayerPed) != (CEntity*)pTargetedObject))
    {
        int pedBoneID = BONE_UNKNOWN;
        if (pTargetedObject->m_nType == ENTITY_TYPE_PED)
        {
            pedBoneID = BONE_HEAD;
        }
        g_ikChainMan->LookAt("ProcPlyrWeapon", pPlayerPed, pTargetedObject, gDefaultTaskTime, pedBoneID,
            0, 0, 0.25, 500, 3, 0);
    }

    pThis->m_pLookingAtEntity = pTargetedObject; 
}
