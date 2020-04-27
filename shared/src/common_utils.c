#include "common_utils.h"

new_pokemon* stream_to_new_pokemon(void* stream){

    new_pokemon* newPokemonMessage = malloc(sizeof(new_pokemon)); 
    memcpy(&(newPokemonMessage->sizePokemon), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    newPokemonMessage->pokemon = malloc(newPokemonMessage->sizePokemon);
    memcpy(newPokemonMessage->pokemon, stream, newPokemonMessage->sizePokemon);
    stream += newPokemonMessage->sizePokemon;
    memcpy(&((newPokemonMessage->position).posx), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&((newPokemonMessage->position).posy), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(newPokemonMessage->quantity), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(newPokemonMessage->id_message), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    return newPokemonMessage;
}

void* new_pokemon_to_stream(new_pokemon* newPokemonMessage){

    uint32_t size = sizeof(uint32_t) * 5 + strlen(newPokemonMessage->pokemon) + 1;
    void* stream = malloc(size); 
    uint32_t forward = 0;

    memcpy(stream + forward, &(newPokemonMessage->sizePokemon), sizeof(uint32_t));
    forward += sizeof(uint32_t);    
    memcpy(stream + forward, newPokemonMessage->pokemon, strlen(newPokemonMessage->pokemon) + 1);
    forward += strlen(newPokemonMessage->pokemon) + 1;
    memcpy(stream + forward, &((newPokemonMessage->position).posx), sizeof(uint32_t));
    forward += sizeof(uint32_t);
    memcpy(stream + forward, &((newPokemonMessage->position).posy), sizeof(uint32_t));
    forward += sizeof(uint32_t);
    memcpy(stream + forward, &(newPokemonMessage->quantity), sizeof(uint32_t));
    forward += sizeof(uint32_t);
    memcpy(stream + forward, &(newPokemonMessage->id_message), sizeof(uint32_t));
    forward += sizeof(uint32_t);

    return stream;
}

appeared_pokemon* stream_to_appeared_pokemon(void* stream){

    appeared_pokemon* appearedPokemonMessage = malloc(sizeof(appeared_pokemon)); 

    memcpy(&(appearedPokemonMessage->sizePokemon), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    appearedPokemonMessage->pokemon = malloc(appearedPokemonMessage->sizePokemon);
    memcpy(appearedPokemonMessage->pokemon, stream, appearedPokemonMessage->sizePokemon);
    stream += appearedPokemonMessage->sizePokemon;
    memcpy(&((appearedPokemonMessage->position).posx), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&((appearedPokemonMessage->position).posy), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(appearedPokemonMessage->id_message), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(appearedPokemonMessage->id_correlational), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    return appearedPokemonMessage;
}

void* appeared_pokemon_to_stream(appeared_pokemon* appearedPokemonMessage){

    uint32_t size = sizeof(uint32_t ) * 5 + strlen(appearedPokemonMessage->pokemon) + 1;
    void* stream = malloc(size); 
    uint32_t forward = 0;

    memcpy(stream + forward, &(appearedPokemonMessage->sizePokemon), sizeof(uint32_t));
    forward += sizeof(uint32_t);
    memcpy(stream + forward, appearedPokemonMessage->pokemon, strlen(appearedPokemonMessage->pokemon) + 1);
    forward += strlen(appearedPokemonMessage->pokemon) + 1;
    memcpy(stream + forward, &((appearedPokemonMessage->position).posx), sizeof(uint32_t));
    forward += sizeof(uint32_t);
    memcpy(stream + forward, &((appearedPokemonMessage->position).posy), sizeof(uint32_t));
    forward += sizeof(uint32_t);
    memcpy(stream + forward, &(appearedPokemonMessage->id_message), sizeof(uint32_t));
    forward += sizeof(uint32_t);
    memcpy(stream + forward, &(appearedPokemonMessage->id_correlational), sizeof(uint32_t));
    forward += sizeof(uint32_t);

    return stream;
}

catch_pokemon* stream_to_catch_pokemon(void* stream){

    catch_pokemon* catchPokemonMessage = malloc(sizeof(catch_pokemon)); 

    memcpy(&(catchPokemonMessage->sizePokemon), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    catchPokemonMessage->pokemon = malloc(catchPokemonMessage->sizePokemon);
    memcpy(catchPokemonMessage->pokemon, stream, catchPokemonMessage->sizePokemon);
    stream += catchPokemonMessage->sizePokemon;
    memcpy(&((catchPokemonMessage->position).posx), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&((catchPokemonMessage->position).posy), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(catchPokemonMessage->id_message), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    return catchPokemonMessage;
}

void* catch_pokemon_to_stream(catch_pokemon* catchPokemonMessage){

    uint32_t size = sizeof(uint32_t ) * 4 + strlen(catchPokemonMessage->pokemon) + 1;
    void* stream = malloc(size); 
    uint32_t forward = 0;

    memcpy(stream + forward, &(catchPokemonMessage->sizePokemon), sizeof(uint32_t));
    forward += sizeof(uint32_t);
    memcpy(stream + forward, catchPokemonMessage->pokemon, strlen(catchPokemonMessage->pokemon) + 1);
    forward += strlen(catchPokemonMessage->pokemon) + 1;
    memcpy(stream + forward, &((catchPokemonMessage->position).posx), sizeof(uint32_t));
    forward += sizeof(uint32_t);
    memcpy(stream + forward, &((catchPokemonMessage->position).posy), sizeof(uint32_t));
    forward += sizeof(uint32_t);
    memcpy(stream + forward, &(catchPokemonMessage->id_message), sizeof(uint32_t));
    forward += sizeof(uint32_t);

    return stream;
}

caught_pokemon* stream_to_caught_pokemon(void* stream){

    caught_pokemon* caughtPokemonMessage = malloc(sizeof(caught_pokemon)); 

    memcpy(&(caughtPokemonMessage->id_message), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(caughtPokemonMessage->id_correlational), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(caughtPokemonMessage->success), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    return caughtPokemonMessage;
}

void* caught_pokemon_to_stream(caught_pokemon* caughtPokemonMessage){

    uint32_t size = sizeof(uint32_t) * 3;
    void* stream = malloc(size); 
    uint32_t forward = 0;

    memcpy(stream + forward, &(caughtPokemonMessage->id_message), sizeof(uint32_t));
    forward += sizeof(uint32_t);
    memcpy(stream + forward, &(caughtPokemonMessage->id_correlational), sizeof(uint32_t));
    forward += sizeof(uint32_t);
    memcpy(stream + forward, &(caughtPokemonMessage->success), sizeof(uint32_t));
    forward += sizeof(uint32_t);

    return stream;
}

get_pokemon* stream_to_get_pokemon(void* stream){

    get_pokemon* getPokemonMessage = malloc(sizeof(get_pokemon)); 

    memcpy(&(getPokemonMessage->sizePokemon), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    getPokemonMessage->pokemon = malloc(getPokemonMessage->sizePokemon);
    memcpy(getPokemonMessage->pokemon, stream, getPokemonMessage->sizePokemon);
    stream += getPokemonMessage->sizePokemon;
    memcpy(&(getPokemonMessage->id_message), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    return getPokemonMessage;
}

void* get_pokemon_to_stream(get_pokemon* getPokemonMessage){

    uint32_t size = sizeof(uint32_t ) * 2 + strlen(getPokemonMessage->pokemon) + 1;
    void* stream = malloc(size); 
    uint32_t forward = 0;

    memcpy(stream + forward, &(getPokemonMessage->sizePokemon), sizeof(uint32_t));
    forward += sizeof(uint32_t);
    memcpy(stream + forward, getPokemonMessage->pokemon, strlen(getPokemonMessage->pokemon) + 1);
    forward += strlen(getPokemonMessage->pokemon) + 1;
    memcpy(stream + forward, &(getPokemonMessage->id_message), sizeof(uint32_t));
    forward += sizeof(uint32_t);

    return stream;
}

localized_pokemon* stream_to_localized_pokemon(void* stream){

    localized_pokemon* localizedPokemonMessage = malloc(sizeof(localized_pokemon)); 
    uint32_t sizePositions = 0; 

    memcpy(&(localizedPokemonMessage->sizePokemon), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    localizedPokemonMessage->pokemon = malloc(localizedPokemonMessage->sizePokemon);
    memcpy(localizedPokemonMessage->pokemon, stream, localizedPokemonMessage->sizePokemon);
    stream += localizedPokemonMessage->sizePokemon;
    memcpy(&(localizedPokemonMessage->id_message), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(localizedPokemonMessage->id_correlational), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    memcpy(&(sizePositions), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    
    localizedPokemonMessage->positions = list_create();
    for(int i = 0; i < sizePositions; i++){
        t_position* position = malloc(sizeof(t_position));
        memcpy(&(position->posx), stream, sizeof(uint32_t));
        stream += sizeof(uint32_t);
        memcpy(&(position->posy), stream, sizeof(uint32_t));
        stream += sizeof(uint32_t);
        list_add(localizedPokemonMessage->positions, position);
    }

    return localizedPokemonMessage;
}

void* localized_pokemon_to_stream(localized_pokemon* localizedPokemonMessage){

    uint32_t size = sizeof(uint32_t) * 4 + strlen(localizedPokemonMessage->pokemon) + 1 + sizeof(uint32_t) * 2 * (*localizedPokemonMessage->positions).elements_count;
    void* stream = malloc(size); 
    uint32_t forward = 0;

    memcpy(stream + forward, &(localizedPokemonMessage->sizePokemon), sizeof(uint32_t));
    forward += sizeof(uint32_t);
    memcpy(stream + forward, &(localizedPokemonMessage->pokemon), strlen(localizedPokemonMessage->pokemon) + 1);
    forward += strlen(localizedPokemonMessage->pokemon) + 1;
    forward += sizeof(uint32_t);
    memcpy(stream + forward, &(localizedPokemonMessage->id_message), sizeof(uint32_t));
    forward += sizeof(uint32_t);
    memcpy(stream + forward, &(localizedPokemonMessage->id_correlational), sizeof(uint32_t));
    forward += sizeof(uint32_t);
    memcpy(stream + forward, &((*localizedPokemonMessage->positions).elements_count), sizeof(uint32_t));
    stream += sizeof(uint32_t);

    for(int i = 0; i < (*localizedPokemonMessage->positions).elements_count; i++){
        t_position* position = list_get(localizedPokemonMessage->positions, i);
        memcpy(stream + forward, &(position->posx), sizeof(uint32_t));
        stream += sizeof(uint32_t);
        memcpy(stream + forward, &(position->posy), sizeof(uint32_t)); 
        stream += sizeof(uint32_t);
    }

    return stream;
}

subscribe* stream_to_subscribe(void* stream){

    subscribe* subscribeMessage = malloc(sizeof(subscribe)); 

    memcpy(&(subscribeMessage->colaMensajes), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    return subscribeMessage;
}

void* subscribe_to_stream(subscribe* subscribeMessage){

    uint32_t size = sizeof(uint32_t);
    void* stream = malloc(size); 
    uint32_t forward = 0;

    memcpy(stream + forward, &(subscribeMessage->colaMensajes), sizeof(uint32_t));
    forward += sizeof(uint32_t);

    return stream;
}

new_connection* stream_to_new_connection(void* stream){

    new_connection* newConnectionMessage = malloc(sizeof(new_connection)); 

    return newConnectionMessage;
}

void* new_connection_to_stream(new_connection* newConnectionMessage){

    uint32_t size = 0;
    void* stream = malloc(size); 

    return stream;
}

reconnect* stream_to_reconnect(void* stream){

    reconnect* reconnectMessage = malloc(sizeof(reconnect)); 

    memcpy(&(reconnectMessage->id_connection), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    return reconnectMessage;
}

void* reconnect_to_stream(reconnect* reconnectMessage){

    uint32_t size = 0;
    void* stream = malloc(size);
    uint32_t forward = 0;

    memcpy(stream + forward, &(reconnectMessage->id_connection), sizeof(uint32_t));
    forward += sizeof(uint32_t);

    return stream;
}

connection* stream_to_connection(void* stream){

    connection* connectionMessage = malloc(sizeof(connection)); 

    memcpy(&(connectionMessage->id_connection), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    return connectionMessage;
}

void* connection_to_stream(connection* connectionMessage){

    uint32_t size = 0;
    void* stream = malloc(size);
    uint32_t forward = 0;

    memcpy(stream + forward, &(connectionMessage->id_connection), sizeof(uint32_t));
    forward += sizeof(uint32_t);

    return stream;
}

ack* stream_to_ack(void* stream){

    ack* acknowledgementMessage = malloc(sizeof(ack)); 

    memcpy(&(acknowledgementMessage->id_message), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    return acknowledgementMessage;
}

void* ack_to_stream(ack* acknowledgementMessage){

    uint32_t size = 0;
    void* stream = malloc(size);
    uint32_t forward = 0;

    memcpy(stream + forward, &(acknowledgementMessage->id_message), sizeof(uint32_t));
    forward += sizeof(uint32_t);

    return stream;
}