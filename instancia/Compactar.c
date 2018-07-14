/*
 * Compactar.c
 *
 *  Created on: 24 jun. 2018
 *      Author: PabloSMonti
 */
#include "Compactar.h"



bool hay_espacio_fragmentado(t_list** tabla,int tamEntrada,int cantEntradas){

	int tamanio = list_size(*tabla);
	struct Dato* unDato;
	int entradasNetasNecesarias = 0;
	for(int i=0;i<tamanio;i++){

		unDato = list_get(tabla,i);
		entradasNetasNecesarias += calcular_cant_entradas(unDato->cantidadDeBytes,tamEntrada);

	}

return (cantEntradas != entradasNetasNecesarias);
}

int compactar(t_list** tabla,char* storage,char** posicionDeLectura,char* posicionFin,int tamEntrada){

	char* posicionInsercion = storage;

	ordenar_tabla(tabla,posicionInsercion);

	int tamanio = list_size(*tabla);

	struct Dato* unDato; //= list_get(*tabla,tamanio);

	int espacioAOcupar;

	for(int i=0 ; i<tamanio ; i++){

		unDato = list_get(*tabla,i);

		espacioAOcupar = calcular_cantidad_entradas(unDato->cantidadDeBytes,tamEntrada)*(tamEntrada);

		memcpy(*posicionInsercion,unDato->posicionMemoria,unDato->cantidadDeBytes);

		posicionInsercion += espacioAOcupar;

	}

	*posicionDeLectura = posicionInsercion;

return -1;
}
