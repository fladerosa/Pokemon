#include "deadlock.h"
/*
void detectDeadlock(){
    //If exists a cycle of blocked trainers without disponible space for more pokemon, exists deadlock
    //cycleDeadLock = list_create();
    int blockedTrainersSize = list_size(blocked_trainers);
    for(int i = 0; i < blockedTrainersSize; i++){
        t_trainer* blockedTrainer = list_get(blocked_trainers, i);
        if(isCandidateDeadlock(blockedTrainer)){
            t_list* pokemonsNeeded = getPokemonsNeeded(blockedTrainer);
            for(int j = 0; j < list_size(pokemonsNeeded); j++){
                cycleDeadLock == getCycleDeadlock(blockedTrainer, (char*)list_get(pokemonsNeeded, j));

                if(list_size(cycleDeadLock) > 0){
                    setInterchangePokemon(list_get(cycleDeadLock, 0));
                }
            }
        }
    }
}

bool isCandidateDeadlock(t_trainer* blockedTrainer){
    if(list_size(blockedTrainer->pokemonOwned) != list_size(blockedTrainer->pokemonNeeded)){
        return false;
    }

    //If its on blocked state, and cant get another pokemon we assume that needs some pokemon that doesnot have
    return true;
}

t_list* getCycleDeadlock(t_trainer* blockedTrainer, char* pokemonNeeded){
    t_list* result = list_create();

    t_cycleDeadlock* cycleDeadlockNode = malloc(t_cycleDeadlock);
    cycleDeadlockNode->idTrainer = blockedTrainer->id_trainer;
    cycleDeadlockNode->pokemon = malloc(strlen(pokemonNeeded));
    strcpy(cycleDeadlockNode->pokemon, pokemonNeeded);
    

    return result;
}
*/