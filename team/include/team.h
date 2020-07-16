#ifndef TEAM_H
#define TEAM_H

#include "common_utils.h"
#include "inicializacion.h"
#include "mapa.h"

void calculateTrainerFromNewToReady();
void calculateTrainerToReady(enum_process_state threadTrainerState);
t_threadTrainer* getClosestTrainer(t_position position, enum_process_state threadTrainerState);
uint32_t calculateDistance(t_position positionFrom, t_position positionTo);
//Called always that a trainer its ready
void calculateTrainerFromReadyToExec();
bool existsThreadTrainerInExec(void* threadTrainer);
void setTrainerToExec();
void setTrainerToExec_FirstCome();
void setTrainerToExec_SJF();
//Called when a pokemon appear, on deadlock thread, and on message "caught pokemon"
void calculateLeaveBlockedFromAppear();
void calculateLeaveBlockedFromDeadlock(uint32_t idTrainer, t_position positionTo);
void calculateLeaveBlockedFromCaught(uint32_t idTrainer);
bool compareStrings(void* string1, void* string2);
void calculateTrainerInExit(uint32_t idTrainer);
void writeTrainerMetrics(uint32_t idTrainer);
bool trainerStateIsExit(void* threadTrainer);
void writeTeamMetrics();
void finishTeam();

#endif