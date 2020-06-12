#ifndef MAPA_H
#define MAPA_H

#include<commons/collections/list.h>
#include<commons/string.h>
#include "common_utils.h"
#include "inicializacion.h"

/* map sera  matriz dinamica para ubicacion de entrenadores y pokemon
   *fil como *col del mapa, permitiran el desplazamiento para la ubicacion y planificacion
*/
typedef struct{
    uint32_t **map;  
    uint32_t *fil;
    uint32_t *col; 
}position_map;

enum t_algoritmo {fifo, rr, sjf_sd, sjf_cd};
#endif