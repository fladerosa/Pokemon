#include "team.h"
#include "configuracion.h" 

int main(int argc, char ** argv){
    
//crear t_list de las commons que usa la estructura configuracion 
//REVISAR
    t_list *posicion_entrenador = list_create();
    t_list *pokemon_entrenador = list_create();
    t_list *objetivo_entrenador = list_create();
    
    inicializar();

    //establecer conexion con broker como cliente
    establecerConexionBroker();
    
    //establecer puerto escucha con gameboy como servidor
    establecerConexionGAmeBoy();
    
    liberar_recursos();

    return EXIT_SUCCESS;
}

void establecerConexionGameBoy()
{
    start_server(valores.ip_team, valores.puerto_team, p_on_request);
}

void establecerConexionBroker()
{
    int conexion = crear_conexion(valores.ip_broker, valores.puerto_broker);
    enviar_mensaje("Prueba", conexion);
}