/*
 * consola.h
 *
 *  Created on: 5 jun. 2018
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_

#define CONSOLA_H_
#define CONSOLA_H_
#include <commons/log.h>
#include <strings.h>

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


#endif /* CONSOLA_H_ */
