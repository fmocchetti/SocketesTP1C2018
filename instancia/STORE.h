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
#include "socket_client.h"
#include "LRU.h"

int STORE(t_list** tabla,t_list** registro,char* primeraPosicionMemoria,char* clave,char* ruta,int tamanioEntrada);

#endif /* STORE_H_ */
