#include "tests.h"

run_tests_gameboy(){

    CU_initialize_registry();
    CU_pSuite prueba = CU_add_suite("GAMEBOY Suite",NULL,NULL);
    
    run_tests();
}