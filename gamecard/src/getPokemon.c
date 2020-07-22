#include "getPokemon.h"

void getPokemonTallGrass(threadPokemonMessage* threadGetPokemonMessage){
    get_pokemon* getPokemon = threadGetPokemonMessage->pokemon;
    char buffer[100];   
    char* stream = malloc(getPokemon->sizePokemon + 1);
    memcpy(stream, getPokemon->pokemon, getPokemon->sizePokemon); 
    stream[getPokemon->sizePokemon] = '\0';

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
            list_destroy(listPosiciones);
            listPosiciones = getPositionsPokemon(directorioMetadata, stream);
            cerrarMetadata(directorioMetadata, stream); 
        }
    }

    if(threadGetPokemonMessage->client_fd != 0){
        localized_pokemon* localizedPokemon = malloc(sizeof(localized_pokemon));
        localizedPokemon->pokemon = stream;
        localizedPokemon->sizePokemon = getPokemon->sizePokemon;
        localizedPokemon->positions = listPosiciones; 

        send_localized(localizedPokemon, threadGetPokemonMessage->client_fd, threadGetPokemonMessage->id_mensaje);
        free(localizedPokemon);
    }

    list_destroy_and_destroy_elements(listPosiciones, free);
    free(directory);
    free(stream);
    free(directorioMetadata);
    free(threadGetPokemonMessage->pokemon);
    free(threadGetPokemonMessage);
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

    for(int i = 0; i<cantidadBloques; i++){
        free(bloques[i]);
    }

    list_destroy_and_destroy_elements(lista, free);
    free(bloques);
    config_destroy(configMetadataTallGrass);
    return listPositions;

}

void* structALineaSinCantidad(void* posicion){
    positionQuantity* lineaStruct = (positionQuantity*)posicion;

    t_position* posicionAAgregar = malloc(sizeof(t_position));
    posicionAAgregar->posx = lineaStruct->posicionX;
    posicionAAgregar->posy = lineaStruct->posicionY;


    return (void*)posicionAAgregar;
}