#include "StdInc.h"
#include "detours.h"

#pragma comment(lib, "detours.lib")

//bool __thiscall CTaskSimpleSwim_ProcessSwimmingResistance(CPhysical* pThis)
auto OLD_CTaskSimpleSwim_ProcessSwimmingResistance = (void(__thiscall*) (CTaskSimpleSwim * pThis, CPed * pPed))0x68A1D0;
void __fastcall CTaskSimpleSwim_ProcessSwimmingResistance(CTaskSimpleSwim* pThis, void* padding, CPed* pPed);

void __cdecl HOOK_THEFUNCTION();

void InjectHooksMain(void)
{
    /*CAnimManager::InjectHooks();
    pTaskManager->InjectHooks();
    std::printf("okay, only CAnimManager and CTaskManager hooks\n ");
    CStreaming::InjectHooks();
    CRenderer::InjectHooks();*/

    //HookInstall(0x6899F0, &CTaskSimpleSwim::ProcessSwimAnims, 7);

  ///*
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    std::printf("GOING TO HOOK FUNC NOW\n");
    DetourAttach(&(PVOID&)OLD_CTaskSimpleSwim_ProcessSwimmingResistance, CTaskSimpleSwim_ProcessSwimmingResistance);
    DetourTransactionCommit();
  // */
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

void __fastcall CTaskSimpleSwim_ProcessSwimmingResistance(CTaskSimpleSwim* pThis, void* padding, CPed* pPed)
{
    printf(" calling CTaskSimpleSwim_ProcessSwimmingResistance \n");

    float fSubmergeZ = -1.0f;
    CVector vecPedMoveSpeed;

    switch (pThis->m_nSwimState)
    {
    case SWIM_TREAD:
    case SWIM_SPRINT:
    case SWIM_SPRINTING:
    {
        CAnimBlendAssociation* pAnimSwimBreast = RpAnimBlendClumpGetAssociation(pPed->m_pRwClump, SWIM_SWIM_BREAST);
        CAnimBlendAssociation* pAnimSwimCrawl = RpAnimBlendClumpGetAssociation(pPed->m_pRwClump, SWIM_SWIM_CRAWL);

        float fAnimBlendSum = 0.0f;
        float fAnimBlendDifference = 1.0f;
        if (pAnimSwimBreast)
        {
            fAnimBlendSum = 0.40000001f * pAnimSwimBreast->m_fBlendAmount;
            fAnimBlendDifference = 1.0f - pAnimSwimBreast->m_fBlendAmount;
        }

        if (pAnimSwimCrawl)
        {
            fAnimBlendSum += 0.2f * pAnimSwimCrawl->m_fBlendAmount;
            fAnimBlendDifference -= pAnimSwimCrawl->m_fBlendAmount;
        }

        if (fAnimBlendDifference < 0.0f)
        {
            fAnimBlendDifference = 0.0f;
        }

        fSubmergeZ = fAnimBlendDifference * 0.55000001f + fAnimBlendSum;

        vecPedMoveSpeed = pPed->m_vecAnimMovingShiftLocal.x * pPed->m_matrix->right;
        vecPedMoveSpeed += pPed->m_vecAnimMovingShiftLocal.y * pPed->m_matrix->up;

        goto def_68A208;
    }
    case SWIM_DIVE_UNDERWATER:
    {
        vecPedMoveSpeed = pPed->m_vecAnimMovingShiftLocal.x * pPed->m_matrix->right;
        vecPedMoveSpeed += pPed->m_vecAnimMovingShiftLocal.y * pPed->m_matrix->up;

        auto pAnimSwimDiveUnder = RpAnimBlendClumpGetAssociation(pPed->m_pRwClump, SWIM_SWIM_DIVE_UNDER);
        if (pAnimSwimDiveUnder)
        {
            vecPedMoveSpeed.z = pAnimSwimDiveUnder->m_fCurrentTime / pAnimSwimDiveUnder->m_pHierarchy->m_fTotalTime * -0.1f;
        }
        goto def_68A208;
    }
    case SWIM_UNDERWATER_SPRINTING:
    {
        vecPedMoveSpeed = pPed->m_vecAnimMovingShiftLocal.x * pPed->m_matrix->right;
        vecPedMoveSpeed += cos(pThis->m_fRotationX) * pPed->m_vecAnimMovingShiftLocal.y * pPed->m_matrix->up;
        vecPedMoveSpeed.z += sin(pThis->m_fRotationX) * pPed->m_vecAnimMovingShiftLocal.y  + 0.0099999998f;


        def_68A208:
        float fTheTimeStep = pow(0.89999998f, CTimer::ms_fTimeStep);
        vecPedMoveSpeed *= (1.0f - fTheTimeStep);
        pPed->m_vecMoveSpeed *= fTheTimeStep;
        pPed->m_vecMoveSpeed += vecPedMoveSpeed;

        const CVector& vecPedPosition = pPed->GetPosition();
        CVector vecCheckWaterLevelPos = CTimer::ms_fTimeStep * pPed->m_vecMoveSpeed + pPed->GetPosition();

        float fWaterLevel = 0.0;         
        if (!CWaterLevel::GetWaterLevel(vecCheckWaterLevelPos.x, vecCheckWaterLevelPos.y, vecPedPosition.z, &fWaterLevel, true, 0))
        {
            goto LABEL_67;
        }
        if (pThis->m_nSwimState != SWIM_UNDERWATER_SPRINTING || pThis->m_fStateChanger < 0.0)
        {
            goto LABEL_34;
        }

        if (vecPedPosition.z + 0.64999998f > fWaterLevel && pThis->m_fRotationX > 0.78539819f)
        {
            pThis->m_nSwimState = SWIM_TREAD;
        LABEL_33:
            pThis->m_fStateChanger = 0.0;
            goto LABEL_34;
        }
        if (pThis->m_fRotationX >= 0.0)
        {
            if (vecPedPosition.z + 0.64999998f <= fWaterLevel)
            {
                if (pThis->m_fStateChanger <= 0.001f)
                    goto LABEL_33;
                pThis->m_fStateChanger *= 0.94999999f;
            }
            else
            {
                float fMinimumSpeed = 0.050000001f * 0.5f;
                if (pThis->m_fStateChanger > fMinimumSpeed)
                {
                    pThis->m_fStateChanger *= 0.94999999f;
                }
                if (pThis->m_fStateChanger < fMinimumSpeed)
                {
                    pThis->m_fStateChanger += CTimer::ms_fTimeStep * 0.0020000001f;
                    pThis->m_fStateChanger = std::min(fMinimumSpeed, pThis->m_fStateChanger);
                }
                pThis->m_fRotationX += CTimer::ms_fTimeStep * pThis->m_fStateChanger;
                fSubmergeZ = (0.55000001f - 0.2f) * (pThis->m_fRotationX * 1.2732395f) * 0.75f + 0.2f;
            }
        }
        else
        {
            if (vecPedPosition.z - sin(pThis->m_fRotationX) + 0.64999998f <= fWaterLevel)
            {
                if (pThis->m_fStateChanger > 0.001f)
                {
                    pThis->m_fStateChanger *= 0.94999999f;
                    pThis->m_fRotationX += CTimer::ms_fTimeStep * pThis->m_fStateChanger;
                    goto LABEL_34;
                }
                pThis->m_fStateChanger = 0.0;
            }
            else
            {
                pThis->m_fStateChanger += CTimer::ms_fTimeStep * 0.0020000001f;
                if (pThis->m_fStateChanger > 0.050000001f)
                {
                    pThis->m_fStateChanger = 0.050000001f;
                    pThis->m_fRotationX += CTimer::ms_fTimeStep * pThis->m_fStateChanger;
                    goto LABEL_34;
                }
            }
            pThis->m_fRotationX += CTimer::ms_fTimeStep * pThis->m_fStateChanger;
        }
    LABEL_34:
        if (fSubmergeZ > 0.0)
        {

            fWaterLevel -= (fSubmergeZ + vecPedPosition.z);
            float fTimeStepMoveSpeedZ = fWaterLevel / CTimer::ms_fTimeStep;
            float fTimeStep = CTimer::ms_fTimeStep * 0.1f;
            if (fTimeStepMoveSpeedZ > fTimeStep)
            {
                fTimeStepMoveSpeedZ = fTimeStep;
            }

            if (-fTimeStep > fTimeStepMoveSpeedZ)
            {
                fTimeStepMoveSpeedZ = -fTimeStep;
            }

            fTimeStepMoveSpeedZ -= pPed->m_vecMoveSpeed.z;

            fTimeStep = CTimer::ms_fTimeStep * 0.02f;
            if (fTimeStepMoveSpeedZ > fTimeStep)
            {
                fTimeStepMoveSpeedZ = fTimeStep;
            }

            if (-fTimeStep > fTimeStepMoveSpeedZ)
            {
                fTimeStepMoveSpeedZ = -fTimeStep;
            }
            pPed->m_vecMoveSpeed.z += fTimeStepMoveSpeedZ;
        }
    LABEL_67:
        CVector* pPedPosition = &pPed->GetPosition();
        if (pPedPosition->z < -69.0f)
        {
            pPedPosition->z = -69.0f;
            if (pPed->m_vecMoveSpeed.z < 0.0f)
            {
                pPed->m_vecMoveSpeed.z = 0.0f;
            }
        }
        return;
    }
    case SWIM_BACK_TO_SURFACE:
    {
        auto pAnimAssociation = RpAnimBlendClumpGetAssociation(pPed->m_pRwClump, 128);
        if (!pAnimAssociation)
        {
            pAnimAssociation = RpAnimBlendClumpGetAssociation(pPed->m_pRwClump, SWIM_SWIM_JUMPOUT);
        }

        if (pAnimAssociation)
        {
            if (pAnimAssociation->m_pHierarchy->m_fTotalTime > pAnimAssociation->m_fCurrentTime
                && (pAnimAssociation->m_fBlendAmount >= 1.0f || pAnimAssociation->m_fBlendDelta > 0.0f))
            {
                float fMoveForceZ = CTimer::ms_fTimeStep * pPed->m_fMass * 0.30000001f * 0.0080000004f;
                pPed->ApplyMoveForce(0.0f, 0.0f, fMoveForceZ);
            }
        }
        return;
    }
    }
}
