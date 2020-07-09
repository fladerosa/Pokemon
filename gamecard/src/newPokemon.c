#include "newPokemon.h"

//void newPokemon(int id_mensaje,char* pokemon,coordenadaPokemon coord, int cantidad){
    //Verificar si esta el pokemon en el directorio Pokemon, si no esta crearlo
    //Verificar si no hay otro proceso que lo esté abriendo. En caso de que si lo haya finalizar hilo y reintentar en "TIEMPO_DE_REINTENTO_OPERACION"
    //Verificar si la coordenada ya existe dentro del archivo. En caso de existir, agregar la cantidad pasada por parámetro a la actual. En caso de no existir se debe agregar al final del archivo una nueva línea indicando la cantidad de Pokémon pasadas.
    //Cerrar el archivo
    //Enviar un mensaje que contenga:ID del mensaje recibido, Pokémon y Coordenada a la cola de mensajes "APPEARED_POKEMON" 
    /**En caso de que no se pueda establecer conexion con el broker notificarlo por Logs y continuar**/
//}

void newPokemonTallGrass(new_pokemon* newPokemon){
    char* directorio = "./TALL_GRASS/Files/";
    char buffer[50];   
    char* stream = malloc(sizeof(new_pokemon));
    memcpy(stream, newPokemon->pokemon, newPokemon->sizePokemon); 
    stream[newPokemon->sizePokemon] = '\0';

    strcpy(buffer, "");
    strcat(buffer, directorio);
    strcat(buffer, stream);

    char* directory = malloc(strlen(directorio) + newPokemon->sizePokemon); 
    memcpy(directory, buffer, strlen(directorio) + newPokemon->sizePokemon);
    directory[strlen(directorio) + newPokemon->sizePokemon] = '\0';

    int created = mkdir(directory, ACCESSPERMS);
    if(created == -1){
        printf("%d",errno);
        printf("No se pudo crear el directorio.");
    }else{
        printf("Directorio creado o abierto.");
    }
    
}