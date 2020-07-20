#include "getPokemon.h"

//void getPokemon(){
    //Verificar si esta el pokemon en el directorio Pokemon. Si no existe se deberá informar el mensaje sin posiciones ni cantidades
    //Verificar si no hay otro proceso que lo esté abriendo. En caso de que si lo haya finalizar hilo y reintentar en "TIEMPO_DE_REINTENTO_OPERACION"
    //Obtener todas las posiciones y cantidades de Pokémon requerido.
    //Cerrar el archivo.
    //Se deberá enviar un mensaje al Broker a la Cola de Mensajes "LOCALIZED_POKEMON" con: ID del mensaje original, el Pokémon solicitado, la lista de posiciones y la cantidad de cada una de ellas en el mapa.
    /**En caso de que no se pueda establecer conexion con el broker notificarlo por Logs y continuar**/
//}

void getPokemonTallGrass(threadPokemonMessage* threadGetPokemonMessage){
    get_pokemon* getPokemon = threadGetPokemonMessage->pokemon;
    char buffer[100];   
    char* stream = malloc(getPokemon->sizePokemon + 1);
    memcpy(stream, getPokemon->pokemon, getPokemon->sizePokemon); 
    stream[getPokemon->sizePokemon] = '\0';

    log_info(optional_logger, "stream creado");

    strcpy(buffer, "");
    strcat(buffer, filesPath);
    strcat(buffer, "/");
    strcat(buffer, stream);

    char* directory = malloc(strlen(filesPath) + getPokemon->sizePokemon + 3);
    memcpy(directory, buffer, strlen(filesPath) + getPokemon->sizePokemon + 2);
    memcpy(directory + strlen(filesPath) + getPokemon->sizePokemon + 2, "\0", sizeof(char));

    char* metadata = "/Metadata.bin";
    char* directorioMetadata = malloc(strlen(directory) + strlen(metadata) + 1);
    strcpy(directorioMetadata,"");
    strcat(directorioMetadata,directory);
    strcat(directorioMetadata, metadata);

    t_list* listPosiciones = list_create();

    if(access( directorioMetadata, F_OK ) != -1){
        FILE* file = fopen(directorioMetadata, "rb");
        flock(fileno(file), LOCK_EX);
    
        fseek(file, 0, SEEK_END);
        int sizeFile = ftell(file);
        fseek(file, 0, SEEK_SET);
    
        flock(fileno(file), LOCK_UN);
        fclose(file);

        if( sizeFile != 0){
            abrirMetadata(directorioMetadata, stream);
            listPosiciones = getPositionsPokemon(directorioMetadata, stream);
            cerrarMetadata(directorioMetadata, stream); 
        }
    }

    localized_pokemon* localizedPokemon = malloc(sizeof(localized_pokemon));
    localizedPokemon->pokemon = getPokemon->pokemon; 
    localizedPokemon->sizePokemon = getPokemon->sizePokemon;
    localizedPokemon->positions = listPosiciones; 

    send_localized(localizedPokemon, threadGetPokemonMessage->client_fd, threadGetPokemonMessage->id_mensaje);

    free(directory);
}

t_list* getPositionsPokemon(char* metadata, char* pokemon){

    pthread_mutex_lock(&metadata_create);
    t_config* configMetadataTallGrass = config_create(metadata);
    pthread_mutex_unlock(&metadata_create);

    char** bloques = config_get_array_value(configMetadataTallGrass,"BLOCKS");
    int size = config_get_int_value(configMetadataTallGrass, "SIZE");
    int cantidadBloques = ceil((float)size / configM.blockSize);

    t_list* lista = levantarBloquesAMemoria(bloques, cantidadBloques);
    t_list* listPositions = list_map(lista, structALineaSinCantidad);

    return listPositions;

}

void* structALineaSinCantidad(void* posicion){
    positionQuantity* lineaStruct = (positionQuantity*)posicion;

    char* posX = malloc(10);
    strcpy(posX,"");
    sprintf(posX,"%d",lineaStruct->posicionX);

    char* posY = malloc(10);
    strcpy(posY,"");
    sprintf(posY,"%d",lineaStruct->posicionY);

    char* writeBinary = malloc(strlen(posX) + strlen("-") + strlen(posY) + 3); 
    strcpy(writeBinary,"");
    strcat(writeBinary,posX);
    strcat(writeBinary,"-");
    strcat(writeBinary,posY);

    free(posX);
    free(posY);

    return (void*)writeBinary;
}