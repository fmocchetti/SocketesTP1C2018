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
  int socket_esi;
  int id_mensaje;
  int id_ESI;
  int cantidadDeLineas;
  char claveAEjecutar[40];
}  ESI;
//__attribute__((packed))
typedef struct {
  int id;
  int len;
} __attribute__((packed)) ContentHeader;



#endif /* ESI_H_ */
