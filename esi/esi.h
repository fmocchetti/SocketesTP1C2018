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
bool solicitudDeEjecucionPlanificador();
bool envioYRespuestaCoordinador(t_esi_operacion parsed);
void enviarRespuestaAlPlanificador();


#endif  /* ESI_H_ */
