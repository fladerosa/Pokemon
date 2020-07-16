#ifndef MAPA_H
#define MAPA_H

#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/collections/queue.h>
#include "common_utils.h"
#include "inicializacion.h"

typedef enum{
	P_FREE = 0,
    P_CHASING,
    P_CATCHED
} e_pokemon_catch_state;

typedef struct {
    char* pokemon;
    t_position position;
} t_pokemon_receive;

typedef struct {
    char* pokemon;
    t_position position;
    e_pokemon_catch_state state;
} t_pokemon_on_map;

t_list* pokemonsOnMap;

void execThreadTrainerSetedFCFS(t_threadTrainer*, t_pokemon_on_map);
uint32_t calculateDistance(t_position positionFrom, t_position positionTo);
void calculate_cpu_burst_FF_threadTrainerOnExec(t_threadTrainer*, uint32_t);
void move_to_objetive_FCFS(t_threadTrainer*, t_pokemon_on_map);
void send_catch_pokemon(uint32_t, t_pokemon_on_map);

void execThreadTrainerSetedRR(t_threadTrainer*, t_pokemon_on_map);
void calculate_cpu_burst_RR_threadTrainerOnExec(t_threadTrainer*, uint32_t);
void move_to_objetive_RR(t_threadTrainer*, t_pokemon_on_map, uint32_t);


void execThreadTrainerSetedSJF_SD();
void execThreadTrainerSetedSJF_CD();

#endif