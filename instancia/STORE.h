/*
 * STORE.h
 *
 *  Created on: 7 jun. 2018
 *      Author: Pablo Monti
 */

#ifndef STORE_H_
#define STORE_H_

#include "tabla.h"
#include "Dump.h"
#include <pthread.h>

int STORE(t_list* tabla,char* clave,char* ruta,t_log* logger);

#endif /* STORE_H_ */
