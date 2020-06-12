#include "blocked_trainers.h"

t_list* blockes = NULL;

void create_blocked_list(){

    if(!blockes)
    blockes = list_create();
}

void close_blocked_list(){

    if(blockes){
        list_destroy(blockes);
    }
}