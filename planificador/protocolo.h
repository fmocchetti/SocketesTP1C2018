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
  int socket_esi;
  int id_ESI;
  //int cantidadDeLineas;
  float rafaga;
  int lineas_ejecutadas;
  float estimacion_rafaga;
  int espera;
  float prioridad;
}  ESI;
//__attribute__((packed))


typedef struct {
    int id_ESI;
    char claveAEjecutar[40];
} claves;

typedef struct {
  int id;
  int len;
} __attribute__((packed)) ContentHeader;

void * wait_content(int socket);
void _exit_with_error(int socket, char* error_msg, char * buffer);
void _exit_with_error_and_close(int socket, char* error_msg, char * buffer);
void exit_gracefully(int return_nr);

#endif /* PROTOCOLO_H_ */
