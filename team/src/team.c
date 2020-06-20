#include "team.h"

int main(int argc, char ** argv){

    initialize_team();
    assign_data_trainer();
    
    //connection_broker_global_suscribe(); 
    new_thread_connection_broker_global_suscribe();
    release_resources();
    close_socket();

    return 0;
}

