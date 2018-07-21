/*
 * BSU.h
 *
 *  Created on: 17 jul. 2018
 *      Author: utnso
 */

#ifndef BSU_H_
#define BSU_H_
#include "tabla.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include "ClaveValor.h"
#include "Dato.h"
#include "circular.h"
#include "socket_client.h"


int SET_BSU(int server,t_list** tabla,char* primeraPosicionMemoria,char** posicionDeLectura,char* posicionFinalMemoria,struct ClaveValor* claveValor);

int liberar_entradas_atomicas_con_valores_mas_grandes(t_list** tabla, char*  primeraPosicionMemoria, int tamanioEntrada,int cantidadEntradasNecesariasLiberar);

#endif /* BSU_H_ */
