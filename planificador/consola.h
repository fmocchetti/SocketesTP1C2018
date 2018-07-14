/*
 * consola.h
 *
 *  Created on: 17 abr. 2018
 *      Author: Pablo Delucchi
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_
#include <commons/log.h>
#include <strings.h>
#include "algoritmos.h"

t_log * logger;

void consola();
void configure_logger_consola();
void opciones();
void pausar();
void continuar();
void bloquear();
void desbloquear();
void listar();
void kill();
void status();
void deadlock();
void listar_tomadas(char*);

#endif /* CONSOLA_H_ */
