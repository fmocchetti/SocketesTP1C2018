/*
 * Compactar.h
 *
 *  Created on: 24 jun. 2018
 *      Author: PabloSMonti
 */

#ifndef COMPACTAR_H_
#define COMPACTAR_H_
#include "tabla.h"


int compactar(t_list** tabla,char* storage,char** posicionDeLectura,char* posicionFin,int tamEntrada);

bool hay_espacio_fragmentado(t_list** tabla,int tamEntrada,int cantEntradas);

#endif /* COMPACTAR_H_ */
