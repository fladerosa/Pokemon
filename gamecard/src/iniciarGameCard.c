#include "iniciarGameCard.h"

void receiveMessage(uint32_t cod_op, uint32_t sizeofstruct, uint32_t client_fd) {
	void* stream = malloc(sizeofstruct);
    if (recv(client_fd, stream, sizeofstruct, MSG_WAITALL)<=0){free(stream); return;}

    switch(cod_op){
        case NEW_POKEMON:;
            new_pokemon* newPokemonMessage = stream_to_new_pokemon(stream,NULL,false); 
            log_info(optional_logger, "New pokemon!");
            break;
        case CATCH_POKEMON:;
            catch_pokemon* catchPokemonMessage = stream_to_catch_pokemon(stream,NULL,false);
            log_info(optional_logger, "Catch pokemon!");
            break;
        case GET_POKEMON:;
            get_pokemon* getPokemonMessage = stream_to_get_pokemon(stream,NULL,false);
            log_info(optional_logger, "Get pokemon!"); 
            break;
        case CONNECTION:;

            connection* connectionMessage = stream_to_connection(stream);
            id_connection = connectionMessage->id_connection;
            suscribirseA(NEW_POKEMON, client_fd);
            suscribirseA(CATCH_POKEMON, client_fd);
            suscribirseA(GET_POKEMON, client_fd);
            log_info(optional_logger, "Connection!"); 
            log_info(optional_logger, "This is the id connection: %d", connectionMessage->id_connection);
            log_info(optional_logger, "Subscribing to queues %d, %d and & %d", NEW_POKEMON, CATCH_POKEMON, GET_POKEMON);
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
    PUERTO_BROKER = config_get_string_value(config,"PUERTO_BROKER");
    IP_BROKER = config_get_string_value(config,"IP_BROKER");

    //Se intentara suscribir globalmente al Broker a las siguientes colas de mensajes
    //TODO
    pthread_t hiloClienteBroker;
    pthread_create(&hiloClienteBroker,NULL,(void*)suscribirseATodo,NULL); 

    start_server(IP_GAMECARD,PUERTO_GAMECARD,request);

    pthread_join(hiloClienteBroker,NULL);

    finalizarGameCard();
}

void suscribirseATodo(){
    socket_broker = crear_conexion(IP_BROKER,PUERTO_BROKER);
    send_new_connection(socket_broker);
    suscribirseA(NEW_POKEMON,socket_broker);
    suscribirseA(CATCH_POKEMON,socket_broker);
    suscribirseA(GET_POKEMON,socket_broker);
    connect_client();
}

void connect_client(){
    on_request request = &receiveMessage; 
    
    t_process_request* process_request = malloc(sizeof(t_process_request)); 
    (*process_request).socket = socket_broker; 
    (*process_request).request_receiver = request;

    uint32_t id_connection = receive_connection_id(socket_broker);
    while(1){
        serve_client(process_request);
        close(socket_broker);
        socket_broker = crear_conexion(IP_BROKER, PUERTO_BROKER);
        send_reconnect(id_connection);
    } 

    pthread_join(server, NULL);
}

void finalizarGameCard(){
    log_destroy(obligatory_logger);
    log_destroy(optional_logger);
    config_destroy(config);
    close(socket_broker);
}
