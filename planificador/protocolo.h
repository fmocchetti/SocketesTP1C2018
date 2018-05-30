/*
 * protocolo.h
 *
 *  Created on: 11 may. 2018
 *      Author: utnso
 */

#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

#include <string.h>
#include <stdlib.h> // Para malloc
#include <sys/socket.h> // Para crear sockets, enviar, recibir, etc
#include <commons/log.h>
#include "algoritmos.h"

typedef struct  {
  int socket_esi;
  int id_ESI;
  int cantidadDeLineas;
}  ESI;
//__attribute__((packed))


typedef struct {
    int id_ESI;
    char claveAEjecutar[40];
}claves;

typedef struct {
  int id;
  int len;
} __attribute__((packed)) ContentHeader;

#endif /* PROTOCOLO_H_ */
