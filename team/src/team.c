#include "team.h"

int main(int argc, char ** argv){

    initialize_team();
    establish_broker_connection(); 
    
    //establecer puerto escucha con gameboy como servidor
    listen_to_gameboy();
    
    release_resources();

    return EXIT_SUCCESS;
}

