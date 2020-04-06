#include "broker.h"


int main(int argc, char ** argv){
    initialize();

    start_server(
        cfg_values.ip_broker, 
        cfg_values.puerto_broker,
        process_request    
    );

    finalize();
    return EXIT_SUCCESS;
}