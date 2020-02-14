#include "StdInc.h"
#include "detours.h"

#pragma comment(lib, "detours.lib")

//void __thiscall CPhysical_ApplySpeed(CObject *this)

auto OLD_CPhysical_ApplySpeed = (void(__thiscall*) (CPhysical* pThis))0x547B80;
void __fastcall CPhysical_ApplySpeed(CPhysical* pThis, void* padding);

void __cdecl HOOK_THEFUNCTION();

void InjectHooksMain(void)
{
    // CStreaming is unstable for now.
    //CStreaming::InjectHooks();

    /*
    CPhysical::InjectHooks();
    CRenderer::InjectHooks();
    CPedIntelligence::InjectHooks();
    CTrain::InjectHooks();
    CTaskManager::InjectHooks();
    CTaskComplexUseSequence::InjectHooks();
    CTaskComplexSequence::InjectHooks();
    CTaskComplexWander::InjectHooks();
    CTaskComplexWanderCop::InjectHooks();
    CTaskSimplePlayerOnFoot::InjectHooks();
    CTaskSimpleSwim::InjectHooks();
    */
    ///*
     DetourRestoreAfterWith();
     DetourTransactionBegin();
     DetourUpdateThread(GetCurrentThread());

     std::printf("GOING TO HOOK FUNC NOW\n");
     DetourAttach(&(PVOID&)OLD_CPhysical_ApplySpeed, CPhysical_ApplySpeed);
     DetourTransactionCommit();
     //*/
}

static void UpdateTimeStep(float fTimeStep)
{
    if (fTimeStep > 0.00001f)
    {
        CTimer::ms_fTimeStep = fTimeStep;
    }
    else
    {
        CTimer::ms_fTimeStep = 0.00001f;
    }
}


void __fastcall CPhysical_ApplySpeed(CPhysical* pThis, void* padding)
{
    printf("CPhysical_ApplySpeed called!\n");

    float fNewTimeStep1  = 0.0f;

    float fSnookerTableX = 0.0f;
    float fSnookerTableY = 0.0f;

    CColPoint colPoint;
    CObject* pObject = static_cast<CObject*>(pThis);
    float fOldTimeStep = CTimer::ms_fTimeStep;
    if (pThis->physicalFlags.bDisableZ)
    {
        pThis->m_matrix = pThis->m_matrix;
        if (pThis->physicalFlags.bApplyGravity)
        {
            if (CTimer::ms_fTimeStep * pThis->m_vecMoveSpeed.z + pThis->m_matrix->pos.z < CWorld::SnookerTableMin.z)
            {
                pThis->m_matrix->pos.z = CWorld::SnookerTableMin.z;
                pThis->m_vecMoveSpeed = CVector(0.0f, 0.0f, 0.0f);
                pThis->m_vecTurnSpeed = CVector(0.0f, 0.0f, 0.0f);
            }
            goto LABEL_57;
        }

        float fTimeStep = 1000.0f;
        float fNewTimeStep0 = 1000.0f;
        float fNewPositionX = CTimer::ms_fTimeStep * pThis->m_vecMoveSpeed.x + pThis->m_matrix->pos.x;
        if (fNewPositionX <= CWorld::SnookerTableMax.x || pThis->m_vecMoveSpeed.x <= 0.0f)
        {
            if (fNewPositionX >= CWorld::SnookerTableMin.x || pThis->m_vecMoveSpeed.x >= 0.0f)
            {
            LABEL_12:
                float fNewPositionY = CTimer::ms_fTimeStep * pThis->m_vecMoveSpeed.y + pThis->m_matrix->pos.y;
                if (fNewPositionY <= CWorld::SnookerTableMax.y || pThis->m_vecMoveSpeed.y <= 0.0f)
                {
                    if (fNewPositionY >= CWorld::SnookerTableMin.y || pThis->m_vecMoveSpeed.y >= 0.0f)
                    {
                    LABEL_19:
                        bool bXIsLessThanY = true;
                        if (CWorld::SnookerTableMax.x - CWorld::SnookerTableMin.x < CWorld::SnookerTableMax.y
                            - CWorld::SnookerTableMin.y)
                        {
                            bXIsLessThanY = false;
                        }

                        float fNormalX = 0.0f;
                        float fNormalY = 0.0f;
                        if (fTimeStep < fNewTimeStep0 && fTimeStep < 1000.0f)
                        {
                            fNormalX = -1.0f;
                            //fMoveSpeedY = fabs(pThis->m_vecMoveSpeed.x);
                            if (pThis->m_vecMoveSpeed.x <= 0.0f)
                            {
                                fNormalX = 1.0f;
                            }

                            UpdateTimeStep(fTimeStep);
                            pThis->ApplyMoveSpeed();
                            pThis->ApplyTurnSpeed();

                            bool bUpdateMoveSpeedX = false;
                            if (CWorld::SnookerTableMax.x - CWorld::SnookerTableMin.x < CWorld::SnookerTableMax.y
                                - CWorld::SnookerTableMin.y)
                            {
                                // nothing
                            }
                            else 
                            {
                                float fSnookerTableY = (CWorld::SnookerTableMin.y + CWorld::SnookerTableMax.y) * 0.5f;
                                if (fSnookerTableY - 0.06f >= pThis->m_matrix->pos.y || pThis->m_matrix->pos.y >= fSnookerTableY + 0.06f)
                                {
                                    bUpdateMoveSpeedX = true;
                                }
                            }

                            if (CWorld::SnookerTableMax.y - 0.06 >= pThis->m_matrix->pos.y
                                && CWorld::SnookerTableMin.y + 0.06 <= pThis->m_matrix->pos.y
                                && bUpdateMoveSpeedX)
                            {
                                pThis->m_vecMoveSpeed.x = pThis->m_vecMoveSpeed.x * -1.0;
                                fNewTimeStep1 = fOldTimeStep - fTimeStep;
                            }
                            else
                            {
                                float fTimeStepMoveSpeedX = fOldTimeStep * pThis->m_vecMoveSpeed.x;
                                pThis->physicalFlags.bApplyGravity = true;
                                if (fTimeStepMoveSpeedX <= 0.03)
                                {
                                    if (fTimeStepMoveSpeedX < -0.03)
                                        pThis->m_vecMoveSpeed.x = -(0.03 / fOldTimeStep);
                                    fNewTimeStep1 = fOldTimeStep - fTimeStep;
                                }
                                else
                                {
                                    pThis->m_vecMoveSpeed.x = 0.03 / fOldTimeStep;
                                    fNewTimeStep1 = fOldTimeStep - fTimeStep;
                                }
                            }
                            goto LABEL_51;
                        }
                        if (fNewTimeStep0 < 1000.0f)
                        {
                            float fNormalY = -1.0f;
                            float fMoveSpeedY = fabs(pThis->m_vecMoveSpeed.y);
                            if (pThis->m_vecMoveSpeed.y <= 0.0f)
                            {
                                fNormalY = 1.0f;
                            }

                            UpdateTimeStep(fNewTimeStep0);
                            pThis->ApplyMoveSpeed();
                            pThis->ApplyTurnSpeed();
                           
                            float fNewMoveSpeedY = 0.0f;

                            bool bUpdateMoveSpeedY = false;
                            if (CWorld::SnookerTableMax.x - CWorld::SnookerTableMin.x < CWorld::SnookerTableMax.y
                                - CWorld::SnookerTableMin.y)
                            {
                                // nothing
                            }
                            else
                            {
                                float fSnookerTableX = (CWorld::SnookerTableMin.x + CWorld::SnookerTableMax.x) * 0.5f;
                                if (fSnookerTableX - 0.06 < pThis->m_matrix->pos.x && pThis->m_matrix->pos.x < fSnookerTableX + 0.06f)
                                {
                                    bUpdateMoveSpeedY = true;
                                }
                            }

                            if (CWorld::SnookerTableMax.x - 0.06f < pThis->m_matrix->pos.x
                                || CWorld::SnookerTableMin.x + 0.06f > pThis->m_matrix->pos.x
                                || bUpdateMoveSpeedY)
                            {
                                float fTimeStepMoveSpeedY = fOldTimeStep * pThis->m_vecMoveSpeed.y;
                                pThis->physicalFlags.bApplyGravity = true;
                                if (fTimeStepMoveSpeedY <= 0.03f)
                                {
                                    if (fTimeStepMoveSpeedY >= -0.03f)
                                    {
                                    LABEL_50:
                                        fNewTimeStep1 = fOldTimeStep - fNewTimeStep0;
                                    LABEL_51:
                                        //fNewTimeStep2 = fNewTimeStep1;
                                        UpdateTimeStep(fNewTimeStep1);
                                        if (fMoveSpeedY > 0.0f)
                                        {
                                            float fRadius = CModelInfo::ms_modelInfoPtrs[pThis->m_nModelIndex]->m_pColModel->m_boundSphere.m_fRadius;
                                            CVector thePosition = CVector(fNormalX * fRadius, fNormalY * fRadius, 0.0f);
                                            colPoint.m_vecPoint = pThis->GetPosition() - thePosition;
                                            colPoint.m_vecNormal = CVector(fNormalX, fNormalY, 0.0f);
                                            pThis->ApplyFriction(10.0f * fMoveSpeedY, &colPoint);

                                            if (pThis->m_nType == ENTITY_TYPE_OBJECT)
                                            {
                                                AudioEngine.ReportMissionAudioEvent(1016, pObject);
                                                pObject->m_nLastWeaponDamage = 4 * (pObject->m_nLastWeaponDamage == -1) + 50;
                                            }
                                        }
                                        goto LABEL_57;
                                    }
                                    fNewMoveSpeedY = -(0.03 / fOldTimeStep);
                                }
                                else
                                {
                                    fNewMoveSpeedY = 0.03 / fOldTimeStep;
                                }
                            }
                            else
                            {
                                fNewMoveSpeedY = pThis->m_vecMoveSpeed.y * -1.0;
                            }
                            pThis->m_vecMoveSpeed.y = fNewMoveSpeedY;
                            goto LABEL_50;
                        }
                    LABEL_57:
                        pThis->ApplyMoveSpeed();
                        pThis->ApplyTurnSpeed();
                        float fNewTimeStep = fOldTimeStep;
                        if (fOldTimeStep <= 0.00001f)
                        {
                            fNewTimeStep = 0.00001f;
                        }
                        CTimer::ms_fTimeStep = fNewTimeStep;
                        return;
                    }
                    fSnookerTableY = CWorld::SnookerTableMin.y;
                }
                else
                {
                    fSnookerTableY = CWorld::SnookerTableMax.y;
                }
                fNewTimeStep0 = (fSnookerTableY - pThis->m_matrix->pos.y) / pThis->m_vecMoveSpeed.y;
                goto LABEL_19;
            }
            fSnookerTableX = CWorld::SnookerTableMin.x;
        }
        else
        {
            fSnookerTableX = CWorld::SnookerTableMax.x;
        }
        fTimeStep = (fSnookerTableX - pThis->m_matrix->pos.x) / pThis->m_vecMoveSpeed.x;
        goto LABEL_12;
    }

    if (!pThis->physicalFlags.bDisableMoveForce || pThis->m_nType != ENTITY_TYPE_OBJECT || pObject->m_fDoorStartAngle <= -1000.0f)
    {
        goto LABEL_57;
    }

    float fDoorAngle = pObject->m_fDoorStartAngle;
    float fHeading = pThis->GetHeading();
    if (fDoorAngle + M_PI >= fHeading)
    {
        if (fDoorAngle - M_PI <= fHeading)
        {
            goto LABEL_72;
        }
        fHeading += (M_PI + M_PI);
    }
    else
    {
        fHeading -= (M_PI + M_PI);
    }

LABEL_72:
    float fNewTimeStep = -1000.0f;
    float fTheDoorAngle = 1.885f + fDoorAngle;
    if (pThis->m_vecTurnSpeed.z <= 0.0f
        || CTimer::ms_fTimeStep * pThis->m_vecTurnSpeed.z + fHeading <= fTheDoorAngle)
    {
        if (pThis->m_vecTurnSpeed.z < 0.0f)
        {
            fTheDoorAngle = fDoorAngle - 1.885f;
            if (CTimer::ms_fTimeStep * pThis->m_vecTurnSpeed.z + fHeading < fTheDoorAngle)
            {
                fNewTimeStep = (fTheDoorAngle - fHeading) / pThis->m_vecTurnSpeed.z;
            }
        }
    }
    else
    {
        fNewTimeStep = (fTheDoorAngle - fHeading) / pThis->m_vecTurnSpeed.z;
    }

    if (-CTimer::ms_fTimeStep <= fNewTimeStep)
    {
        UpdateTimeStep(fNewTimeStep);
        pThis->ApplyTurnSpeed();
        pThis->m_vecTurnSpeed.z = -0.2f * pThis->m_vecTurnSpeed.z;
        UpdateTimeStep(fOldTimeStep - fNewTimeStep);
        pThis->physicalFlags.b31 = true;
    }

    pThis->ApplyMoveSpeed();
    pThis->ApplyTurnSpeed();
    UpdateTimeStep(fOldTimeStep);

    if (pObject->objectFlags.bIsDoorMoving)
    {
        float fNewHeading = pThis->GetHeading();
        if (fNewHeading + M_PI >= fDoorAngle)
        {
            if (fNewHeading - M_PI > fDoorAngle)
            {
                fNewHeading = fNewHeading - (M_PI + M_PI);
            }
        }
        else
        {
            fNewHeading = fNewHeading + (M_PI + M_PI);
        }

        float fTheHeading = fHeading - fDoorAngle;
        float fTheNewHeading = fNewHeading - fDoorAngle;
        if (fabs(fTheHeading) < 0.001f)
        {
            fTheHeading = 0.0f;
        }

        if (fabs(fTheNewHeading) < 0.001f)
        {
            fTheNewHeading = 0.0f;
        }

        if (fTheHeading * fTheNewHeading < 0.0f)
        {
            pThis->m_vecTurnSpeed.z = 0.0f;
        }
    }
}

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
    //FUNCTION_SWITCH_CASE_2 = 3,
    //FUNCTION_SOMELABEL = 4
};

DWORD RETURN_HOOK_INSIDE_IF = 0x0;
DWORD RETURN_HOOK_OUTSIDE_IF = 0x0554ADD;
DWORD RETURN_HOOK_EXIT_WITH_GRACE = 0x0;
void _declspec(naked) HOOK_THEFUNCTION()
{
    _asm
    {
    }
}
