#include "iniciarGameCard.h"

int iniciarGameCard(){

    config = config_create("./cfg/gamecard.config");
    int showConsole = config_get_int_value(config,"LOG_SHOW");
    obligatory_logger = log_create("./cfg/obligatory.log", "obligatory", showConsole, LOG_LEVEL_INFO); 
    optional_logger = log_create("./cfg/optional.log", "optional", showConsole, LOG_LEVEL_INFO);
    char* IP = config_get_string_value(config,"IP_BROKER");
	char* PUERTO = config_get_string_value(config,"PUERTO_BROKER");
    char* ipGameCard = config_get_string_value(config,"IP_GAMECARD");
    char* puertoGameCard = config_get_string_value(config,"PUERTO_GAMECARD");

    int socket_broker = crear_conexion(IP,PUERTO);
    /*//Se intentara suscribir globalmente al Broker a las siguientes colas de mensajes
    //TODO
    suscribirseA(socket_broker,NEW_POKEMON);
    suscribirseA(socket_broker,CATCH_POKEMON);
    suscribirseA(socket_broker,GET_POKEMON);
    */
    //start_server(ipGameCard, puertoGameCard);
    //finalizarGameCard(socket_broker,logger,config); //TODO
    return 0;
}
