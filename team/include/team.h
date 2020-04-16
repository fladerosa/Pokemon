#ifndef TEAM_H
#define TEAM_H

#include<commons/collections/list.h>
#include "common_utils.h"

typedef struct configuracion{
       char *ip_team;
       uint32_t *puerto_team;
       t_list *posicion_entrenador;
       t_list *pokemon_entrenador;
       t_list *objetivo_entrenador;
       uint32_t  tiempo_reconexion; 
       uint32_t  retardo_ciclo_cpu; 
       char *algoritmo_planificacion;
       uint32_t quantum;
       uint32_t estimacion_inicial;
       char *ip_broker;
       uint32_t *puerto_broker;       

}configuracion;

configuracion valores;
void leer_config();
void crear_logger_obligatorio();
void crear_logger_opcional();
void cargar_valores_config();
void inicializar();
void liberar_recursos();   
void establecerConexionGameBoy();
void establecerConexionBroker();
enum t_algoritmo {fifo, rr, sjf_sd, sjf_cd};

#endif