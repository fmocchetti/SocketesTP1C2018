#ifndef ESI_H_
#define ESI_H_

//#define "socketClient.h"
#include <pthread.h>
#include <parsi/parser.h>
#include <stdlib.h>
#include <stdio.h>
#include <parsi/parser.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h> // Para crear sockets, enviar, recibir, etc
#include <commons/log.h>


typedef struct  {
  int id_ESI;
  int cantidadDeLineas;
  char *clave;
  char *valor;
  unsigned char operacion;
}  ESI;
//__attribute__((packed))
typedef struct {
  int id;
  int len;
} __attribute__((packed)) ContentHeader;

void * planificador();
void * coordinador();
bool solicitudDeEjecucionPlanificador(int socket);
bool envioYRespuestaCoordinador(int socket, ESI* esi);
void enviarRespuestaAlPlanificador(int socket, bool respuesta);


#endif  /* ESI_H_ */
