#include "StdInc.h"
#include "detours.h"

#pragma comment(lib, "detours.lib")

//bool __thiscall CTaskSimpleSwim_ProcessControlInput(CPhysical* pThis)
auto OLD_CTaskSimpleSwim_ProcessControlInput = (void(__thiscall*) (CTaskSimpleSwim * pThis, CPlayerPed * pPed))0x688A90;
void __fastcall CTaskSimpleSwim_ProcessControlInput(CTaskSimpleSwim* pThis, void* padding, CPlayerPed* pPed);

void __cdecl HOOK_THEFUNCTION();

void InjectHooksMain(void)
{
    /*CAnimManager::InjectHooks();
    pTaskManager->InjectHooks();
    std::printf("okay, only CAnimManager and CTaskManager hooks\n ");
    CStreaming::InjectHooks();
    CRenderer::InjectHooks();*/

   // HookInstall(0x68A1D0, &CTaskSimpleSwim::ProcessSwimmingResistance, 7);

  ///*
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    std::printf("GOING TO HOOK FUNC NOW\n");
    DetourAttach(&(PVOID&)OLD_CTaskSimpleSwim_ProcessControlInput, CTaskSimpleSwim_ProcessControlInput);
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

void __fastcall CTaskSimpleSwim_ProcessControlInput(CTaskSimpleSwim* pThis, void* padding, CPlayerPed* pPed)
{
    printf(" calling CTaskSimpleSwim_ProcessControlInput \n");

    /*if (pThis->m_nSwimState == SWIM_UNDERWATER_SPRINTING)
    {
        return OLD_CTaskSimpleSwim_ProcessControlInput(pThis, pPed);
    }*/

    CVector vecPedWalk; // [esp+1Ch] [ebp-18h]


    if (!pThis->m_bFinishedBlending || !pThis->m_bAnimBlockRefAdded)
    {
        pPed->m_pPlayerData->m_fMoveBlendRatio = 0.0;
        return;
    }

    CPad* pPad = pPed->GetPadFromPlayer();
    short pedWalkUpDown = pPad->GetPedWalkUpDown();
    short pedWalkLeftRight = pPad->GetPedWalkLeftRight();
    vecPedWalk.x = pedWalkLeftRight * 0.0078125f;
    vecPedWalk.y = pedWalkUpDown * 0.0078125f;

    float fWalkMagnitude = vecPedWalk.Magnitude2D();
    if (pThis->m_nSwimState < SWIM_SPRINTING)
    {
        if (pPad->JumpJustDown())
        {
            pThis->m_nSwimState = SWIM_BACK_TO_SURFACE;
        }
        else if (pPad->WeaponJustDown(pPed))
        {
            pThis->m_nSwimState = SWIM_DIVE_UNDERWATER;
            pPed->m_pPlayerData->m_fMoveBlendRatio = 0.0;
        }
    }

    CPlayerData* pPlayerData = pPed->m_pPlayerData;
    switch (pThis->m_nSwimState)
    {
    case SWIM_TREAD:
    case SWIM_SPRINT:
    case SWIM_SPRINTING:
    {
        float pedWalkX = 0.0f;
        bool bPlayerUse2PlayerControls = 0;
        if (CGameLogic::IsPlayerUse2PlayerControls(pPed))
        {
            bPlayerUse2PlayerControls = 1;
            pedWalkX = vecPedWalk.x;
            if (fWalkMagnitude > 0)
            {
                float negativePedWalkX = -pedWalkX;
                float fRadianAngle = CGeneral::GetRadianAngleBetweenPoints(0.0, 0.0, negativePedWalkX, vecPedWalk.y)
                    - TheCamera.m_fOrientation;
                float fLimitedRadianAngle = CGeneral::LimitRadianAngle(fRadianAngle);

                CVector vecPedWalkDirection (0.0f, -sin(fLimitedRadianAngle), cos(fLimitedRadianAngle));
                if (fLimitedRadianAngle <= pPed->m_fCurrentRotation + 3.1415927f)
                {
                    if (fLimitedRadianAngle < pPed->m_fCurrentRotation - 3.1415927f)
                    {
                        fLimitedRadianAngle = fLimitedRadianAngle + 6.2831855f;
                    }
                }
                else
                {
                    fLimitedRadianAngle = fLimitedRadianAngle - 6.2831855f;
                }
                
                float fCurrenRotation = fLimitedRadianAngle - pPed->m_fCurrentRotation;
                if (fCurrenRotation <= 1.0f)
                {
                    if (fCurrenRotation < -1.0f)
                        fCurrenRotation = -1.0f;
                }
                else
                {
                    fCurrenRotation = 1.0f;
                }

                pPed->m_fAimingRotation = fCurrenRotation * (CTimer::ms_fTimeStep * 0.079999998f) + pPed->m_fCurrentRotation;
                if (pPed->m_fAimingRotation <= 3.14159274f)
                {
                    if (pPed->m_fAimingRotation < -3.1415927f)
                    {
                        pPed->m_fAimingRotation = pPed->m_fAimingRotation + 6.2831855f;
                    }
                }
                else
                {
                    pPed->m_fAimingRotation = pPed->m_fAimingRotation - 6.2831855f;
                }
                if (CGameLogic::IsPlayerAllowedToGoInThisDirection(
                    pPed,
                    vecPedWalkDirection.x,
                    vecPedWalkDirection.y,
                    vecPedWalkDirection.z,
                    0.0))
                {
                    CMatrixLink* pPedMatrix = pPed->m_matrix;
                    pedWalkX = (vecPedWalkDirection.y * pPedMatrix->right.y 
                              + vecPedWalkDirection.x * pPedMatrix->right.x
                              + pPedMatrix->right.z * 0.0f) * 
                        fWalkMagnitude;
                    vecPedWalk.y = -((vecPedWalkDirection.y * pPedMatrix->up.y
                                    + pPedMatrix->up.z * 0.0f
                                    + vecPedWalkDirection.x * pPedMatrix->up.x)
                                    * fWalkMagnitude);
                }
                else
                {
                    pedWalkX = 0.0;
                    fWalkMagnitude = 0.0;
                    vecPedWalk.y = 0.0;
                }
            }
        }
        else                                      // if CGameLogic::IsPlayerUse2PlayerControls(pPed) == false
        {
            pedWalkX = vecPedWalk.x;
        }
        if (!CCamera::m_bUseMouse3rdPerson)
        {
            float fMoveBlendRatio = 0.0f;
            if (fWalkMagnitude <= 0.0)
            {
                if (pPlayerData->m_fMoveBlendRatio <= 0.0f)
                {
                    goto LABEL_105;
                }

                fMoveBlendRatio = pPlayerData->m_fMoveBlendRatio - CTimer::ms_fTimeStep * 0.050000001f;
                if (fMoveBlendRatio < 0.0)
                {
                    fMoveBlendRatio = 0.0;
                }
                goto LABEL_104;
            }
            if (bPlayerUse2PlayerControls)
            {
            LABEL_98:
                pPlayerData = pPed->m_pPlayerData;
                float fTimeStep = CTimer::ms_fTimeStep * 0.07f;
                if (fWalkMagnitude - pPlayerData->m_fMoveBlendRatio <= fTimeStep)
                {
                    pPlayerData->m_fMoveBlendRatio = fWalkMagnitude;
                    goto LABEL_105;
                }
                fMoveBlendRatio = fTimeStep + pPlayerData->m_fMoveBlendRatio;
            LABEL_104:
                pPlayerData->m_fMoveBlendRatio = fMoveBlendRatio;
                goto LABEL_105;
            }
            pPed->m_fAimingRotation += CTimer::ms_fTimeStep * -0.029999999f * pedWalkX;
            if (pPed->m_fAimingRotation <= 3.14159274f)
            {
                if (pPed->m_fAimingRotation < -3.1415927f)
                {
                    pPed->m_fAimingRotation += 6.2831855f;
                }
            }
            else
            {
                pPed->m_fAimingRotation -= 6.2831855f;
            }
            float pedWalkY = vecPedWalk.y;
            float negativePedWalkY = 0.0f;
            if (pThis->m_nSwimState)
            {
                if (pedWalkY <= 0.0)
                {
                    goto LABEL_96;
                }
                fWalkMagnitude -= vecPedWalk.y;
            }
            else
            {
                fWalkMagnitude = -pedWalkY;
            }

        LABEL_96:
            if (fWalkMagnitude < 0.0)
            {
                fWalkMagnitude = 0.0;
            }
            goto LABEL_98;
        }
        pPed->m_fAimingRotation = atan2(
            -TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecFront.x,
            TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecFront.y);
        if (TheCamera.GetLookDirection() != 3)
        {
            pPed->m_fAimingRotation += 3.1415927f;
            if (pPed->m_fAimingRotation > 3.14159274f)
            {
                pPed->m_fAimingRotation -= 6.2831855f;
            }
        }

        float negativePedWalkY = -vecPedWalk.y;
        float fTimeStep = CTimer::ms_fTimeStep * 0.07f;
        if (negativePedWalkY - pPlayerData->m_fMoveBlendRatio > fTimeStep)
        {
            pPlayerData->m_fMoveBlendRatio += fTimeStep;
        }
        else
        {
            pPlayerData->m_fMoveBlendRatio = negativePedWalkY;
        }

        float fRotation = -(pPed->m_fAimingRotation - pPed->m_fCurrentRotation);
        if (fRotation <= 3.1415927)
        {
            if (fRotation < -3.1415927f)
            {
                fRotation = 6.2831855f - (pPed->m_fAimingRotation - pPed->m_fCurrentRotation);
            }

        }
        else
        {
            fRotation = fRotation - 6.2831855f;
        }

        fRotation *= 10.0f;
        fRotation = std::max(-1.0f, fRotation);
        fRotation = std::min(1.0f, fRotation);   
    LABEL_105:
        pThis->m_fAimingRotation = CTimer::ms_fTimeStep * 0.079999998f * fRotation+ pThis->m_fAimingRotation;

        if (pThis->m_nSwimState == SWIM_SPRINTING)
        {
            pThis->m_fTurningRotationY = CTimer::ms_fTimeStep * 0.039999999f * fRotation + pThis->m_fTurningRotationY;
        }
        else if (pThis->m_nSwimState == SWIM_SPRINT)
        {
            pThis->m_fUpperTorsoRotationX = fabs(pedWalkX) * CTimer::ms_fTimeStep * 0.039999999f
                + pThis->m_fUpperTorsoRotationX;
        }

        if (pThis->m_nSwimState == SWIM_SPRINTING)
        {
            CVector vecPositionSum;
            CVector vecPositionDifference;
            CVector* pPedPosition = &pPed->GetPosition();
           
            float fSumWaterLevel = 0.0;         
            float fDifferenceWaterLevel = 0.0f;
            VectorAdd(&vecPositionSum, pPedPosition, &pPed->m_matrix->up);
            VectorSub(&vecPositionDifference, pPedPosition, &pPed->m_matrix->up);
            if (CWaterLevel::GetWaterLevel(vecPositionSum.x, vecPositionSum.y, vecPositionSum.z, &fSumWaterLevel, 1, 0)
             && CWaterLevel::GetWaterLevel(vecPositionDifference.x, vecPositionDifference.y, vecPositionDifference.z, &fDifferenceWaterLevel, 1, 0))
            {
                pThis->m_fRotationX = atan2(fSumWaterLevel - fDifferenceWaterLevel, 2.0f) * 1.0f;
            }
        }

        if (pPed->ControlButtonSprint(static_cast<eSprintType>(2)) < 1.0f)
        {
            pThis->m_nSwimState = static_cast<eSwimState>(pPed->m_pPlayerData->m_fMoveBlendRatio > 0.5f);
        }
        else
        {
            pThis->m_nSwimState = SWIM_SPRINTING;
        }
    def_688B54:

        fRotation = 0.94999999f;
        if (pThis->m_nSwimState == SWIM_UNDERWATER_SPRINTING || pThis->m_nSwimState == SWIM_SPRINTING)
        {
            fRotation = 0.94999999f;
        }

        if (pThis->m_fTurningRotationY > 0.0099999998f || pThis->m_fTurningRotationY < -0.0099999998f)
        {
            pThis->m_fTurningRotationY *= pow(fRotation, CTimer::ms_fTimeStep);
        }
        else
        {
            pThis->m_fTurningRotationY = 0.0f;
        }
        if (pThis->m_nSwimState != SWIM_UNDERWATER_SPRINTING && pThis->m_nSwimState != SWIM_SPRINTING)
        {
            if (pThis->m_fRotationX > 0.0099999998f || pThis->m_fRotationX < -0.0099999998f)
            {
                pThis->m_fRotationX *= pow(fRotation, CTimer::ms_fTimeStep);
            }
            else
            {
                pThis->m_fRotationX = 0.0f;
            }
        }

        if (pThis->m_nSwimState == SWIM_DIVE_UNDERWATER || pThis->m_nSwimState == SWIM_BACK_TO_SURFACE)
        {
            fRotation = 0.94999999f;
        }
        else
        {
            fRotation = 0.92000002f;
        }

        if (pThis->m_fAimingRotation > 0.0099999998f
            || pThis->m_fAimingRotation < -0.0099999998f
            || pThis->m_fUpperTorsoRotationX > 0.0099999998f
            || pThis->m_fUpperTorsoRotationX < -0.0099999998f)
        {
            float fTimeStepRotation = pow(fRotation, CTimer::ms_fTimeStep);
            pThis->m_fAimingRotation *= fTimeStepRotation;
            pThis->m_fUpperTorsoRotationX *= fTimeStepRotation;
        }
        else
        {
            pThis->m_fAimingRotation = 0.0f;
            pThis->m_fUpperTorsoRotationX = 0.0f;
        }
        if (pThis->m_nSwimState == SWIM_SPRINT)
        {
            pPed->HandleSprintEnergy(0, 0.5f);
        }
        else if (pThis->m_nSwimState != SWIM_SPRINTING)
        {
            pPed->HandleSprintEnergy(0, 1.0f);
        }
        return;
    }
    case SWIM_DIVE_UNDERWATER:
    {
        if (pThis->m_fStateChanger > 0.0f)
        {
            pThis->m_fStateChanger = 0.0f;
        }
        goto def_688B54;
    }
    case SWIM_UNDERWATER_SPRINTING:
    {
        float fUpperTorsoRotationX = 0.0f;
        if (CCamera::m_bUseMouse3rdPerson)
        {
            CVector * pVecActiveCamFront = &TheCamera.m_aCams[TheCamera.m_nActiveCam].m_vecFront;
            float fActiveCamFrontX = pVecActiveCamFront->x;
            float fActiveCamFrontY = pVecActiveCamFront->y;
            float fActiveCamFrontZ = pVecActiveCamFront->z;
            CVector vecPedWalkDirection (fActiveCamFrontX, fActiveCamFrontY, fActiveCamFrontZ);
            float fActiveCamFrontX0 = fActiveCamFrontX;
            float fActiveCamFrontY0 = 0;
            if (TheCamera.GetLookDirection() == 3)
            {
                fActiveCamFrontY0 = vecPedWalkDirection.y;
            }
            else
            {
                fActiveCamFrontX0 = fActiveCamFrontX0 * -1.0;
                vecPedWalkDirection.z = 0.0;
                fActiveCamFrontY0 = vecPedWalkDirection.y * -1.0;
            }

            float fAimingRotation0 = atan2(-fActiveCamFrontX0, fActiveCamFrontY0);
            fAimingRotation0 = atan2(-fActiveCamFrontX0, fActiveCamFrontY0);
            pPed->m_fAimingRotation = fAimingRotation0;
            float fRotation0 = -(fAimingRotation0 - pPed->m_fCurrentRotation);
            if (fRotation0 <= 3.1415927)
            {
                if (fRotation0 < -3.1415927)
                    fRotation0 = fRotation0 + 6.2831855;
            }
            else
            {
                fRotation0 = fRotation0 - 6.2831855;
            }
            float fRotationMultiplied0 = fRotation0 * 10.0;
            float fMaximumRotation0 = std::max(-1.0f, fRotationMultiplied0);
            vecPedWalk.x = std::min(1.0f, fMaximumRotation0);// should be fMinimumRotation
            pThis->m_fTurningRotationY = CTimer::ms_fTimeStep * 0.039999999 * vecPedWalk.x + pThis->m_fTurningRotationY;
            pThis->m_fAimingRotation = CTimer::ms_fTimeStep * 0.079999998 * vecPedWalk.x + pThis->m_fAimingRotation;
            float fRotationX0 = (asin(vecPedWalkDirection.z) - pThis->m_fRotationX) * 10.0;
            float fMaxRotationX = std::max(-1.0f, fRotationX0);
            float fRotationX = std::min(1.0f, fMaxRotationX);
            if (pThis->m_fStateChanger == 0.0 || fRotationX > 0.0)
                pThis->m_fRotationX = CTimer::ms_fTimeStep * 0.02 * fRotationX + pThis->m_fRotationX;
            float fTurningRotationYDivided = pThis->m_fTurningRotationY / 0.5;
            if (fTurningRotationYDivided > 1.0 || fTurningRotationYDivided >= -1.0)
            {
                if (fTurningRotationYDivided > 1.0)
                    fTurningRotationYDivided = 1.0;
            }
            else
            {
                fTurningRotationYDivided = -1.0;
            }
            fUpperTorsoRotationX = fRotationX + fTurningRotationYDivided * -0.079999998 * vecPedWalk.x;
            if (fUpperTorsoRotationX <= 1.0)
            {
                if (fUpperTorsoRotationX < -1.0)
                    fUpperTorsoRotationX = -1.0;
                goto LABEL_47;
            }
        }
        else
        {
            if (fWalkMagnitude <= 0.0f)
            {
                goto LABEL_48;
            }

            float fNormalizedWalkMagnitude = 1.0f / fWalkMagnitude;
            vecPedWalk.x = vecPedWalk.x * fNormalizedWalkMagnitude;
            float pedWalkY = fNormalizedWalkMagnitude * vecPedWalk.y;
            pPed->m_fAimingRotation += CTimer::ms_fTimeStep * -0.029999999f * vecPedWalk.x;
            if (pPed->m_fAimingRotation <= 3.14159274f)
            {
                if (pPed->m_fAimingRotation < -3.1415927f)
                {
                    pPed->m_fAimingRotation += 6.2831855f;
                }
            }
            else
            {
                pPed->m_fAimingRotation -= 6.2831855f;
            }
            pThis->m_fTurningRotationY += CTimer::ms_fTimeStep * 0.039999999f * vecPedWalk.x;
            pThis->m_fAimingRotation += CTimer::ms_fTimeStep * 0.079999998f * vecPedWalk.x;
            if (pThis->m_fStateChanger == 0.0f || pedWalkY > 0.0f)
            {
                pThis->m_fRotationX += CTimer::ms_fTimeStep * 0.02f * pedWalkY;
            }

            fUpperTorsoRotationX = pThis->m_fTurningRotationY / 0.5f;
            if (fUpperTorsoRotationX > 1.0f || fUpperTorsoRotationX >= -1.0f)
            {
                if (fUpperTorsoRotationX > 1.0f)
                {
                    fUpperTorsoRotationX = 1.0f;
                }
            }
            else
            {
                fUpperTorsoRotationX  = -1.0f;
            }
            fUpperTorsoRotationX = pedWalkY + fUpperTorsoRotationX  * -0.079999998f * vecPedWalk.x;
            if (fUpperTorsoRotationX <= 1.0f)
            {
                if (fUpperTorsoRotationX < -1.0f)
                {
                    fUpperTorsoRotationX = -1.0f;
                }
                goto LABEL_47;
            }
        }
        fUpperTorsoRotationX = 1.0;
    LABEL_47:

        pThis->m_fUpperTorsoRotationX += CTimer::ms_fTimeStep * -0.079999998f * fUpperTorsoRotationX;
    LABEL_48:

        float fRotationX1 = CTimer::ms_fTimeStep * 0.001 + pThis->m_fRotationX;
        pThis->m_fRotationX = fRotationX1;
        if (fRotationX1 > 1.3962635 || fRotationX1 >= -1.3962635)
        {
            if (fRotationX1 > 1.3962635)
                fRotationX1 = 1.3962635;
        }
        else
        {
            fRotationX1 = -1.3962635;
        }
        pThis->m_fRotationX = fRotationX1;
        /*
        pThis->m_fRotationX += CTimer::ms_fTimeStep * 0.001f;

        if (pThis->m_fRotationX > 1.3962635f || pThis->m_fRotationX >= -1.3962635f)
        {
            if (pThis->m_fRotationX > 1.3962635f)
            {
                pThis->m_fRotationX = 1.3962635f;
            }
        }
        else
        {
            pThis->m_fRotationX = -1.3962635f;
        }
        */
        if (pPed->m_pPlayerData->m_fTimeCanRun <= 0.0f)
        {
            pPed->m_pPlayerData->m_fTimeCanRun = 0.1f;
        }
        pPed->ControlButtonSprint(static_cast<eSprintType>(3));
        goto def_688B54;
    }
    }
  
}
