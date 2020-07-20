#include "newPokemon.h"

//void newPokemon(int id_mensaje,char* pokemon,coordenadaPokemon coord, int cantidad){
    //Verificar si esta el pokemon en el directorio Pokemon, si no esta crearlo
    //Verificar si no hay otro proceso que lo esté abriendo. En caso de que si lo haya finalizar hilo y reintentar en "TIEMPO_DE_REINTENTO_OPERACION"
    //Verificar si la coordenada ya existe dentro del archivo. En caso de existir, agregar la cantidad pasada por parámetro a la actual. En caso de no existir se debe agregar al final del archivo una nueva línea indicando la cantidad de Pokémon pasadas.
    //Cerrar el archivo
    //Enviar un mensaje que contenga:ID del mensaje recibido, Pokémon y Coordenada a la cola de mensajes "APPEARED_POKEMON" 
    /**En caso de que no se pueda establecer conexion con el broker notificarlo por Logs y continuar**/
//}

void newPokemonTallGrass(threadPokemonMessage* threadPokemonMessage){
    new_pokemon* newPokemon = threadPokemonMessage->pokemon;
    char buffer[100];   
    char* stream = malloc(newPokemon->sizePokemon + 1);
    memcpy(stream, newPokemon->pokemon, newPokemon->sizePokemon); 
    stream[newPokemon->sizePokemon] = '\0';

    strcpy(buffer, "");
    strcat(buffer, filesPath);
    strcat(buffer, "/");
    strcat(buffer, stream);

    char* directory = malloc(strlen(filesPath) + newPokemon->sizePokemon + 3);
    memcpy(directory, buffer, strlen(filesPath) + newPokemon->sizePokemon + 2);
    memcpy(directory + strlen(filesPath) + newPokemon->sizePokemon + 2, "\0", sizeof(char));

    int created = mkdir(directory, ACCESSPERMS);
    
    if(created != -1){
        pthread_mutex_lock(&mutexListOfMutex);
        mutexDirectory* mutex = malloc(sizeof(mutexDirectory));
        mutex->nombreDirectorio = malloc(strlen(stream) + 1);
        strcpy(mutex->nombreDirectorio, "");
        strcat(mutex->nombreDirectorio, stream);
        strcat(mutex->nombreDirectorio, "\0");
        pthread_mutex_init(&mutex->mutex, NULL);
        list_add(mutexListDirectory, mutex); //Asi solo tiene el nombre del pokemon 
        pthread_mutex_unlock(&mutexListOfMutex);
    }

    createMetadataPokemon(directory, newPokemon);

    free(directory);
    free(stream);
    //free(newPokemon->pokemon);
    //free_new_pokemon(newPokemon);

    appeared_pokemon* appearedPokemon = malloc(sizeof(appeared_pokemon));
    appearedPokemon->sizePokemon = newPokemon->sizePokemon;
    appearedPokemon->pokemon = newPokemon->pokemon;
    appearedPokemon->position.posx = newPokemon->position.posx;
    appearedPokemon->position.posy = newPokemon->position.posy;
    //void* appearedStream = appeared_pokemon_to_stream(appearedPokemon, -1, threadPokemonMessage->id_mensaje);
    send_appeared(appearedPokemon, threadPokemonMessage->client_fd, threadPokemonMessage->id_mensaje);

    free(appearedPokemon);
}

void createMetadataPokemon(char* directory, new_pokemon* newPokemon){
    char* metadata = "/Metadata.bin";
    char* directorioMetadata = malloc(strlen(directory) + strlen(metadata) + 1);
    strcpy(directorioMetadata,"");
    strcat(directorioMetadata,directory);
    strcat(directorioMetadata, metadata);

    FILE* file = fopen(directorioMetadata,"ab+");
    flock(fileno(file), LOCK_EX);
    
    fseek(file, 0, SEEK_END);
    int sizeFile = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    flock(fileno(file), LOCK_UN);
    fclose(file);

    char* stream = malloc(newPokemon->sizePokemon + 1);
    memcpy(stream, newPokemon->pokemon, newPokemon->sizePokemon); 
    stream[newPokemon->sizePokemon] = '\0';

    if(sizeFile == 0){
        configMetadataCreate(directorioMetadata);
    }

    intentarAbrirMetadata(directorioMetadata, stream);
    int cantidadBloques = metadataBlocks(directorioMetadata, stream);

    
    if(cantidadBloques == 0){
        log_info(optional_logger, "creando mi primer bloque");
        char* block = crearBloque(newPokemon);
        addBlockMetadata(directorioMetadata, block, newPokemon);
        
        free(block);
        log_info(optional_logger, "ya sali de crear mi primer bloque");
    }else{
        agregarDatosYOrdenarBloques(directorioMetadata, newPokemon);
    }
    
    cerrarMetadata(directorioMetadata, stream);
    log_info(optional_logger, "se cerro el metadata todo ok");
    free(stream);
    free(directorioMetadata);
    log_info(optional_logger, "free crear metadata ok");
}


void configMetadataCreate(char* metadata){
    
    t_config* configMetadataTallGrass = config_create("./cfg/tall_grass_metadata.config");

    pthread_mutex_lock(&metadata_create);
    config_save_in_file(configMetadataTallGrass, metadata);
    pthread_mutex_unlock(&metadata_create);

    config_destroy(configMetadataTallGrass);
}

void agregarDatosYOrdenarBloques(char* metadata, new_pokemon* newPokemon){
    char* stream = malloc(newPokemon->sizePokemon + 1);
    memcpy(stream, newPokemon->pokemon, newPokemon->sizePokemon); 
    stream[newPokemon->sizePokemon] = '\0';

    pthread_mutex_lock(&metadata_create);
    t_config* configMetadataTallGrass = config_create(metadata);
    pthread_mutex_unlock(&metadata_create);

    char** bloques = config_get_array_value(configMetadataTallGrass,"BLOCKS");
    int size = config_get_int_value(configMetadataTallGrass, "SIZE");
    int cantidadBloques = ceil((float)size / configM.blockSize);

    config_destroy(configMetadataTallGrass);
    
    char* ultimoBloque = bloques[cantidadBloques-1]; 
    char* extension = ".bin";

    char* bloque = malloc(strlen(blocksPath) + sizeof(char) + strlen(extension) + 2);
    strcpy(bloque,"");
    strcat(bloque, blocksPath);
    strcat(bloque, "/"); 
    strcat(bloque, ultimoBloque); 
    strcat(bloque, extension);

    FILE* file = fopen(bloque,"ab+");

    fseek(file, 0, SEEK_END);
    int sizeFile = ftell(file);
    fseek(file,0,SEEK_SET);

    if(sizeFile >= configM.blockSize){
        log_info(optional_logger, "agregar datos y bloque");
        char* block = crearBloque(newPokemon);
        addBlockMetadata(metadata, block, newPokemon);
        
        free(block);
    }else{

        char* posX = malloc(10);
        strcpy(posX,"");
        sprintf(posX,"%d",newPokemon->position.posx);//(char); 

        char* posY = malloc(10);
        strcpy(posY,"");
        sprintf(posY,"%d",newPokemon->position.posy);//(char); 

        char* quantity = malloc(10);
        strcpy(quantity,"");
        sprintf(quantity,"%d",newPokemon->quantity);//(char); 

        t_list* lista = levantarBloquesAMemoria(bloques, cantidadBloques);

        positionQuantity* posicionNewPokemon = malloc(sizeof(positionQuantity));
        posicionNewPokemon->posicionX = atoi(posX); 
        posicionNewPokemon->posicionY = atoi(posY);
        posicionNewPokemon->cantidad = atoi(quantity);

        positionQuantity* posicionEncontrada = list_find_with_args(lista, coincidePosicion, (void*)posicionNewPokemon);

        if(posicionEncontrada != NULL){
            posicionEncontrada->cantidad = posicionEncontrada->cantidad + posicionNewPokemon->cantidad;
            free(posicionNewPokemon);
        }else{
            list_add(lista, posicionNewPokemon);
        }

        char* sizeMetadata = bajarBloquesADisco(lista, bloques, cantidadBloques, newPokemon->pokemon, newPokemon->position.posx, newPokemon->position.posy, newPokemon->quantity, metadata);

        pthread_mutex_lock(&metadata_create);
        t_config* configMetadataUpdated = config_create(metadata);
        pthread_mutex_unlock(&metadata_create);
        config_set_value(configMetadataUpdated, "SIZE", sizeMetadata);

        pthread_mutex_lock(&metadata_create);
        config_save(configMetadataUpdated); 
        pthread_mutex_unlock(&metadata_create);

        list_destroy_and_destroy_elements(lista,free);

        free(posY);
        free(posX);
        free(quantity);
        free(sizeMetadata);
        config_destroy(configMetadataUpdated);
    }
    
    fclose(file);
    free(bloque);
    //free(bloques);
    for(int i = 0; i<cantidadBloques; i++){
        free(bloques[i]);
    }
    free(bloques);
    free(stream);
}