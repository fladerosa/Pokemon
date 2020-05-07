#include "iniciarGameCard.h"

void receiveMessage(uint32_t cod_op, uint32_t sizeofstruct, uint32_t client_fd) {
	void* stream = malloc(sizeofstruct);
    recv(client_fd, stream, sizeofstruct, MSG_WAITALL);

    switch(cod_op){
        case NEW_POKEMON:;
            new_pokemon* newPokemonMessage = stream_to_new_pokemon(stream); 
            log_info(optional_logger, "New pokemon!");
            break;
        case CATCH_POKEMON:;
            catch_pokemon* catchPokemonMessage = stream_to_catch_pokemon(stream);
            log_info(optional_logger, "Catch pokemon!");
            break;
        case GET_POKEMON:;
            get_pokemon* getPokemonMessage = stream_to_get_pokemon(stream);
            log_info(optional_logger, "Get pokemon!"); 
            break;
    }
}

void iniciarGameCard(){

    config = config_create("./cfg/gamecard.config");
    uint32_t showConsole = config_get_int_value(config,"LOG_SHOW");
    obligatory_logger = log_create("./cfg/obligatory.log", "obligatory", showConsole, LOG_LEVEL_INFO); 
    optional_logger = log_create("./cfg/optional.log", "optional", showConsole, LOG_LEVEL_INFO);
    char* IP_GAMECARD = config_get_string_value(config,"IP_GAMECARD");
    char* PUERTO_GAMECARD = config_get_string_value(config,"PUERTO_GAMECARD");

    //Se intentara suscribir globalmente al Broker a las siguientes colas de mensajes
    //TODO
    pthread_t hiloClienteBroker;
    pthread_create(&hiloClienteBroker,NULL,(void*)suscribirseATodo,NULL); 

    on_request request = &receiveMessage; 

    start_server(IP_GAMECARD,PUERTO_GAMECARD,request);
    
    uint32_t id_connection = receive_connection_id(socket_broker);
    while(1){
        serve_client(request);
        /*socket_broker = crear_conexion(IP_BROKER, PUERTO_BROKER);
        send_reconnect(id_connection);*/
    }  

    pthread_join(server, NULL);
    pthread_join(hiloClienteBroker,NULL);

    finalizarGameCard();
}

void suscribirseATodo(){
    PUERTO_BROKER = config_get_string_value(config,"PUERTO_BROKER");
    IP_BROKER = config_get_string_value(config,"IP_BROKER");
    socket_broker = crear_conexion(IP_BROKER,PUERTO_BROKER);
    send_new_connection(socket_broker);
    suscribirseA(NEW_POKEMON,socket_broker);
    suscribirseA(CATCH_POKEMON,socket_broker);
    suscribirseA(GET_POKEMON,socket_broker);
}

void finalizarGameCard(){
    log_destroy(obligatory_logger);
    log_destroy(optional_logger);
    config_destroy(config);
    close(socket_broker);
}
