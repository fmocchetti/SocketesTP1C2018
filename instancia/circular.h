/*
 * circular.h
 *
 *  Created on: 26 abr. 2018
 *      Author: Pablo S Monti
 */


#ifndef CIRCULAR_H_
#define CIRCULAR_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include "tabla.h"
#include "ClaveValor.h"
#include <pthread.h>
#include "socket_client.h"
#include "Compactar.h"
#include "ReemplazoEntrada.h"

bool no_hay_lugar(int tamanio,char* l,char* fin);

void cargar_info_en_dato(struct Dato* unDato,char*posicionDeLectura,struct ClaveValor* claveValor);

int SET_circular(int server,char** l,t_list** t,struct ClaveValor* cv,char* ppio,char* fin);

int calcular_cantidad_entradas(int longitudS,int tamEntrada);

int liberar_entradas_atomicas(t_list**tabla,char*  primeraPosicionMemoria,int tamanioEntrada,int cantidadEntradasAOcupar);



#endif /* CIRCULAR_H_ */
