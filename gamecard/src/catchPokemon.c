#include "catchPokemon.h"

void catchPokemonTallGrass(threadPokemonMessage* threadCatchPokemonMessage){
    catch_pokemon* catchPokemon = threadCatchPokemonMessage->pokemon;

    char buffer[100];   
    char* stream = malloc(catchPokemon->sizePokemon + 1);
    memcpy(stream, catchPokemon->pokemon, catchPokemon->sizePokemon); 
    stream[catchPokemon->sizePokemon] = '\0';

    strcpy(buffer, "");
    strcat(buffer, filesPath);
    strcat(buffer, "/");
    strcat(buffer, stream);

    char* directory = malloc(strlen(filesPath) + catchPokemon->sizePokemon + 3);
    memcpy(directory, buffer, strlen(filesPath) + catchPokemon->sizePokemon + 2);
    memcpy(directory + strlen(filesPath) + catchPokemon->sizePokemon + 2, "\0", sizeof(char));

    char* metadata = "/Metadata.bin";
    char* directorioMetadata = malloc(strlen(directory) + strlen(metadata) + 1);
    strcpy(directorioMetadata,"");
    strcat(directorioMetadata,directory);
    strcat(directorioMetadata, metadata);

    caught_pokemon* caughtPokemon = malloc(sizeof(caughtPokemon));

    if(access( directorioMetadata, F_OK ) != -1){
        FILE* file = fopen(directorioMetadata, "rb");
        flock(fileno(file), LOCK_EX);
    
        fseek(file, 0, SEEK_END);
        uint32_t sizeFile = ftell(file);
        fseek(file, 0, SEEK_SET);
    
        flock(fileno(file), LOCK_UN);
        fclose(file);

        if(sizeFile != 0){
            intentarAbrirMetadata(directorioMetadata, stream);
            sacarDatosYOrdenarBloques(directorioMetadata, catchPokemon);
            cerrarMetadata(directorioMetadata, stream); 
            caughtPokemon->success = 1;
        }else{
            log_error(obligatory_logger, "No existen posiciones para ese pokemon (No existe el pokemon).");
            caughtPokemon = 0;
            //hacer fail
        }
    }else{
        caughtPokemon = 0;
    }

    if(threadCatchPokemonMessage->client_fd != 0){
        send_caught(caughtPokemon, threadCatchPokemonMessage->client_fd, threadCatchPokemonMessage->id_mensaje);
    }

    free(caughtPokemon);
    free(stream);
    free(directory);
    free(directorioMetadata);
    free(threadCatchPokemonMessage->pokemon);
    free(catchPokemon->pokemon);
    free(threadCatchPokemonMessage->id_mensaje);
    free(threadCatchPokemonMessage);
}

void sacarDatosYOrdenarBloques(char* metadata, catch_pokemon* catchPokemon){

    char* stream = malloc(catchPokemon->sizePokemon + 1);
    memcpy(stream, catchPokemon->pokemon, catchPokemon->sizePokemon); 
    stream[catchPokemon->sizePokemon] = '\0';

    pthread_mutex_lock(&metadata_create);
    t_config* configMetadataTallGrass = config_create(metadata);
    pthread_mutex_unlock(&metadata_create);

    char** bloques = config_get_array_value(configMetadataTallGrass,"BLOCKS");
    uint32_t size = config_get_int_value(configMetadataTallGrass, "SIZE");
    uint32_t cantidadBloques = ceil((float)size / configM.blockSize);

    char* posX = malloc(10);
    strcpy(posX,"");
    sprintf(posX,"%d",catchPokemon->position.posx);

    char* posY = malloc(10);
    strcpy(posY,"");
    sprintf(posY,"%d",catchPokemon->position.posy);

    char* quantity = malloc(10);
    strcpy(quantity,"");
    sprintf(quantity,"%d",1);

    t_list* lista = levantarBloquesAMemoria(bloques, cantidadBloques);

    positionQuantity* posicionPokemonSacar = malloc(sizeof(positionQuantity));
    posicionPokemonSacar->posicionX = atoi(posX); 
    posicionPokemonSacar->posicionY = atoi(posY);
    posicionPokemonSacar->cantidad = atoi(quantity);

    positionQuantity* posicionEncontrada = list_find_with_args(lista, coincidePosicion, (void*)posicionPokemonSacar);

    if(posicionEncontrada != NULL){
        posicionEncontrada->cantidad = posicionEncontrada->cantidad - posicionPokemonSacar->cantidad;
        if(posicionEncontrada->cantidad == 0){
            for(uint32_t i = 0; i < lista->elements_count; i++){
                positionQuantity* posicionEncontradaSacar = list_get(lista,i); 
                if(coincidePosicion(posicionEncontradaSacar, posicionEncontrada)){
                    void* elem = list_remove(lista, i);
                    free(elem);
                }
            }  
        }
    }

    char* sizeMetadata = bajarBloquesADisco(lista, bloques, cantidadBloques, catchPokemon->pokemon, catchPokemon->position.posx, catchPokemon->position.posy, 1, metadata);

    uint32_t cantidadBloquesUpdated = ceil((float) atoi(sizeMetadata)/configM.blockSize);

    if(cantidadBloquesUpdated < cantidadBloques){
        removeLastBlock(metadata, catchPokemon, posicionPokemonSacar);
    }

    pthread_mutex_lock(&metadata_create);
    t_config* configMetadataUpdated = config_create(metadata);
    pthread_mutex_unlock(&metadata_create);
    config_set_value(configMetadataUpdated, "SIZE", sizeMetadata);

    pthread_mutex_lock(&metadata_create);
    config_save(configMetadataUpdated); 
    pthread_mutex_unlock(&metadata_create);

    list_destroy_and_destroy_elements(lista,free);

    free(posicionPokemonSacar);
    free(posY);
    free(posX);
    free(quantity);
    free(sizeMetadata);
    config_destroy(configMetadataUpdated);
    config_destroy(configMetadataTallGrass);
    free(stream);
}

void removeLastBlock(char* metadata, catch_pokemon* catchPokemon, positionQuantity* posicionPokemonSacar){
    pthread_mutex_lock(&metadata_create);
    t_config* configMetadataTallGrass = config_create(metadata);
    pthread_mutex_unlock(&metadata_create);

    uint32_t size = config_get_int_value(configMetadataTallGrass, "SIZE");
    uint32_t cantidadBloques = ceil((float)size / configM.blockSize);
    char** bloques = config_get_array_value(configMetadataTallGrass,"BLOCKS");

    char* lineaBorrada = structALinea(posicionPokemonSacar);
    uint32_t sizeBloqueBorrado = strlen(lineaBorrada);

    size -= sizeBloqueBorrado; 

    if(size == 1){ //Porque el EOF es 1 byte
        size--;
    }

    if(cantidadBloques > 0){
        uint32_t bloque = atoi(bloques[cantidadBloques - 1]);
        bool testBit = bitarray_test_bit(bitmap, bloque - 1);
        if(testBit == 1){
            bitarray_clean_bit(bitmap, bloque - 1);
        }
    }

    uint32_t cantidadBloquesUpdated = ceil((float)size / configM.blockSize);

    char* bloquesConfig = malloc(sizeof(char)*3*(cantidadBloquesUpdated + 1) + 1);
    strcpy(bloquesConfig,"");
    strcat(bloquesConfig,"[");
    for(uint32_t i = 0; i < cantidadBloquesUpdated; i++){ 
        strcat(bloquesConfig,bloques[i]);
        if(i < cantidadBloquesUpdated - 1){
            strcat(bloquesConfig,",");
        }
    }
    strcat(bloquesConfig,"]");
    
    char* sizeChar = malloc(sizeof(uint32_t));
    strcpy(sizeChar, "");
    sprintf(sizeChar, "%d", size);

    config_set_value(configMetadataTallGrass, "SIZE", sizeChar);
    config_set_value(configMetadataTallGrass,"BLOCKS", bloquesConfig);

    pthread_mutex_lock(&metadata_create);
    config_save(configMetadataTallGrass);
    pthread_mutex_unlock(&metadata_create);

    log_info(obligatory_logger, "Se ha liberado un bloque del pokemon");

    if(size == 0){ 
        log_info(obligatory_logger, "El pokemon fue eliminado");
    }

    config_destroy(configMetadataTallGrass);

    free(bloquesConfig);
    for(uint32_t i = 0; i<cantidadBloques; i++){
        free(bloques[i]);
    }
    free(bloques);
    free(sizeChar);
    free(lineaBorrada);
}