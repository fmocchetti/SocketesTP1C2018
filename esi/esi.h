#ifndef ESI_H_
#define ESI_H_

//#define "socketClient.h"
#include <pthread.h>
#include <parsi/parser.h>
#include <stdlib.h>
#include <stdio.h>
#include <parsi/parser.h>
#include <stdbool.h>


void * planificador();
void * coordinador();
bool solicitudDeEjecucionPlanificador(int socket);
bool envioYRespuestaCoordinador(int socket, t_esi_operacion parsed);
void enviarRespuestaAlPlanificador(int socket, bool respuesta);


#endif  /* ESI_H_ */
