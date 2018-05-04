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


bool no_hay_lugar(int tamanio,char* l,char* fin);

void cargar_info_en_dato(struct Dato** unDato,char*posicionDeLectura,int longitudS);

void SET_circular(char** l,t_list** t,char* s,char* ppio,char* fin);




#endif /* CIRCULAR_H_ */
