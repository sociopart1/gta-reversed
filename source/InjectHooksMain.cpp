#include "StdInc.h"
#include "detours.h"

#pragma comment(lib, "detours.lib")

//bool __thiscall CTrain_ProcessControl(CPhysical* pThis)
auto OLD_CTrain_ProcessControl = (void(__thiscall*) (CTrain * pThis))0x6F86A0;
void __fastcall CTrain_ProcessControl(CTrain* pThis, void* padding);

void __cdecl HOOK_THEFUNCTION();

void InjectHooksMain(void)
{
    /*CAnimManager::InjectHooks();
    pTaskManager->InjectHooks();
    std::printf("okay, only CAnimManager and CTaskManager hooks\n ");
    CStreaming::InjectHooks();
    CRenderer::InjectHooks();*/

    ///*
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    std::printf("GOING TO HOOK FUNC NOW\n");
    DetourAttach(&(PVOID&)OLD_CTrain_ProcessControl, CTrain_ProcessControl);
    DetourTransactionCommit();
    //s*/
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

void __fastcall CTrain_ProcessControl(CTrain* pThis, void* padding)
{
    //printf(" calling CTrain_ProcessControl \n");

    pThis->vehicleFlags.bWarnedPeds = 0;
    pThis->m_vehicleAudio.Service();
    if (gbModelViewer)
    {
        return;
    }

    CVector vecOldTrainPosition = pThis->GetPosition();

    float fOldTrainHeading = pThis->m_placement.m_fHeading;
    if (pThis->m_matrix)
    {
        fOldTrainHeading = atan2(-pThis->m_matrix->up.x, pThis->m_matrix->up.y);
    }


    float fTotalTrackLength = arrTotalTrackLength[pThis->m_nTrackId];
    CTrainNode* pTrainNodes = pTrackNodes[pThis->m_nTrackId];
    int numTrackNodes = NumTrackNodes[pThis->m_nTrackId];

    if (pThis->trainFlags.bNotOnARailRoad == 0)
    {
        if (!pThis->trainFlags.bIsFrontCarriage)
        {
            CTrain* pPrevCarriage = pThis->m_pPrevCarriage;
            if (pPrevCarriage)
            {
                pThis->m_fTrainSpeed = pPrevCarriage->m_fTrainSpeed;
                pThis->m_fCurrentRailDistance = pPrevCarriage->m_fCurrentRailDistance + pThis->m_fLength;
            }
            else
            {
                pThis->m_fTrainSpeed *= pow(0.9900000095367432f, CTimer::ms_fTimeStep);
                pThis->m_fCurrentRailDistance += pThis->m_fTrainSpeed * CTimer::ms_fTimeStep;
            }

            if (pThis->trainFlags.b01 && pThis->trainFlags.bStoppedAtStation
                && pThis->m_nModelIndex == MODEL_STREAKC
                && !pThis->trainFlags.bMissionTrain)
            {
                CPlayerPed* pLocalPlayer = FindPlayerPed(-1);
                if (pThis->m_nPassengersGenerationState == TRAIN_PASSENGERS_QUERY_NUM_PASSENGERS_TO_LEAVE)
                {
                    if (pLocalPlayer->m_pVehicle == pThis)
                    {
                        pThis->m_nNumPassengersToLeave = 0;
                    }
                    else
                    {
                        pThis->m_nNumPassengersToLeave = (rand() & 3) + 1; // rand(1, 4)
                    }
                    pThis->m_nPassengersGenerationState = TRAIN_PASSENGERS_TELL_PASSENGERS_TO_LEAVE;
                }

                if (pThis->m_nPassengersGenerationState == TRAIN_PASSENGERS_TELL_PASSENGERS_TO_LEAVE)
                {
                    pThis->RemoveRandomPassenger();
                    if (pThis->m_nNumPassengersToLeave == 0)
                    {
                        pThis->m_nPassengersGenerationState = TRAIN_PASSENGERS_QUERY_NUM_PASSENGERS_TO_ENTER;
                    }
                }

                if (pThis->m_nPassengersGenerationState == TRAIN_PASSENGERS_QUERY_NUM_PASSENGERS_TO_ENTER)
                {
                    if (pLocalPlayer->m_pVehicle == pThis)
                    {
                        pThis->m_nNumPassengersToEnter = 0;
                    }
                    else
                    {
                        pThis->m_nNumPassengersToEnter = (rand() & 3) + 1; // rand(1, 4)
                    }
                    pThis->m_nPassengersGenerationState = TRAIN_PASSENGERS_TELL_PASSENGERS_TO_ENTER;
                }

                if (pThis->m_nPassengersGenerationState == TRAIN_PASSENGERS_TELL_PASSENGERS_TO_ENTER)
                {
                    if (pThis->trainFlags.bPassengersCanEnterAndLeave)
                    {
                        pThis->AddNearbyPedAsRandomPassenger();
                        if (pThis->m_nNumPassengersToLeave == pThis->m_nNumPassengersToEnter)
                        {
                            pThis->m_nPassengersGenerationState = TRAIN_PASSENGERS_GENERATION_FINISHED;
                        }
                    }
                }
            }

            goto MASSIVE_CODE;
        }
        else
        {
            CPad* pPad = CPad::GetPad(0);
            if (pThis->m_pDriver && pThis->m_pDriver->IsPlayer())
            {
                CPlayerPed* pLocalPlayer = static_cast<CPlayerPed*>(pThis->m_pDriver);
                pPad = pLocalPlayer->GetPadFromPlayer();
            }

            unsigned int numCarriagesPulled = pThis->FindNumCarriagesPulled();
            if (!pThis->trainFlags.bClockwiseDirection)
            {
                pThis->m_fTrainSpeed = -pThis->m_fTrainSpeed;
            }

            ///*
            if (pThis->m_nStatus)
            {
                bool bIsStreakModel = pThis->trainFlags.bIsStreakModel;
                float fStopAtStationSpeed = static_cast<float>(pThis->m_autoPilot.m_nCruiseSpeed);

                unsigned int timeInMilliSeconds = CTimer::m_snTimeInMilliseconds;
                unsigned int timeAtStation = CTimer::m_snTimeInMilliseconds - pThis->m_nTimeWhenStoppedAtStation;
                if (timeAtStation >= (bIsStreakModel ? 20000 : 10000))
                {
                    if (timeAtStation >= (bIsStreakModel ? 28000 : 18000))
                    {
                        if (timeAtStation >= (bIsStreakModel ? 32000 : 22000))
                        {
                            if (pThis->trainFlags.bStopsAtStations)
                            {
                                float maxTrainSpeed = 0.0f;
                                if (pThis->FindMaximumSpeedToStopAtStations(&maxTrainSpeed))
                                {
                                    fStopAtStationSpeed = 0.0;
                                    pThis->m_nTimeWhenStoppedAtStation = timeInMilliSeconds;
                                }
                                else
                                {
                                    if (fStopAtStationSpeed >= maxTrainSpeed)
                                    {
                                        fStopAtStationSpeed = maxTrainSpeed;
                                    }
                                }
                            }
                        }
                        else if (pThis->trainFlags.bStoppedAtStation)
                        {
                            CTrain* pTrainCarriage = pThis;
                            do
                            {
                                pThis->trainFlags.bStoppedAtStation = false;
                                pTrainCarriage->m_nPassengersGenerationState = TRAIN_PASSENGERS_GENERATION_FINISHED;
                                pTrainCarriage = pTrainCarriage->m_pNextCarriage;
                            } while (pTrainCarriage);
                        }
                    }
                    else
                    {
                        fStopAtStationSpeed = 0.0;
                        if (pThis->trainFlags.bStoppedAtStation)
                        {
                            CTrain* pTrainCarriage = pThis;
                            do
                            {
                                pThis->trainFlags.bPassengersCanEnterAndLeave = false;
                                pTrainCarriage->m_nPassengersGenerationState = TRAIN_PASSENGERS_GENERATION_FINISHED;
                                pTrainCarriage = pTrainCarriage->m_pNextCarriage;
                            } while (pTrainCarriage);
                        }
                    }
                }
                else
                {
                    fStopAtStationSpeed = 0.0;
                    if (!pThis->trainFlags.bStoppedAtStation)
                    {
                        CTrain* pTrainCarriage = pThis;
                        do
                        {
                            pThis->trainFlags.bStoppedAtStation = true;
                            pThis->trainFlags.bPassengersCanEnterAndLeave = true;
                            pTrainCarriage->m_nPassengersGenerationState = TRAIN_PASSENGERS_QUERY_NUM_PASSENGERS_TO_LEAVE;
                            pTrainCarriage = pTrainCarriage->m_pNextCarriage;
                        } while (pTrainCarriage);
                    }
                }

                fStopAtStationSpeed = fStopAtStationSpeed * 0.02 - pThis->m_fTrainSpeed;
                if (fStopAtStationSpeed > 0.0)
                {
                    pThis->m_fTrainGas = fStopAtStationSpeed * 30.0;
                    if (pThis->m_fTrainGas >= 1.0)
                    {
                        pThis->m_fTrainGas = 1.0;
                    }

                    pThis->m_fTrainGas *= 255.0f;
                    pThis->m_fTrainBrake = 0.0f;
                }
                else
                {
                    float fTrainSpeed = fStopAtStationSpeed * -30.0f;
                    pThis->m_fTrainGas = 0.0f;
                    if (fTrainSpeed >= 1.0f)
                    {
                        fTrainSpeed = 1.0f;
                    }
                    pThis->m_fTrainBrake = fTrainSpeed * 255.0f;
                }
            }
            else
            {
                float fTrainSpeed = pThis->m_fTrainSpeed;
                if (fTrainSpeed < 0.0f)
                {
                    fTrainSpeed = -fTrainSpeed;
                }

                if (fTrainSpeed < 0.001f)
                {
                    pThis->m_fTrainBrake = 0.0;
                    pThis->m_fTrainGas = static_cast<float>(pPad->GetAccelerate() - pPad->GetBrake());
                    goto APPLY_BRAKES;
                }
                else
                {
                    if (pThis->m_fTrainSpeed > 0.0)
                    {
                        pThis->m_fTrainBrake = static_cast<float>(pPad->GetBrake());
                        pThis->m_fTrainGas = static_cast<float>(pPad->GetAccelerate());
                        goto APPLY_BRAKES;
                    }
                    else
                    {
                        pThis->m_fTrainGas = static_cast<float>(-pPad->GetBrake());
                        pThis->m_fTrainBrake = static_cast<float>(pPad->GetAccelerate());
                    }
                }
            }
            goto APPLY_BRAKES;

            //APPLY_BRAKES:
            //    const int i = 0;

        APPLY_BRAKES:
            if (pThis->trainFlags.bForceSlowDown)
            {
                CVector vecPoint = pThis->GetPosition();
                CVector vecDistance;
                if (CGameLogic::CalcDistanceToForbiddenTrainCrossing(vecPoint, pThis->m_vecMoveSpeed, true, &vecDistance) < 230.0f)
                {
                    CVector vecTopDirection;
                    pThis->GetTopDirection(&vecTopDirection);
                    if (vecDistance.z * vecTopDirection.z + vecDistance.y * vecTopDirection.y + vecDistance.x * vecTopDirection.x <= 0.0f)
                    {
                        pThis->m_fTrainGas = std::max(0.0f, pThis->m_fTrainGas);
                    }
                    else
                    {
                        pThis->m_fTrainGas = std::min(0.0f, pThis->m_fTrainGas);
                    }

                    if (CGameLogic::CalcDistanceToForbiddenTrainCrossing(vecPoint, pThis->m_vecMoveSpeed, false, &vecDistance) < 230.0f)
                    {
                        pThis->m_fTrainBrake = 512.0f;

                    }
                }
            }

            numCarriagesPulled += 3;

            pThis->m_fTrainSpeed += pThis->m_fTrainGas * 0.00390625 * CTimer::ms_fTimeStep * 0.0020000001 / numCarriagesPulled;

            if (pThis->m_fTrainBrake != 0.0f)
            {
                float fTrainSpeed = pThis->m_fTrainSpeed;
                if (pThis->m_fTrainSpeed < 0.0)
                {
                    fTrainSpeed = -fTrainSpeed;
                }
                float fBreak = pThis->m_fTrainBrake * 0.00390625 * CTimer::ms_fTimeStep * 0.0060000001 / numCarriagesPulled;
                if (fTrainSpeed >= fBreak)
                {
                    if (pThis->m_fTrainSpeed < 0.0)
                    {
                        pThis->m_fTrainSpeed += fBreak;
                    }
                    else
                    {
                        pThis->m_fTrainSpeed -= fBreak;
                    }
                }
                else
                {
                    pThis->m_fTrainSpeed = 0.0;
                }
            }

            pThis->m_fTrainSpeed *= pow(0.999750018119812, CTimer::ms_fTimeStep);
            if (!pThis->trainFlags.bClockwiseDirection)
            {
                pThis->m_fTrainSpeed = -pThis->m_fTrainSpeed;
            }

            pThis->m_fCurrentRailDistance += CTimer::ms_fTimeStep * pThis->m_fTrainSpeed;

            if (pThis->m_nStatus == STATUS_PLAYER)
            {

                float fTheTrainSpeed = pThis->m_fTrainSpeed;
                if (fTheTrainSpeed < 0.0)
                {
                    fTheTrainSpeed = -fTheTrainSpeed;
                }
                if (fTheTrainSpeed > 1.0 * 0.94999999)
                {
                    CPad::GetPad(0)->StartShake(300, 0x46u, 0);
                    CVector vecVehiclePosition = pThis->GetPosition();
                    TheCamera.CamShake(0.1f, vecVehiclePosition.x, vecVehiclePosition.y, vecVehiclePosition.z);
                }

                fTheTrainSpeed = pThis->m_fTrainSpeed;
                if (fTheTrainSpeed < 0.0)
                {
                    fTheTrainSpeed = -fTheTrainSpeed;
                }

                if (fTheTrainSpeed > 1.0f)
                {
                    int nNodeIndex = pThis->m_nNodeIndex;
                    int previousNodeIndex = nNodeIndex - 1;
                    if (previousNodeIndex < 0)
                    {
                        previousNodeIndex = numTrackNodes;
                    }

                    int previousNodeIndex2 = previousNodeIndex - 1;
                    if (previousNodeIndex2 < 0)
                    {
                        previousNodeIndex2 = numTrackNodes;
                    }

                    CTrainNode* pCurrentTrainNode = &pTrainNodes[pThis->m_nNodeIndex];
                    CTrainNode* pPreviousTrainNode = &pTrainNodes[previousNodeIndex];
                    CTrainNode* pPreviousTrainNode2 = &pTrainNodes[previousNodeIndex2];

                    CVector* currentNodePosition = &pCurrentTrainNode->GetPosn();
                    CVector* previousNodePosition = &pPreviousTrainNode->GetPosn();
                    CVector* previousNodePosition2 = &pPreviousTrainNode2->GetPosn();

                    CVector vecDifference1, vecDifference2;
                    VectorSub(&vecDifference1, currentNodePosition, previousNodePosition);
                    VectorSub(&vecDifference2, previousNodePosition, previousNodePosition2);
                    vecDifference1.Normalise();
                    vecDifference2.Normalise();

                    if (vecDifference1.x * vecDifference2.x +
                        vecDifference1.y * vecDifference2.y +
                        vecDifference1.z * vecDifference2.z < 0.99599999f)
                    {
                        CTrain* pCarriage = pThis;
                        bool bIsInTunnel = false;
                        while (!bIsInTunnel)
                        {
                            bIsInTunnel = pCarriage->IsInTunnel();
                            pCarriage = pCarriage->m_pNextCarriage;
                            if (!pCarriage)
                            {
                                if (!bIsInTunnel)
                                {
                                    CTrain* pTheTrainCarriage = pThis;
                                    do
                                    {
                                        pThis->trainFlags.bNotOnARailRoad = true;
                                        pTheTrainCarriage->physicalFlags.bDisableCollisionForce = false;
                                        pTheTrainCarriage->physicalFlags.b18 = false;
                                        pTheTrainCarriage->SetIsStatic(false);
                                        pTheTrainCarriage = pTheTrainCarriage->m_pNextCarriage;
                                    } while (pTheTrainCarriage);

                                    pThis->CPhysical::ProcessControl();
                                }
                                break;
                            }
                        }
                    }
                }
            }
            goto MASSIVE_CODE;
        }

    MASSIVE_CODE:
        if (pThis->m_fCurrentRailDistance < 0.0)
        {
            do
            {
                pThis->m_fCurrentRailDistance += fTotalTrackLength;
            } while (pThis->m_fCurrentRailDistance < 0.0);
        }

        if (pThis->m_fCurrentRailDistance >= fTotalTrackLength)
        {
            do
            {
                pThis->m_fCurrentRailDistance -= fTotalTrackLength;
            } while (pThis->m_fCurrentRailDistance >= fTotalTrackLength);
        }

        float fNextNodeTrackLength = 0.0f;
        int nextNodeIndex = pThis->m_nNodeIndex + 1;
        if (nextNodeIndex < numTrackNodes)
        {
            CTrainNode* pNextTrainNode = &pTrainNodes[nextNodeIndex];
            fNextNodeTrackLength = pNextTrainNode->m_nDistanceFromStart * 0.33333334;
        }
        else
        {
            fNextNodeTrackLength = fTotalTrackLength;
            nextNodeIndex = 0;
        }

        CTrainNode* pTheTrainNode = &pTrainNodes[pThis->m_nNodeIndex];
        float fCurrentNodeTrackLength = pTheTrainNode->m_nDistanceFromStart * 0.33333334;
        while (pThis->m_fCurrentRailDistance < fCurrentNodeTrackLength || fNextNodeTrackLength < pThis->m_fCurrentRailDistance)
        {
            int newNodeIndex = pThis->m_nNodeIndex - 1; // previous node
            if (fCurrentNodeTrackLength <= pThis->m_fCurrentRailDistance)
            {
                newNodeIndex = pThis->m_nNodeIndex + 1; // next node
            }
            pThis->m_nNodeIndex = newNodeIndex % numTrackNodes;
            pThis->m_vehicleAudio.AddAudioEvent(100, 0.0);

            pTheTrainNode = &pTrainNodes[pThis->m_nNodeIndex];
            fCurrentNodeTrackLength = pTheTrainNode->m_nDistanceFromStart * 0.33333334;

            nextNodeIndex = pThis->m_nNodeIndex + 1;
            if (nextNodeIndex < numTrackNodes)
            {
                CTrainNode* pNextTrainNode = &pTrainNodes[nextNodeIndex];
                fNextNodeTrackLength = pNextTrainNode->m_nDistanceFromStart * 0.33333334;
            }
            else
            {
                fNextNodeTrackLength = fTotalTrackLength;
                nextNodeIndex = 0;
            }
        }

        CTrainNode* pNextTrainNode = &pTrainNodes[nextNodeIndex];
        fNextNodeTrackLength = pNextTrainNode->m_nDistanceFromStart * 0.33333334;

        float fTrackNodeDifference = fNextNodeTrackLength - fCurrentNodeTrackLength;
        if (fTrackNodeDifference < 0.0f)
        {
            fTrackNodeDifference += fTotalTrackLength;
        }

        float fTheDistance = (pThis->m_fCurrentRailDistance - fCurrentNodeTrackLength) / fTrackNodeDifference;
        CVector vecPosition1;
        vecPosition1.x = pTheTrainNode->x * 0.125 * (1.0 - fTheDistance) + pNextTrainNode->x * 0.125 * fTheDistance;
        vecPosition1.y = pTheTrainNode->y * 0.125 * (1.0 - fTheDistance) + pNextTrainNode->y * 0.125 * fTheDistance;
        vecPosition1.z = pTheTrainNode->z * 0.125 * (1.0 - fTheDistance) + pNextTrainNode->z * 0.125 * fTheDistance;


        CColModel* pVehicleColModel = CModelInfo::ms_modelInfoPtrs[pThis->m_nModelIndex]->m_pColModel;
        CBoundingBox* pBoundingBox = &pVehicleColModel->m_boundBox;
        float fTotalCurrentRailDistance = pBoundingBox->m_vecMax.y - pBoundingBox->m_vecMin.y + pThis->m_fCurrentRailDistance;
        if (fTotalCurrentRailDistance > fTotalTrackLength)
        {
            fTotalCurrentRailDistance -= fTotalTrackLength;
        }

        nextNodeIndex = pThis->m_nNodeIndex + 1;
        if (nextNodeIndex < numTrackNodes)
        {
            CTrainNode* pNextTrainNode = &pTrainNodes[nextNodeIndex];
            fNextNodeTrackLength = pNextTrainNode->m_nDistanceFromStart * 0.33333334;
        }
        else
        {
            fNextNodeTrackLength = fTotalTrackLength;
            nextNodeIndex = 0;
        }

        int trainNodeIndex = pThis->m_nNodeIndex;
        while (fTotalCurrentRailDistance < fCurrentNodeTrackLength || fTotalCurrentRailDistance > fNextNodeTrackLength)
        {
            trainNodeIndex = (trainNodeIndex + 1) % numTrackNodes;

            pTheTrainNode = &pTrainNodes[trainNodeIndex];
            fCurrentNodeTrackLength = pTheTrainNode->m_nDistanceFromStart * 0.33333334;

            nextNodeIndex = trainNodeIndex + 1;
            if (nextNodeIndex < numTrackNodes)
            {
                CTrainNode* pNextTrainNode = &pTrainNodes[nextNodeIndex];
                fNextNodeTrackLength = pNextTrainNode->m_nDistanceFromStart * 0.33333334;
            }
            else
            {
                fNextNodeTrackLength = fTotalTrackLength;
                nextNodeIndex = 0;
            }
        }

        pNextTrainNode = &pTrainNodes[nextNodeIndex];
        fNextNodeTrackLength = pNextTrainNode->m_nDistanceFromStart * 0.33333334;

        fTrackNodeDifference = fNextNodeTrackLength - fCurrentNodeTrackLength;
        if (fTrackNodeDifference < 0.0)
        {
            fTrackNodeDifference += fTotalTrackLength;
        }

        fTheDistance = (fTotalCurrentRailDistance - fCurrentNodeTrackLength) / fTrackNodeDifference;
        CVector vecPosition2;
        vecPosition2.x = pTheTrainNode->x * 0.125 * (1.0 - fTheDistance) + pNextTrainNode->x * 0.125 * fTheDistance;
        vecPosition2.y = pTheTrainNode->y * 0.125 * (1.0 - fTheDistance) + pNextTrainNode->y * 0.125 * fTheDistance;
        vecPosition2.z = pTheTrainNode->z * 0.125 * (1.0 - fTheDistance) + pNextTrainNode->z * 0.125 * fTheDistance;

        {
            CVector& vecVehiclePosition = pThis->GetPosition();
            vecVehiclePosition = (vecPosition1 + vecPosition2) * 0.5f;
            vecVehiclePosition.z += pThis->m_pHandlingData->m_fSuspensionLowerLimit - pBoundingBox->m_vecMin.z;
        }

        pThis->m_matrix->up = vecPosition2 - vecPosition1;
        pThis->m_matrix->up.Normalise();
        if (!pThis->trainFlags.bClockwiseDirection)
        {
            pThis->m_matrix->up *= -1.0f;
        }

        CVector vecTemp(0.0f, 0.0f, 1.0f);

        CrossProduct(&pThis->m_matrix->right, &pThis->m_matrix->up, &vecTemp);
        pThis->m_matrix->right.Normalise();

        CrossProduct(&pThis->m_matrix->at, &pThis->m_matrix->right, &pThis->m_matrix->up);

        unsigned char trainNodeLighting = pTheTrainNode->GetLightingFromCollision();;
        unsigned char trainNextNodeLighting = pNextTrainNode->GetLightingFromCollision();

        float fTrainNodeLighting = ScaleLighting(trainNodeLighting, 0.5);
        float fTrainNextNodeLighting = ScaleLighting(trainNextNodeLighting, 0.5);

        fTrainNodeLighting += (fTrainNextNodeLighting - fTrainNodeLighting) * fTheDistance;
        pThis->m_fContactSurfaceBrightness = fTrainNodeLighting;
        pThis->m_vecMoveSpeed = (1.0f / CTimer::ms_fTimeStep) * (pThis->GetPosition() - vecOldTrainPosition);

        float fNewTrainHeading = pThis->m_placement.m_fHeading;
        if (pThis->m_matrix)
        {
            fNewTrainHeading = atan2(-pThis->m_matrix->up.x, pThis->m_matrix->up.y);
        }

        float fHeading = fNewTrainHeading - fOldTrainHeading;
        if (fHeading <= 3.1415927)
        {
            if (fHeading < -3.1415927)
            {
                fHeading += 6.2831855;
            }
        }
        else
        {
            fHeading -= 6.2831855;
        }

        pThis->m_vecTurnSpeed = CVector(0.0f, 0.0f, fHeading / CTimer::ms_fTimeStep);

        if (pThis->trainFlags.bNotOnARailRoad)
        {
            pThis->m_vecMoveSpeed *= -1.0f;
            pThis->m_vecTurnSpeed *= -1.0f;

            pThis->ApplyMoveSpeed();

            pThis->m_vecMoveSpeed *= -1.0f;
            pThis->m_vecTurnSpeed *= -1.0f;

            pThis->ProcessControl();
        }
        else
        {
            float fVehicleMoveSpeedX = pThis->m_vecMoveSpeed.x;
            if (fVehicleMoveSpeedX <= -2.0)
            {
                fVehicleMoveSpeedX = -2.0;
            }

            if (fVehicleMoveSpeedX >= 2.0f)
            {
                fVehicleMoveSpeedX = 2.0;
            }
            pThis->m_vecMoveSpeed.x = fVehicleMoveSpeedX;

            float fVehicleMoveSpeedY = pThis->m_vecMoveSpeed.y;
            if (fVehicleMoveSpeedY <= -2.0)
            {
                fVehicleMoveSpeedY = -2.0;
            }

            if (fVehicleMoveSpeedY >= 2.0f)
            {
                fVehicleMoveSpeedY = 2.0;
            }
            pThis->m_vecMoveSpeed.y = fVehicleMoveSpeedY;

            float fVehicleMoveSpeedZ = pThis->m_vecMoveSpeed.z;
            if (fVehicleMoveSpeedZ <= -2.0)
            {
                fVehicleMoveSpeedZ = -2.0;
            }

            if (fVehicleMoveSpeedZ >= 2.0f)
            {
                fVehicleMoveSpeedZ = 2.0;
            }
            pThis->m_vecMoveSpeed.z = fVehicleMoveSpeedZ;

            float fVehicleTurnSpeedX = pThis->m_vecTurnSpeed.x;
            if (fVehicleTurnSpeedX <= -0.1)
            {
                fVehicleTurnSpeedX = -0.1;
            }

            if (fVehicleTurnSpeedX >= 0.1f)
            {
                fVehicleTurnSpeedX = 0.1;
            }
            pThis->m_vecTurnSpeed.x = fVehicleTurnSpeedX;

            float fVehicleTurnSpeedY = pThis->m_vecTurnSpeed.y;
            if (fVehicleTurnSpeedY <= -0.1)
            {
                fVehicleTurnSpeedY = -0.1;
            }

            if (fVehicleTurnSpeedY >= 0.1f)
            {
                fVehicleTurnSpeedY = 0.1;
            }
            pThis->m_vecTurnSpeed.y = fVehicleTurnSpeedY;

            float fVehicleTurnSpeedZ = pThis->m_vecTurnSpeed.z;
            if (fVehicleTurnSpeedZ <= -0.1)
            {
                fVehicleTurnSpeedZ = -0.1;
            }

            if (fVehicleTurnSpeedZ >= 0.1f)
            {
                fVehicleTurnSpeedZ = 0.1;
            }
            pThis->m_vecTurnSpeed.z = fVehicleTurnSpeedZ;
        }

        RwObject* pRwObject = pThis->m_pRwObject;
        if (pRwObject)
        {
            RwMatrix* pRwMatrix = &((RwFrame*)pRwObject->parent)->modelling;
            if (pThis->m_matrix)
            {
                pThis->m_matrix->UpdateRwMatrix(pRwMatrix);
            }
            else
            {
                pThis->m_placement.UpdateRwMatrix(pRwMatrix);
            }
        }
        pThis->UpdateRwFrame();
        pThis->RemoveAndAdd();

        pThis->m_bIsStuck = false;
        pThis->m_bWasPostponed = false;
        pThis->m_bIsInSafePosition = true;

        pThis->m_fMovingSpeed = (vecOldTrainPosition - pThis->GetPosition()).Magnitude();

        if (pThis->trainFlags.bIsFrontCarriage || pThis->trainFlags.bIsLastCarriage)
        {
            CVector vecPoint = pBoundingBox->m_vecMax.y * pThis->m_matrix->up;
            vecPoint += pThis->GetPosition();
            vecPoint += CTimer::ms_fTimeStep * pThis->m_vecMoveSpeed;

            MarkSurroundingEntitiesForCollisionWithTrain(vecPoint, 3.0f, pThis, false);
        }

        if (!pThis->vehicleFlags.bWarnedPeds)
        {
            CCarCtrl::ScanForPedDanger(pThis);
        }
        return;
    }
    else
    {
        if (!pThis->m_bIsStuck)
        {
            float fMaxForce = 0.003f;
            float fMaxTorque = 0.00090000004f;
            float fMaxMovingSpeed = 0.0049999999f; // 0.005f;

            if (pThis->m_nStatus != STATUS_PLAYER)
            {
                fMaxForce = 0.006;
                fMaxTorque = 0.0015f;
                fMaxMovingSpeed = 0.015f;
            }

            float fMaxForceTimeStep = (fMaxForce * CTimer::ms_fTimeStep) * (fMaxForce * CTimer::ms_fTimeStep);
            float fMaxTorqueTimeStep = (fMaxTorque * CTimer::ms_fTimeStep) * (fMaxTorque * CTimer::ms_fTimeStep);

            pThis->m_vecForce = (pThis->m_vecForce + pThis->m_vecMoveSpeed) * 0.5f;
            pThis->m_vecTorque = (pThis->m_vecTorque + pThis->m_vecTurnSpeed) * 0.5f;

            if (pThis->m_vecForce.SquaredMagnitude() > fMaxForceTimeStep
                || pThis->m_vecTorque.SquaredMagnitude() > fMaxTorqueTimeStep
                || pThis->m_fMovingSpeed >= fMaxMovingSpeed
                || pThis->m_fDamageIntensity > 0.0
                && pThis->m_pDamageEntity != 0
                && (pThis->m_pDamageEntity->m_nType == ENTITY_TYPE_PED)
                )
            {
                pThis->m_bFakePhysics = 0;
            }
            else
            {

                pThis->m_bFakePhysics += 1;
                if (pThis->m_bFakePhysics > 10)
                {

                    //if (!dummy_424100()) dummy_424100 returns 0, nothing else
                    {
                        if (pThis->m_bFakePhysics > 10)
                        {
                            pThis->m_bFakePhysics = 10;
                        }

                        pThis->m_vecMoveSpeed = CVector();
                        pThis->m_vecTurnSpeed = CVector();
                        pThis->SkipPhysics();
                        return;
                    }
                }
            }
        }

        pThis->CPhysical::ProcessControl();

        CVector vecMoveForce, vecTurnForce;
        if (mod_Buoyancy.ProcessBuoyancy(pThis, pThis->m_fBuoyancyConstant, &vecMoveForce, &vecTurnForce))
        {
            pThis->physicalFlags.bTouchingWater = true;

            float fTimeStep = 0.0099999998;
            if (CTimer::ms_fTimeStep >= 0.0099999998)
            {
                fTimeStep = CTimer::ms_fTimeStep;
            }

            float fSpeedFactor = 1.0f - vecOldTrainPosition.z / (fTimeStep * pThis->m_fMass * 0.0080000004f) * 0.050000001f;
            fSpeedFactor = pow(fSpeedFactor, CTimer::ms_fTimeStep);

            pThis->m_vecMoveSpeed *= fSpeedFactor;
            pThis->m_vecTurnSpeed *= fSpeedFactor;
            pThis->ApplyMoveForce(vecOldTrainPosition.x, vecOldTrainPosition.y, vecOldTrainPosition.z);
            pThis->ApplyTurnForce(vecTurnForce, vecMoveForce);
        }
    }
}
