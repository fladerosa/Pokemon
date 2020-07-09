#include "tests.h"


void broker_run_tests(){
    initialize();
    CU_initialize_registry();
    CU_pSuite broker_tests = CU_add_suite("BROKER Suite",NULL,NULL);
    CU_add_test(broker_tests,"Cargar memoria simple y complejo", baseBroker);
    CU_add_test(broker_tests,"Consolidacion Particiones Dinamicas", consolidacionBroker);
    CU_add_test(broker_tests,"Compactación Particiones Dinámicas", compactacionBroker);
    //CU_add_test(broker_tests,"Test manejo a memoria",manejo_memoria);
    run_tests();
}

void initializeMemoryMockup(char* algoritmo_memoria, char* algoritmo_reemplazo, char* algoritmo_particion_libre,
    uint32_t tamano_memoria, uint32_t tamano_minimo_particion, uint32_t frecuencia_compactacion){
    memory.partitions = list_create();
    memory.configuration.memoryAlgorithm = algoritmo_memoria;
    memory.configuration.replaceAlgorithm = algoritmo_reemplazo;
    memory.configuration.freePartitionAlgorithm = algoritmo_particion_libre;
    memory.configuration.size = tamano_memoria;
    memory.configuration.minimunPartitionSize = tamano_minimo_particion;
    memory.configuration.countFailedSearchForCompact = frecuencia_compactacion;    
    memory.failedSearchCount = 0;
    memory.data = malloc(memory.configuration.size);
    memory.m_partitions_modify = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(memory.m_partitions_modify, NULL);
    memory.m_failed_search_modify = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(memory.m_failed_search_modify, NULL );
    t_data* data = malloc(sizeof(t_data));
    data->size = memory.configuration.size;
    data->partition_size = data->size;
    data->offset = 0;
    data->state = FREE;

    list_add(memory.partitions, data);
}


void cargar_memoria_simple(){
    uint32_t id_message = -1;

    get_pokemon* pikachu = init_get_pokemon("Pikachu");
    get_pokemon* charmander = init_get_pokemon("Charmander");

    assign_and_return_message(GET_POKEMON, size_of_get_pokemon(pikachu), get_pokemon_to_stream(pikachu, &id_message));
    assign_and_return_message(GET_POKEMON, size_of_get_pokemon(charmander), get_pokemon_to_stream(charmander, &id_message));
}

void cargar_memoria_complejo(){
    uint32_t id_message = -1;

    catch_pokemon* catchPikachu = init_catch_pokemon("Pikachu", 9, 3);
    catch_pokemon* catchSquirtle = init_catch_pokemon("Squirtle", 9, 3);

    assign_and_return_message(CATCH_POKEMON, size_of_catch_pokemon(catchPikachu), catch_pokemon_to_stream(catchPikachu, &id_message));
    assign_and_return_message(CATCH_POKEMON, size_of_catch_pokemon(catchSquirtle), catch_pokemon_to_stream(catchSquirtle, &id_message));

    caught_pokemon* caughtOk = init_caught_pokemon(true);
    caught_pokemon* caughtFail = init_caught_pokemon(false);

    uint32_t id_corrOk = 10, id_corrFail = 11;
    assign_and_return_message(CAUGHT_POKEMON, size_of_caught_pokemon(caughtOk), caught_pokemon_to_stream(caughtOk, &id_message, &id_corrOk));
    assign_and_return_message(CAUGHT_POKEMON, size_of_caught_pokemon(caughtFail), caught_pokemon_to_stream(caughtFail, &id_message, &id_corrFail));

    catch_pokemon* catchBulbasaur = init_catch_pokemon("Bulbasaur", 9, 3);
    catch_pokemon* catchCharmander = init_catch_pokemon("Charmander", 9, 3);

    assign_and_return_message(CATCH_POKEMON, size_of_catch_pokemon(catchBulbasaur), catch_pokemon_to_stream(catchBulbasaur, &id_message));
    assign_and_return_message(CATCH_POKEMON, size_of_catch_pokemon(catchCharmander), catch_pokemon_to_stream(catchCharmander, &id_message));

    get_pokemon* pichu = init_get_pokemon("Pichu");
    get_pokemon* raichu = init_get_pokemon("Raichu");

    assign_and_return_message(GET_POKEMON, size_of_get_pokemon(pichu), get_pokemon_to_stream(pichu, &id_message));
    assign_and_return_message(GET_POKEMON, size_of_get_pokemon(raichu), get_pokemon_to_stream(raichu, &id_message));
}


void send_all_messagesTEST(uint32_t id_queue){
    pthread_mutex_lock(memory.m_partitions_modify);
    bool isFromQueue(void* elem){ //intellisense no lo reconoce pero compila
        return elem && ((t_data*) elem)->state == USING && ((t_data*) elem)->idQueue == id_queue;
    }
    t_list* queueMessages = list_filter(memory.partitions, isFromQueue);
    void sendMessage(void* data){
        t_data* message = data;
        message->lastTimeUsed = timestamp();
    }
    list_iterate(queueMessages, sendMessage);
    pthread_mutex_unlock(memory.m_partitions_modify);
}

void compactacion_basico(){
    caught_pokemon* caughtOk = init_caught_pokemon(true);
    caught_pokemon* caughtFail = init_caught_pokemon(false);

    uint32_t id_corrOk = 1, id_corrFail = 1;
    assign_and_return_message(CAUGHT_POKEMON, size_of_caught_pokemon(caughtOk), caught_pokemon_to_stream(caughtOk, &id_message, &id_corrOk));
    assign_and_return_message(CAUGHT_POKEMON, size_of_caught_pokemon(caughtFail), caught_pokemon_to_stream(caughtFail, &id_message, &id_corrFail));

    catch_pokemon* catchPikachu = init_catch_pokemon("Pikachu", 2, 3);
    catch_pokemon* catchSquirtle = init_catch_pokemon("Squirtle", 5, 2);

    assign_and_return_message(CATCH_POKEMON, size_of_catch_pokemon(catchPikachu), catch_pokemon_to_stream(catchPikachu, &id_message));
    assign_and_return_message(CATCH_POKEMON, size_of_catch_pokemon(catchSquirtle), catch_pokemon_to_stream(catchSquirtle, &id_message));

    catch_pokemon* catchOnyx = init_catch_pokemon("Onyx", 4, 5);

    assign_and_return_message(CATCH_POKEMON, size_of_catch_pokemon(catchOnyx), catch_pokemon_to_stream(catchOnyx, &id_message));

    send_all_messagesTEST(CAUGHT_POKEMON);

    sleep(2);

    catch_pokemon* catchVaporeon = init_catch_pokemon("Vaporeon", 4, 5);

    assign_and_return_message(CATCH_POKEMON, size_of_catch_pokemon(catchVaporeon), catch_pokemon_to_stream(catchVaporeon, &id_message));
}

void consolidacion_basico(){
    caught_pokemon* caughtOk = init_caught_pokemon(true);
    caught_pokemon* caughtFail = init_caught_pokemon(false);

    uint32_t id_corrOk = 1, id_corrFail = 1;
    assign_and_return_message(CAUGHT_POKEMON, size_of_caught_pokemon(caughtOk), caught_pokemon_to_stream(caughtOk, &id_message, &id_corrOk));
    assign_and_return_message(CAUGHT_POKEMON, size_of_caught_pokemon(caughtFail), caught_pokemon_to_stream(caughtFail, &id_message, &id_corrFail));

    catch_pokemon* catchPikachu = init_catch_pokemon("Pikachu", 2, 3);
    catch_pokemon* catchSquirtle = init_catch_pokemon("Squirtle", 5, 2);

    assign_and_return_message(CATCH_POKEMON, size_of_catch_pokemon(catchPikachu), catch_pokemon_to_stream(catchPikachu, &id_message));
    assign_and_return_message(CATCH_POKEMON, size_of_catch_pokemon(catchSquirtle), catch_pokemon_to_stream(catchSquirtle, &id_message));

    catch_pokemon* catchOnyx = init_catch_pokemon("Onyx", 4, 5);

    assign_and_return_message(CATCH_POKEMON, size_of_catch_pokemon(catchOnyx), catch_pokemon_to_stream(catchOnyx, &id_message));

    send_all_messagesTEST(CAUGHT_POKEMON);

    sleep(2);

    catch_pokemon* catchCharmander = init_catch_pokemon("Charmander", 4, 5);

    assign_and_return_message(CATCH_POKEMON, size_of_catch_pokemon(catchCharmander), catch_pokemon_to_stream(catchCharmander, &id_message));
}

void baseBroker(){
    initializeMemoryMockup("PARTICIONES", "FIFO", "FF", 4096, 4, 10);

    cargar_memoria_simple();

    cargar_memoria_complejo();

    uint32_t expectedValues[] = {0, 11, 25, 44, 64, 68, 72, 93, 115, 124};
    uint32_t lengthArray = sizeof(expectedValues)/sizeof(expectedValues[0]);
    
    for (int i = 0; i<lengthArray; i++){
        t_data* admStruct = list_get(memory.partitions,i);
        CU_ASSERT_EQUAL(admStruct->offset, expectedValues[i]);
    }

    //Segunda parte
    initializeMemoryMockup("PARTICIONES", "FIFO", "FF", 4096, 16, 10);

    cargar_memoria_complejo();

    uint32_t newValues[] = {0, 19, 39, 55, 71, 92, 114, 130};
    uint32_t length = sizeof(newValues)/sizeof(newValues[0]);

    for (int i = 0; i<length; i++){
        t_data* admStruct = list_get(memory.partitions,i);
        CU_ASSERT_EQUAL(admStruct->offset, newValues[i]);
    }

}

void consolidacionBroker(){
    initializeMemoryMockup("PARTICIONES", "FIFO", "FF", 64, 4, 10);

    consolidacion_basico();

    CU_ASSERT_EQUAL(list_size(memory.partitions), 5);
    uint32_t sizes[] = {22, 5, 20, 16, 1};
    uint32_t offsets[] = {0, 22, 27, 47, 63};
    for (int i = 0; i < 5 ; i++){
        t_data* admStruct = list_get(memory.partitions, i);
        CU_ASSERT_EQUAL(admStruct->size, sizes[i]);
        CU_ASSERT_EQUAL(admStruct->offset, offsets[i]);
    }

    // Segunda parte

    initializeMemoryMockup("PARTICIONES", "LRU", "FF", 64, 4, 10);

    consolidacion_basico();

    uint32_t sizesLRU[] = {4, 4, 22, 17, 16, 1};
    uint32_t offsetsLRU[] = {0, 4, 8, 30, 47, 63};
    for (int i = 0; i < 6 ; i++){
        t_data* admStruct = list_get(memory.partitions, i);
        CU_ASSERT_EQUAL(admStruct->size, sizesLRU[i]);
        CU_ASSERT_EQUAL(admStruct->offset, offsetsLRU[i]);
    }
}

void compactacionBroker(){
    initializeMemoryMockup("PARTICIONES", "FIFO", "FF", 64, 4, 1);

    compactacion_basico();

    t_data* admStruct = list_get(memory.partitions, 3);
    CU_ASSERT_EQUAL(admStruct->offset, 36);
}

































void manejo_memoria(){
    initialize();
    initializeMemory();
    CU_ASSERT_TRUE(testSeekAlgorithms());
    initializeMemory();
    CU_ASSERT_TRUE(testAllocateDataBS());
    initializeMemory();
    CU_ASSERT_TRUE(testAllocateDataDP());
    initializeMemory();
    CU_ASSERT_TRUE(testDestroyPartitionFIFO());
    initializeMemory();
    CU_ASSERT_TRUE(testDestroyPartitionLRU());
    initializeMemory();
    CU_ASSERT_TRUE(testCompactDP());
    initializeMemory();
    CU_ASSERT_TRUE(testCompactBS());
    initializeMemory();
    CU_ASSERT_TRUE(testDump());
    /*
    casos de prueba particiones dinamicas
        ubicar dato con memoria vacía
        ubicar dato para verificar First Fit
        ubicar dato para verificar Best Fit
        ubicar dato con memoria llena para verificar compactacion
        ubicar dato con memoria llena para verificar elección de victima FIFO
        ubicar dato con memoria llena para verificar elección de victima LRU

    casos de prueba Buddy System
        ubicar dato con memoria vacía
        ubicar dato para verificar First Fit
        ubicar dato para verificar Best Fit
        ubicar dato con memoria llena para verificar compactacion
        ubicar dato con memoria llena para verificar elección de victima FIFO
        ubicar dato con memoria llena para verificar elección de victima LRU
    */

   //Pruebas Delibird segun documento: 

}

bool testSeekAlgorithms(){
    t_data* data = FF_getPartitionAvailable(512);
    if(data == NULL){
        return false;
    }
    data = BF_getPartitionAvailable(512);
    if(data == NULL){
        return false;
    }
    return true;
}

bool testAllocateDataBS(){
    t_data* data = FF_getPartitionAvailable(512);
    if(data == NULL){
        return false;
    }
    BS_allocateData(128, data);
    if(list_size(memory.partitions) != 5){
        return false;
    }
    return true;
}

bool testAllocateDataDP(){
    t_data* data = FF_getPartitionAvailable(512);
    if(data == NULL){
        return false;
    }
    DP_allocateData(128, data);
    if(list_size(memory.partitions) != 2){
        return false;
    }
    return true;
}

bool testDestroyPartitionFIFO(){
    //Context
    internalStringData(1, "Primer dato");
    internalStringData(2, "Segundo dato");
    if(list_size(memory.partitions) != 3){
        return false;
    }
    FIFO_destroyPartition();
    t_data* firstPartition = (t_data*)list_get(memory.partitions, 0);
    if(firstPartition->state != FREE){
        return false;
    }
    t_data* secondPartition = (t_data*)list_get(memory.partitions, 1);
    if(secondPartition->state == FREE){
        return false;
    }
    void* partitionData = getData(2);
    if(strcmp(partitionData, "Segundo dato") != 0 ){
        return false;
    }
    return true;
}
    
bool testDestroyPartitionLRU(){
    //Context
    internalStringData(1, "Primer dato");
    internalStringData(2, "Segundo dato");
    if(list_size(memory.partitions) != 3){
        return false;
    }

    t_data* firstPartition = (t_data*)list_get(memory.partitions, 0);
    sleep(3);
    void* partitionData = getData(1);

    LRU_destroyPartition();
    firstPartition = (t_data*)list_get(memory.partitions, 0);
    if(firstPartition->state == FREE){
        return false;
    }
    t_data* secondPartition = (t_data*)list_get(memory.partitions, 1);
    if(secondPartition->state != FREE){
        return false;
    }
    partitionData = getData(1);
    if(strcmp(partitionData, "Primer dato") != 0 ){
        return false;
    }
    return true;
}

void internalStringData(uint32_t id, char* text){
    int sizeText = strlen(text)+1;
    void* strData = mallocMemory(id, sizeText);
    memcpy(strData, text, sizeText);
}

bool testCompactDP(){
    //Context
    memory.configuration.memoryAlgorithm = "DP";
    internalStringData(1, "Primer dato");
    internalStringData(2, "Segundo dato");
    internalStringData(3, "Tercer dato");
    if(list_size(memory.partitions) != 4){
        return false;
    }

    sleep(3);
    void* partitionData1 = getData(1);
    void* partitionData2 = getData(3);

    LRU_destroyPartition();
    DP_compact();
    if(list_size(memory.partitions) != 3){
        return false;
    }

    partitionData1 = getData(1);
    partitionData2 = getData(3);
    if(strcmp(partitionData1, "Primer dato") != 0 ){
        return false;
    }
    if(strcmp(partitionData2, "Tercer dato") != 0 ){
        return false;
    }
    return true;
}

bool testCompactBS(){
    t_data* data = FF_getPartitionAvailable(512);
    if(data == NULL){
        return false;
    }
    BS_allocateData(128, data);
    if(list_size(memory.partitions) != 5){
        return false;
    }
    FIFO_destroyPartition();
    BS_compact();
    if(list_size(memory.partitions) != 1){
        return false;
    }
    return true;
}

bool testDump(){
    t_data* data = FF_getPartitionAvailable(512);
    if(data == NULL){
        return false;
    }
    BS_allocateData(128, data);
    if(list_size(memory.partitions) != 5){
        return false;
    }
    dumpMemory();
    return true;
}

