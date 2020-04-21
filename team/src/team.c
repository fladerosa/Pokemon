#include "team.h"

int main(int argc, char ** argv){

    
    valores.pokemon_entrenador= list_create();
    valores.objetivo_entrenador= list_create();

    inicializar();

    //establecer conexion con broker como cliente
    //establecerConexionBroker();
    
    //establecer puerto escucha con gameb()oy como servidor
    //establecerConexionGAmeBoy();
    
    liberar_recursos();

    return EXIT_SUCCESS;
}

