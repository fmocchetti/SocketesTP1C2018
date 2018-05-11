/*
 * esi.h
 *
 *  Created on: 24 abr. 2018
 *      Author: utnso
*/



#ifndef ESI_H_
#define ESI_H_

#include <string.h>
#include <stdlib.h> // Para malloc
#include <sys/socket.h> // Para crear sockets, enviar, recibir, etc
#include <commons/log.h>

t_log * logger;

typedef struct  {
  int id_mensaje;
  int id_ESI;
  int cantidadDeLineas;
} __attribute__((packed)) ESI;

typedef struct {
  int id;
  int len;
} __attribute__((packed)) ContentHeader;



#endif /* ESI_H_ */
