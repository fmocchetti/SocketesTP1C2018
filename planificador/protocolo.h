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

typedef struct  {
  int id_mensaje;
  int id_ESI;
  int cantidadDeLineas;
} __attribute__((packed)) ESI;

typedef struct {
  int id;
  int len;
} __attribute__((packed)) ContentHeader;

#endif /* PROTOCOLO_H_ */
