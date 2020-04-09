#include "iniciarGameCard.h"

int iniciarGameCard(){
    //t_logger* logger = iniciar_logger();
    t_config* config = leer_config();
    char* IP = config_get_string_value(config,"IP_BROKER");
	char* PUERTO = config_get_string_value(config,"PUERTO_BROKER");
    int socket_broker = crear_conexion(IP,PUERTO);
    /*//Se intentara suscribir globalmente al Broker a las siguientes colas de mensajes
    //TODO
    suscribirseA(socket_broker,NEW_POKEMON);
    suscribirseA(socket_broker,CATCH_POKEMON);
    suscribirseA(socket_broker,GET_POKEMON);*/
    //finalizarGameCard(socket_broker,logger,config); //TODO
    return 0;
}
