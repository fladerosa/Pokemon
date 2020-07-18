#include "iniciarGameCard.h"

void receiveMessage(uint32_t cod_op, uint32_t sizeofstruct, uint32_t client_fd) {
	void* stream = malloc(sizeofstruct);
    uint32_t* id_message = malloc(sizeof(uint32_t));
    *id_message = 0; //revisar 
    if (recv(client_fd, stream, sizeofstruct, MSG_WAITALL)<=0){free(stream); return;}

    switch(cod_op){
        case NEW_POKEMON:;
            new_pokemon* newPokemonMessage = stream_to_new_pokemon(stream,id_message,false); 
            log_info(optional_logger, "New pokemon!");
            
            send_ack(client_fd, *id_message);

            pthread_create_and_detach(newPokemonTallGrass, newPokemonMessage);
            //newPokemonTallGrass(newPokemonMessage);
            //TODO SEND APPEARED
            break;
        case CATCH_POKEMON:;
            catch_pokemon* catchPokemonMessage = stream_to_catch_pokemon(stream,id_message,false);
            log_info(optional_logger, "Catch pokemon!");

            send_ack(client_fd, *id_message);
            //TODO SEND CAUGHT
            
            free_catch_pokemon(catchPokemonMessage);
            break;
        case GET_POKEMON:;
            get_pokemon* getPokemonMessage = stream_to_get_pokemon(stream,id_message,false);
            log_info(optional_logger, "Get pokemon!"); 

            send_ack(client_fd, *id_message);
            //TODO SEND LOCALIZED
            
            free_get_pokemon(getPokemonMessage);
            break;
        case CONNECTION:;
            connection* connectionMessage = stream_to_connection(stream);

            threadSubscribe* thread = list_find_with_args(threadSubscribeList, compareSockets, (void*)client_fd);
            thread->idConnection = connectionMessage->id_connection;

            suscribirseA(thread->idCola, client_fd);

            log_info(optional_logger, "Connection!"); 
            log_info(optional_logger, "This is the id connection: %d", connectionMessage->id_connection);
            log_info(optional_logger, "Subscribing to queues %d, %d and & %d", NEW_POKEMON, CATCH_POKEMON, GET_POKEMON);
            break;
        
    }

    free(stream);
    free(id_message);
}

bool compareSockets(void* element, void* args){
    
    threadSubscribe* thread = (threadSubscribe*) element;
    
    return thread->socket == (uint32_t) args;
}

void iniciarMutex(){
    pthread_mutex_init(&mutexListOfMutex, NULL);
    mutexListDirectory = list_create();

    DIR* dirp; 
    struct dirent *direntp; 

    dirp = opendir("./TALL_GRASS/Files");
    
    while((direntp = readdir(dirp)) != NULL){
        if(direntp->d_type == DT_DIR){
            mutexDirectory* mutexDirectoryRead = malloc(sizeof(mutexDirectory));

            char* directory = malloc(strlen(direntp->d_name) + 1);
            memcpy(directory, direntp->d_name, strlen(direntp->d_name)); 
            directory[strlen(direntp->d_name) + 1] = '\0';

            mutexDirectoryRead->nombreDirectorio = directory;
            pthread_mutex_init(&mutexDirectoryRead->mutex, NULL);
            
            pthread_mutex_lock(&mutexListOfMutex); 
            list_add(mutexListDirectory, mutexDirectoryRead);
            pthread_mutex_unlock(&mutexListOfMutex);
        }
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
    
    iniciarMutex();


    request = &receiveMessage; 
    iniciarTallGrass();
    //Se intentara suscribir globalmente al Broker a las siguientes colas de mensajes
    //TODO
    pthread_mutex_init(&mutexthreadSubscribeList, NULL);
    
    threadSubscribeList = list_create();


    suscribirseATodo();

    start_server(IP_GAMECARD,PUERTO_GAMECARD,request);

    pthread_join(suscripcionNewPokemon,NULL);
    pthread_join(suscripcionCatchPokemon,NULL);
    pthread_join(suscripcionGetPokemon,NULL);

    finalizarGameCard();
}

void suscribirseATodo(){ 

    uint32_t socket_new_pokemon = crear_conexion(IP_BROKER,PUERTO_BROKER);
    crearSuscripcion(socket_new_pokemon, NEW_POKEMON, &suscripcionNewPokemon);
    
    //uint32_t socket_catch_pokemon = crear_conexion(IP_BROKER,PUERTO_BROKER);
    //crearSuscripcion(socket_catch_pokemon, CATCH_POKEMON, &suscripcionCatchPokemon);

    //uint32_t socket_get_pokemon = crear_conexion(IP_BROKER,PUERTO_BROKER);
    //crearSuscripcion(socket_get_pokemon, GET_POKEMON, &suscripcionGetPokemon);
}

void crearSuscripcion(uint32_t socket,op_code codeOperation, pthread_t* threadName){
    
    send_new_connection(socket);
    args_pthread* arguments = malloc(sizeof(args_pthread)); 
    arguments->codigoCola = codeOperation;
    arguments->socket = malloc(sizeof(uint32_t));
    *arguments->socket = socket;

    pthread_create(threadName,NULL,(void*)subscribeAndConnect, arguments);
   
}

void subscribeAndConnect(args_pthread* arguments){
    uint32_t id_connection = receive_connection_id(*arguments->socket);
    suscribirseA(arguments->codigoCola,*arguments->socket);

    switch(arguments->codigoCola){
        case NEW_POKEMON:
            structNewPokemon = malloc(sizeof(threadSubscribe));
            structNewPokemon->idCola = NEW_POKEMON;
            structNewPokemon->socket = *arguments->socket;
            structNewPokemon->idConnection = id_connection;

            pthread_mutex_lock(&mutexthreadSubscribeList);
            list_add(threadSubscribeList, structNewPokemon);
            pthread_mutex_unlock(&mutexthreadSubscribeList);
            
            connect_client(*arguments->socket, NEW_POKEMON);
            break;
        case CATCH_POKEMON:
            structCatchPokemon = malloc(sizeof(threadSubscribe));
            structCatchPokemon->idCola = CATCH_POKEMON;
            structCatchPokemon->socket = *arguments->socket;
            structCatchPokemon->idConnection = id_connection;

            pthread_mutex_lock(&mutexthreadSubscribeList);
            list_add(threadSubscribeList, structCatchPokemon);
            pthread_mutex_unlock(&mutexthreadSubscribeList);

            connect_client(*arguments->socket, CATCH_POKEMON);
            break;
        case GET_POKEMON:
            structGetPokemon = malloc(sizeof(threadSubscribe));
            structGetPokemon->idCola = GET_POKEMON;
            structGetPokemon->socket = *arguments->socket;
            structGetPokemon->idConnection = id_connection;

            pthread_mutex_lock(&mutexthreadSubscribeList);
            list_add(threadSubscribeList, structGetPokemon);
            pthread_mutex_unlock(&mutexthreadSubscribeList);

            connect_client(*arguments->socket, GET_POKEMON);
            break;
        default:
            break;
    }
    
}

void connect_client(uint32_t socket,op_code codeOperation){
    
    t_process_request* process_request = malloc(sizeof(t_process_request)); 
    (*process_request).socket = malloc(sizeof(uint32_t));
    *(*process_request).socket = socket; 
    (*process_request).request_receiver = request;
    
    threadSubscribe* thread = list_find_with_args(threadSubscribeList, compareSockets, (void*)socket);

    while(1){
        serve_client(process_request);
        close(socket);
        socket = crear_conexion(IP_BROKER, PUERTO_BROKER);
        process_request = malloc(sizeof(t_process_request)); 
        (*process_request).socket = malloc(sizeof(uint32_t));
        *(*process_request).socket = socket; 
        (*process_request).request_receiver = request;
        send_reconnect(socket, thread->idConnection);
    } 

    pthread_join(server, NULL);
}

void finalizarGameCard(){
    log_destroy(obligatory_logger);
    log_destroy(optional_logger);
    config_destroy(config);
    close(socket_broker);
}
