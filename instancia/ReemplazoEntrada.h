/*
 * ReemplazoEntrada.h
 *
 *  Created on: 24 jun. 2018
 *      Author: utnso
 */

#ifndef REEMPLAZOENTRADA_H_
#define REEMPLAZOENTRADA_H_
#include <commons/collections/list.h>
#include "ClaveValor.h"
#include "LRU.h"
int reemplazar_valor_de_clave_ya_existente(t_list** tabla,t_list** registro,char* primeraPosicionMemoria,struct ClaveValor* claveValor);

#endif /* REEMPLAZOENTRADA_H_ */
