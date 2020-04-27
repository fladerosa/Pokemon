#include "getPokemon.h"

void getPokemon(){
    //Verificar si esta el pokemon en el directorio Pokemon. Si no existe se deberá informar el mensaje sin posiciones ni cantidades
    //Verificar si no hay otro proceso que lo esté abriendo. En caso de que si lo haya finalizar hilo y reintentar en "TIEMPO_DE_REINTENTO_OPERACION"
    //Obtener todas las posiciones y cantidades de Pokémon requerido.
    //Cerrar el archivo.
    //Se deberá enviar un mensaje al Broker a la Cola de Mensajes "LOCALIZED_POKEMON" con: ID del mensaje original, el Pokémon solicitado, la lista de posiciones y la cantidad de cada una de ellas en el mapa.
    /**En caso de que no se pueda establecer conexion con el broker notificarlo por Logs y continuar**/
}