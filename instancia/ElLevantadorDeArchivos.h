/*
 * ElLevantadorDeArchivos.h
 *
 *  Created on: 11 jul. 2018
 *      Author: PabloSMonti
 */

#ifndef ELLEVANTADORDEARCHIVOS_H_
#define ELLEVANTADORDEARCHIVOS_H_
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <commons/log.h>
#include <unistd.h>
#include <fcntl.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include "socket_client.h"
#include "ClaveValor.h"
#include "circular.h"

char* obtener_valor_de_archivo(char * dir , char* path);

int levantar_archivos_a_memoria(char** storage,int tamanioEntrada,int cantidadEntradas,t_list** tabla,t_dictionary* tablaDeRequeridas,char** posicionDeLectura,char* posicionFinDeMemoria,char* path);

#endif /* ELLEVANTADORDEARCHIVOS_H_ */
