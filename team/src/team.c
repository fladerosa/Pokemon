#include "team.h"

int main(int argc, char ** argv){

    initialize_team();
    connection_broker_global_suscribe(); 

    release_resources();

    return 0;
}

