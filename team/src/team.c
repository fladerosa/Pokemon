#include "team.h"

int main(int argc, char ** argv){

    inicializar();
    
    //establecer conexion con broker como cliente
    establecer_conexion_broker();
    
    //establecer puerto escucha con gameboy como servidor
    escuchar_gameboy();
    
    liberar_recursos();

    return EXIT_SUCCESS;
}

