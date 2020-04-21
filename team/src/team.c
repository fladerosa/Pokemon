#include "team.h"

int main(int argc, char ** argv){

    inicializar();

    //establecer conexion con broker como cliente
    //establecerConexionDeBroker();
    
    //establecer puerto escucha con gameb()oy como servidor
    escucharAGameBoy();
    
    liberar_recursos();

    return EXIT_SUCCESS;
}

