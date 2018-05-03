/*
 * tabla.c
 *
 *  Created on: 27 abr. 2018
 *      Author: utnso
 */



#include "tabla.h"



void agregar_a_tabla(t_list** tabla,struct Dato* unDato){

	list_add(*tabla,unDato);

}

char* buscar(t_list *tabla,char* claveBuscada){

	bool elemento_buscado(struct Dato* unDato) {

	     return ((unDato->clave) == claveBuscada);

	}
	struct Dato* d = (struct Dato*)list_find(tabla, elemento_buscado);
return d->valor;
}



void registrar_dato_en_tabla(t_list** tabla,struct Dato* unDato,char* clave){

	list_add(*tabla,unDato);

}
