/*
 * tabla.h
 *
 *  Created on: 27 abr. 2018
 *      Author: utnso
 */

#ifndef TABLA_H_
#define TABLA_H_

#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/node.h>

struct Dato{

	char* valor;
	char* posicionMemoria;
	unsigned int frecuenciaUso;
	//solo se agrego para el test
	char* clave;
};

void agregar_a_tabla(t_list** tabla,struct Dato* unDato);

char* buscar(t_list *tabla,char* claveBuscada);



void registrar_dato_en_tabla(t_list** tabla,struct Dato* unDato,char* clave);


#endif /* TABLA_H_ */
