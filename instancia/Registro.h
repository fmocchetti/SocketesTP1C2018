/*
 * Registro.h
 *
 *  Created on: 24 jun. 2018
 *      Author: PabloSMonti
 */

#ifndef REGISTRO_H_
#define REGISTRO_H_
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include "RegistroReferencias.h"

t_list* crear_registro(int cantEntradas);
void* liberar_registros(t_list** registro);
void* obtener_registro(t_list* registro,int numEntrada);
void ordenar_registro(t_list** registro);

#endif /* REGISTRO_H_ */
