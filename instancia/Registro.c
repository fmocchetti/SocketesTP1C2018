/*
 * Registro.c
 *
 *  Created on: 24 jun. 2018
 *      Author: PabloSMonti
 */

#include "Registro.h"


t_list* crear_registro(int cantEntradas){

	t_list* lista = list_create();

	for(int i = 0; i< cantEntradas; i++){
		struct Registro* registro = (struct Registro*) malloc(sizeof(struct Registro));
		registro->numeroEntrada = i;
		registro->referenciado = 0;
		list_add(lista,registro);
	}

return lista;
}

void* liberar_registros(t_list** registro){

	void liberar_dato(struct Registro* unDato){
		free(unDato);

	}

	list_destroy_and_destroy_elements(*registro,liberar_dato);
return NULL;
}

void* obtener_registro(t_list* registro,int numEntrada){

	bool buscar_registro(struct Registro* reg) {

	     return ((reg->numeroEntrada) == numEntrada);

	}
	struct Registro*  reg = (struct Registro*)list_find(registro,buscar_registro);

return reg;
}

void ordenar_registro(t_list** registro){

	bool menos_accedida_al_comienzo(struct Registro* a,struct Registro* b){

		return(a->referenciado < b->referenciado);
	}
	list_sort(*registro,menos_accedida_al_comienzo);

}
