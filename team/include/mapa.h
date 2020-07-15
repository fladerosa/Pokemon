#ifndef MAPA_H
#define MAPA_H

#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/collections/queue.h>
#include "common_utils.h"
#include "inicializacion.h"

typedef struct {
            t_list* time_cpu_trainer;
            uint32_t total_time_cpu_team;
            uint32_t total_context_switch;

}t_metrics;

typedef struct {
            char* pokemon;
            t_position position;
}t_pokemon_receive;

// se verifica que su estado sea NEW o BLOCKED, si hay mas de uno, el que este mas cerca
//determina cual entrenador de la lista esta mas cerca de la posicion del pokemon localizado, pasada por parametro. 
// calcular los ciclos de cpu a ejecutar el movimiento total (posicionXpokemon - posicionXentrenador, posicionYpokemon - posicionYEntrenador)
// pasa a READY
// generar log obligatorio
bool its_more_closer(t_list*, t_pokemon_receive);

//calcular la distancia al objetivo
uint32_t distance_to_pokemon(t_list*);

//el entrenador se mueve a la posicion pasada por parametro
// cambia a estado EXEC
// realiza el recorrido y consulta si ese entrenador ya atrapo todo lo que necesitaba. si es asi,  cambia a EXIT.
// una vez que finalice, debe retornar el ok para poder seguir pasando entrenador a ese estado..
// generar log obligatorio
void move_to_objetive(t_list*, t_pokemon_receive);
//enviar mensaje caught a broker
//paso trainer a estado EXEC
//calculo el tiempo de cpu hasta ducha posicion

//agrego el pokemon a la lista de pokemon propios
// envio mensaje de captura al broker a la cola // CATCH_POKEMON()
// cambio a estado BLOCKED
// si pasa mas de x tiempo, uso funcion por defecto (CAUGHT_POKEMON OK)dado que quedara bloqueado hasta recibir el ok del broker
//generar log obligatorio
void atrapar_pokemon(t_pokemon, t_list* pokemonOwned);

//esperando_respuesta_catch
// paso trainer a estado BLOCKED
// si pasa x cantidad de tiempo, mando reconexion y uso al funcion por default
// agrego ese poke a la lista de owned
bool get_objetive(t_list*, t_list*);

//verifica luego de ejecutarse el entrenador, si ya se cumplio el objetivo de ese entrenador
// pasa a EXIT
void objetivoLogrado(t_list*owned, t_list*needed);

//compara si los objetivos del entrenador se cumplieron (ambas listas, PokemonNeeded y toCaught, son iguales)
//se encarga de determinar el orden de los procesos(entrenadores) a ready
void planificarProceso();
void planificarProcesoFIFO();
void planificarProcesoRR();
void planificarProcesoSJF_SD();
void planificarProcesoSJF_CD();

#endif