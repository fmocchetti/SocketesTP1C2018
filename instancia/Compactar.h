/*
 * Compactar.h
 *
 *  Created on: 24 jun. 2018
 *      Author: PabloSMonti
 */

#ifndef COMPACTAR_H_
#define COMPACTAR_H_
#include "tabla.h"
#include <stdbool.h>
#include "socket_client.h"
#include "ClaveValor.h"
#include "circular.h"

int compactar(t_list** tabla,char* storage,char** posicionDeLectura,char* posicionFin,int tamEntrada);

bool hay_espacio_fragmentado_para_el_valor(t_list** tabla,struct ClaveValor* cv);

bool son_contiguas(t_list** tabla,struct ClaveValor* cv,int cantidadEntradasAOcupar,char** punteroEntradaLibre,char* primeraPosicionMemoria);

#endif /* COMPACTAR_H_ */
