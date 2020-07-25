#include "deadlock.h"

void detectDeadlock_do(){
    //If exists a cycle of blocked trainers without disponible space for more pokemon, exists deadlock
    int threadsTrainersCount = list_size(threadsTrainers);
    t_threadTrainer* threadTrainerAux;
    t_trainer* trainerAux;
    t_cycleDeadlock* deadlockNode;
    char* pokemonNeededAux;
    bool continueAnalize = true;

    if(flagExistsDeadlock) return;

    log_info(obligatory_logger, "Inicia algoritmo de detección de deadlock.");
    for(int i = 0; i < threadsTrainersCount && continueAnalize; i++){
        threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, i);
        trainerAux = (t_trainer*)list_get(trainers, i);
        
        if(threadTrainerAux->state == BLOCKED && isCandidateDeadlock(trainerAux)){
            t_list* pokemonsNeeded = getPokemonsNeeded(trainerAux);
            for(int j = 0; j < list_size(pokemonsNeeded) && continueAnalize; j++){
                pokemonNeededAux = (char*)list_get(pokemonsNeeded, j);
                cycleDeadLock = list_create();
                deadlockNode = malloc(sizeof(t_cycleDeadlock));
                deadlockNode->idTrainer = trainerAux->id_trainer;
                deadlockNode->pokemon = malloc(strlen(pokemonNeededAux)+1);
                strcpy(deadlockNode->pokemon, pokemonNeededAux);
                list_add(cycleDeadLock, (void*)deadlockNode);
                if(!completeCycleDeadlock()){
                    list_remove_and_destroy_element(cycleDeadLock, 0, (void*)destroy_cycleNode);
                    list_destroy_and_destroy_elements(cycleDeadLock, (void*)destroy_cycleNode);
                }else{
                    if(existsDeadlock()){
                        log_info(obligatory_logger, "Existe deadlock.");
                        flagExistsDeadlock = true;
                        deadlockCount++;
                        setInterchangePokemon();
                        continueAnalize = false;
                    }else{
                        list_destroy_and_destroy_elements(cycleDeadLock, (void*)destroy_cycleNode);
                    }
                }
            }
            list_destroy(pokemonsNeeded);
        }
    }

    if(continueAnalize){
        log_info(obligatory_logger, "No se detectó deadlock.");
    }else{
        log_info(obligatory_logger, "Se detectó deadlock.");
    }
}

t_list* getPokemonsNeeded(t_trainer* trainerAux){
    char* pokemonOwnedAux;
    t_list* pokemonsNeeded = list_duplicate(trainerAux->pokemonNeeded);

    for (int i=0; i<list_size(trainerAux->pokemonOwned); i++){
        pokemonOwnedAux = (char*)list_get(trainerAux->pokemonOwned, i);
        pokemonCompareDeadlock = malloc(strlen(pokemonOwnedAux)+1);
        strcpy(pokemonCompareDeadlock, pokemonOwnedAux);
        list_remove_by_condition(pokemonsNeeded, comparePokemonDeadlock);
        free(pokemonCompareDeadlock);
    }

    return pokemonsNeeded;
}

void* destroy_cycleNode(void* pointer){
    t_cycleDeadlock* cycleDeadLockNode = (t_cycleDeadlock*)pointer;
    free(cycleDeadLockNode->pokemon);
    free(cycleDeadLockNode);

    return NULL;
}

bool comparePokemonDeadlock(void* pokemonOwn){
    return (strcmp((char*)pokemonOwn, pokemonCompareDeadlock) == 0);
}

bool isCandidateDeadlock(t_trainer* blockedTrainer){
    if(list_size(blockedTrainer->pokemonOwned) != list_size(blockedTrainer->pokemonNeeded)){
        return false;
    }

    //If its on blocked state, and cant get another pokemon we assume that needs some pokemon that doesnot have
    return true;
}

bool completeCycleDeadlock(){
    //the cycle already has the elements, so i look for the next to add
    t_cycleDeadlock* deadlockLastNode = (t_cycleDeadlock*)list_get(cycleDeadLock, list_size(cycleDeadLock) - 1);
    int threadsTrainersCount = list_size(threadsTrainers);
    t_threadTrainer* threadTrainerAux;
    t_trainer* trainerAux;
    for(int i = 0; i<threadsTrainersCount; i++){
        threadTrainerAux = (t_threadTrainer*)list_get(threadsTrainers, i);
        trainerAux = (t_trainer*)list_get(trainers, i);
        if(threadTrainerAux->state == BLOCKED && isCandidateDeadlock(trainerAux) && trainerHasPokemonNoNeeded(trainerAux, deadlockLastNode->pokemon)){
            t_list* pokemonsNeeded = getPokemonsNeeded(trainerAux);
            for(int j = 0; j < list_size(pokemonsNeeded); j++){
                if(trainerAlreadyInCycleCount(threadTrainerAux->idTrainer) > list_size(pokemonsNeeded)+1) return false;
                char* pokemonNeededAux = (char*)list_get(pokemonsNeeded, j);
                t_cycleDeadlock* deadlockNode = malloc(sizeof(t_cycleDeadlock));
                deadlockNode->idTrainer = trainerAux->id_trainer;
                deadlockNode->pokemon = malloc(strlen(pokemonNeededAux)+1);
                strcpy(deadlockNode->pokemon, pokemonNeededAux);
                list_add(cycleDeadLock, (void*)deadlockNode);

                if(existsDeadlock()){
                    return true;
                }else{
                    if(!completeCycleDeadlock()){
                        list_remove_and_destroy_element(cycleDeadLock, list_size(cycleDeadLock)-1, (void*)destroy_cycleNode);
                    }else{
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

int trainerAlreadyInCycleCount(uint32_t idTrainer){
    int count=0;
    t_cycleDeadlock* cycleDeadLockAux;

    for(int i=0; i<list_size(cycleDeadLock); i++){
        cycleDeadLockAux = (t_cycleDeadlock*)list_get(cycleDeadLock, i);
        if(cycleDeadLockAux->idTrainer == idTrainer) count++;
    }

    return count;
}

bool trainerHasPokemonNoNeeded(t_trainer* trainerAux, char* pokemonNeeded){
    pokemonCompareDeadlock = malloc(strlen(pokemonNeeded)+1);
    strcpy(pokemonCompareDeadlock, pokemonNeeded);
    bool result = list_count_satisfying(trainerAux->pokemonOwned, comparePokemonDeadlock) > list_count_satisfying(trainerAux->pokemonNeeded, comparePokemonDeadlock);
    free(pokemonCompareDeadlock);
    return result;
}

bool existsDeadlock(){
    t_cycleDeadlock* firstNode = (t_cycleDeadlock*)list_get(cycleDeadLock, 0);
    t_cycleDeadlock* lastNode = (t_cycleDeadlock*)list_get(cycleDeadLock, list_size(cycleDeadLock)-1);

    return firstNode->idTrainer == lastNode->idTrainer && strcmp(firstNode->pokemon, lastNode->pokemon) == 0;
}

void setInterchangePokemon(){
    t_cycleDeadlock* cycleDeadlockAux = (t_cycleDeadlock*)list_get(cycleDeadLock, 0);
    t_threadTrainer* threadTrainerToMove = (t_threadTrainer*)list_get(threadsTrainers, cycleDeadlockAux->idTrainer - 1);
    cycleDeadlockAux = (t_cycleDeadlock*)list_get(cycleDeadLock, 1);
    t_trainer* trainerWithDestiny = (t_trainer*)list_get(trainers, cycleDeadlockAux->idTrainer - 1);

    log_info(obligatory_logger, "El entrenador %d intercambiará con el entrenador %d", threadTrainerToMove->idTrainer, trainerWithDestiny->id_trainer);

    threadTrainerToMove->positionTo.posx = trainerWithDestiny->position.posx;
    threadTrainerToMove->positionTo.posy = trainerWithDestiny->position.posy;
    threadTrainerToMove->destinyIsTrainer = true;

    calculateLeaveBlockedFromDeadlock(threadTrainerToMove->idTrainer);
}