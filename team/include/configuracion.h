#ifndef CONFIGURACION_H
#define CONFIGURACION_H

#include<commons/collections/list.h>
#include<commons/string.h>
#include "common_utils.h"

typedef struct posicion_entrenador{
    uint32_t posix;
    uint32_t posiy;
}posicion_entrenador;

typedef struct pokemon_entrenador{
    uint32_t id_entrenador;
    char * pokemon;   
}pokemon_entrenador;

typedef struct objetivo_entrenador{
    uint32_t id_entrenador;
    char* pokemon;   
}objetivo_entrenador;

typedef struct configuracion{
       char *ip_team;
       char *puerto_team;
       t_list *posicion_entrenador;
       t_list *pokemon_entrenador;
       t_list *objetivo_entrenador;
       uint32_t tiempo_reconexion; 
       uint32_t  retardo_ciclo_cpu; 
       char *algoritmo_planificacion;
       uint32_t quantum;
       uint32_t estimacion_inicial;
       char *ip_broker;
       char *puerto_broker;       

}configuracion;

configuracion valores;
uint32_t socket_team;
on_request respuesta;

void leer_config();
void crear_logger_obligatorio();
void crear_logger_opcional();
void cargar_posiciones_config_team();
void agregar_posicion_a_la_lista(char *);
void imprimir_posicion(char *);
void mostrar(void*);
void cargar_pokemones_config_team();
void agregar_pokemon_a_la_lista(char *);
void imprimir_pokemon(char *);
void cargar_objetivos_config_team();
void agregar_objetivo_a_la_lista(char *);
void imprimir_objetivo(char *);
void cargar_valores_config(t_config*);
void inicializar();
void liberar_recursos();   

void escuchar_gameboy();
void cerrar_servidor();
void establecer_conexion_broker();
enum t_algoritmo {fifo, rr, sjf_sd, sjf_cd};


#endif