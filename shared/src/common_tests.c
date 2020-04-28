#include "common_tests.h"

int run_tests(){
    //CU_initialize_registry(); <- Agregar esto en tu funcion intermedia, antes de agregar tus tests
    CU_pSuite prueba = CU_add_suite("SHARED Suite",NULL,NULL);
    CU_add_test(prueba,"New Pokemon: Serializar & Deserializar",test_new_pokemon);
    CU_add_test(prueba,"Appeared Pokemon: Serializar & Deserializar",test_appeared_pokemon);
    CU_add_test(prueba,"Catch Pokemon: Serializar & Deserializar",test_catch_pokemon);
    CU_add_test(prueba,"Caught Pokemon: Serializar & Deserializar",test_caught_pokemon);
    CU_add_test(prueba,"Get Pokemon: Serializar & Deserializar",test_get_pokemon);
    CU_add_test(prueba,"Localized Pokemon: Serializar & Deserializar",test_localized_pokemon);
    CU_add_test(prueba,"Subscribe: Serializar & Deserializar",test_subscribe);
    CU_add_test(prueba,"New Connection: Serializar & Deserializar",test_new_connection);
    CU_add_test(prueba,"Reconnect: Serializar & Deserializar",test_reconnect);
    CU_add_test(prueba,"Connection: Serializar & Deserializar",test_connection);
    CU_add_test(prueba,"Acknowledgement: Serializar & Deserializar",test_ack);
    CU_add_test(prueba,"Subscribe: Serializar & Deserializar",test_subscribe);

    
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}

void test_new_pokemon(){
    new_pokemon* pokemon = init_new_pokemon("Pikachu",2,3,4,5);
    void* stream = new_pokemon_to_stream(pokemon);
    new_pokemon* copy = stream_to_new_pokemon(stream);

    CU_ASSERT_EQUAL(copy->id_message, pokemon->id_message);
    CU_ASSERT_STRING_EQUAL(copy->pokemon,pokemon->pokemon);
    CU_ASSERT_EQUAL(copy->sizePokemon, pokemon->sizePokemon);
    CU_ASSERT_EQUAL(copy->position.posx, pokemon->position.posx);
    CU_ASSERT_EQUAL(copy->position.posy, pokemon->position.posy);
    CU_ASSERT_EQUAL(copy->quantity, pokemon->quantity);
}

void test_appeared_pokemon(){
    appeared_pokemon* pokemon = init_appeared_pokemon("Bulbasaur", 6,7,8,9);
    void* stream = appeared_pokemon_to_stream(pokemon);
    appeared_pokemon* copy = stream_to_appeared_pokemon(stream);

    CU_ASSERT_EQUAL(copy->id_message, pokemon->id_message);
    CU_ASSERT_STRING_EQUAL(copy->pokemon,pokemon->pokemon);
    CU_ASSERT_EQUAL(copy->sizePokemon, pokemon->sizePokemon);
    CU_ASSERT_EQUAL(copy->position.posx, pokemon->position.posx);
    CU_ASSERT_EQUAL(copy->position.posy, pokemon->position.posy);
    CU_ASSERT_EQUAL(copy->id_correlational, pokemon->id_correlational);
}

void test_catch_pokemon(){
    catch_pokemon* pokemon = init_catch_pokemon("Charmander", 10, 11, 12);
    void* stream = catch_pokemon_to_stream(pokemon);
    catch_pokemon* copy = stream_to_catch_pokemon(stream);

    CU_ASSERT_EQUAL(copy->id_message, pokemon->id_message);
    CU_ASSERT_STRING_EQUAL(copy->pokemon,pokemon->pokemon);
    CU_ASSERT_EQUAL(copy->sizePokemon, pokemon->sizePokemon);
    CU_ASSERT_EQUAL(copy->position.posx, pokemon->position.posx);
    CU_ASSERT_EQUAL(copy->position.posy, pokemon->position.posy);
}

void test_caught_pokemon(){
    caught_pokemon* pokemon = init_caught_pokemon(3,4,true);
    void* stream = caught_pokemon_to_stream(pokemon);
    caught_pokemon* copy = stream_to_caught_pokemon(stream);

    CU_ASSERT_EQUAL(copy->id_message, pokemon->id_message);
    CU_ASSERT_EQUAL(copy->id_correlational, pokemon->id_correlational);
    CU_ASSERT_EQUAL(copy->success, pokemon->success);
}

void test_get_pokemon(){
    get_pokemon* pokemon = init_get_pokemon("Squirtle", 4);
    void* stream = get_pokemon_to_stream(pokemon);
    get_pokemon* copy = stream_to_get_pokemon(stream);

    CU_ASSERT_EQUAL(copy->id_message, pokemon->id_message);
    CU_ASSERT_STRING_EQUAL(copy->pokemon,pokemon->pokemon);
    CU_ASSERT_EQUAL(copy->sizePokemon, pokemon->sizePokemon);
}

void test_localized_pokemon(){
    t_list* list_positions = list_create();
    t_position p1, p2, p3;
    p1.posx = 1;
    p1.posy = 2;
    p2.posx = 3;
    p2.posy = 4;
    p3.posx = 5;
    p3.posy = 6;
    list_add(list_positions, &p1);
    list_add(list_positions, &p2);
    list_add(list_positions, &p3);
    localized_pokemon* pokemon = init_localized_pokemon("Metapod", 4, 5, list_positions);
    void* stream = localized_pokemon_to_stream(pokemon);
    localized_pokemon* copy = stream_to_localized_pokemon(stream);

    CU_ASSERT_STRING_EQUAL(copy->pokemon,pokemon->pokemon);
    CU_ASSERT_EQUAL(copy->sizePokemon, pokemon->sizePokemon);
    CU_ASSERT_EQUAL(copy->id_message, pokemon->id_message);
    CU_ASSERT_EQUAL(copy->id_correlational, pokemon->id_correlational);
    for (int i = 0; i <(*pokemon->positions).elements_count; i++ ){
        t_position* pos_copy = list_get(copy->positions,i),
        *pos_pokemon = list_get(pokemon->positions, i);
        CU_ASSERT_EQUAL(pos_pokemon->posx, pos_copy->posx);
        CU_ASSERT_EQUAL(pos_pokemon->posy, pos_copy->posy);
    }
}

void test_new_connection(){
    new_connection* connection = init_new_connection();
    void* stream = new_connection_to_stream(connection);
    new_connection* copy = stream_to_new_connection(stream);
    free(copy);
    CU_ASSERT_TRUE(true);
}

void test_reconnect(){
    reconnect* recon = init_reconnect(5);
    void* stream = reconnect_to_stream(recon);
    reconnect* copy = stream_to_reconnect(stream);

    CU_ASSERT_EQUAL(copy->id_connection, recon->id_connection);
}

void test_connection(){
    connection* recon = init_connection(5);
    void* stream = connection_to_stream(recon);
    connection* copy = stream_to_connection(stream);

    CU_ASSERT_EQUAL(copy->id_connection, recon->id_connection);
}

void test_ack(){
    ack* acknowledgment = init_ack(5);
    void* stream = ack_to_stream(acknowledgment);
    ack* copy = stream_to_ack(stream);

    CU_ASSERT_EQUAL(copy->id_message, acknowledgment->id_message);
}

void test_subscribe(){
    subscribe* subs = init_subscribe(5);
    void* stream = subscribe_to_stream(subs);
    subscribe* copy = stream_to_subscribe(stream);

    CU_ASSERT_EQUAL(copy->colaMensajes, copy->colaMensajes);
}

