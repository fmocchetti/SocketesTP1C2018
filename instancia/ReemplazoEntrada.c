/*
 * ReemplazoEntrada.c
 *
 *  Created on: 24 jun. 2018
 *      Author: utnso
 */

#ifndef REEMPLAZOENTRADA_C_
#define REEMPLAZOENTRADA_C_
#include "ReemplazoEntrada.h"
#include "circular.h"
#include "tabla.h"


int reemplazar_valor_de_clave_ya_existente(t_list** tabla,t_list** registro,char* primeraPosicionMemoria,struct ClaveValor* claveValor){

	struct Dato* unDato = buscar(*tabla,claveValor->clave);


	int longitudS = strlen(claveValor->valor);
	int cantidadEntradasValorNuevo = calcular_cantidad_entradas(longitudS,claveValor->tamanioEntrada);
	int cantidadEntradasValorExistente = calcular_cantidad_entradas(unDato->cantidadDeBytes,claveValor->tamanioEntrada);

	if(cantidadEntradasValorNuevo <= cantidadEntradasValorExistente){

		if(registro!=NULL){
			registrar_acceso_a_entrada(registro,primeraPosicionMemoria,unDato->posicionMemoria,claveValor->tamanioEntrada,cantidadEntradasValorNuevo);

		}
		unDato->cantidadDeBytes = longitudS;
		log_error(logger, "Size of valor %d %s", longitudS, claveValor->valor);
		memcpy(unDato->posicionMemoria,claveValor->valor,longitudS);

	return 0;
	}
	else{

		//no se puede actualizar el valor de la clave porque ocupa mas espacion que el actual
	return -1;
	}


}


#endif /* REEMPLAZOENTRADA_C_ */
