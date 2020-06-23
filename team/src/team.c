#include "team.h"

int main(int argc, char ** argv){

    initialize_team();
    
    create_threadTrainer_list(trainers);
    planificarProceso();
    
    release_resources();    
    return 0;
}

