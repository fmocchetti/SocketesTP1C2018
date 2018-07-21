/*
 * Compactar.c
 *
 *  Created on: 24 jun. 2018
 *      Author: PabloSMonti
 */
#include "Compactar.h"



bool son_contiguas(t_list** tabla,struct ClaveValor* cv,int cantidadEntradasAOcupar,char** punteroEntradaLibre,char* primeraPosicionMemoria){

	int tamanioEntrada = cv->tamanioEntrada;
	int cantidadEntradas = cv->cantidadEntradas;
	int tamanio = list_size(*tabla);
	struct Dato* unDato,*datoSiguiente;
	int entradasOcupadasPorValor = 0,entradasIntermedias = 0, contadorEntradasContiguas = 0;
	ordenar_tabla(tabla,primeraPosicionMemoria);

	for(int i=0 ; i < (tamanio-1) ; i++ ){

		unDato = list_get(*tabla,i);
		datoSiguiente = list_get(*tabla,i+1);
		entradasOcupadasPorValor = calcular_cant_entradas(unDato->cantidadDeBytes,tamanioEntrada);
		entradasIntermedias = calcular_cant_entradas(datoSiguiente->cantidadDeBytes,tamanioEntrada);
		//el premer dato no esta al comienzo de la memoria, quedo un espacio
		if(i==0 && (unDato->posicionMemoria != primeraPosicionMemoria)){

			entradasOcupadasPorValor = calcular_cant_entradas(unDato->cantidadDeBytes,tamanioEntrada);
			entradasIntermedias = calcular_cant_entradas(datoSiguiente->posicionMemoria - primeraPosicionMemoria,tamanioEntrada);
			if(entradasIntermedias > entradasOcupadasPorValor){

				if(cantidadEntradasAOcupar == (entradasIntermedias - entradasOcupadasPorValor)){

					*punteroEntradaLibre = primeraPosicionMemoria;
					return true;

				}

			}


		}
		else if(i==tamanio-2){
			entradasOcupadasPorValor = calcular_cant_entradas(datoSiguiente->cantidadDeBytes,tamanioEntrada);
			entradasIntermedias = calcular_cant_entradas((primeraPosicionMemoria+(cantidadEntradas*tamanioEntrada)) - datoSiguiente->posicionMemoria,tamanioEntrada);
			if(entradasIntermedias > entradasOcupadasPorValor){

				if(cantidadEntradasAOcupar == (entradasIntermedias - entradasOcupadasPorValor)){

					*punteroEntradaLibre = datoSiguiente->posicionMemoria + (calcular_cant_entradas(datoSiguiente->cantidadDeBytes,tamanioEntrada)*tamanioEntrada);
					return true;

				}

			}
		}
		if(entradasIntermedias > entradasOcupadasPorValor){

			if(cantidadEntradasAOcupar == (entradasIntermedias - entradasOcupadasPorValor)){

				*punteroEntradaLibre = unDato->posicionMemoria + (entradasOcupadasPorValor * tamanioEntrada);
				return true;

			}

		}

	}
return false;
}



bool hay_espacio_fragmentado_para_el_valor(t_list** tabla,struct ClaveValor* cv){

	//me fijo si los datos de la tabla ocupan todas las entradas existentes

	int tamEntrada = cv->tamanioEntrada;
	int cantEntradas = cv->cantidadEntradas;
	int tamanio = list_size(*tabla);
	struct Dato* unDato;
	int entradasNetasNecesarias = 0;
	for(int i=0;i<tamanio;i++){

		unDato = list_get(*tabla,i);
		entradasNetasNecesarias += calcular_cant_entradas(unDato->cantidadDeBytes,tamEntrada);

	}

return (cantEntradas > entradasNetasNecesarias);
}

int compactar(t_list** tabla,char* storage,char** posicionDeLectura,char* posicionFin,int tamEntrada){

	char* posicionInsercion = storage;

	ordenar_tabla(tabla,*posicionInsercion);

	int tamanio = list_size(*tabla);

	struct Dato* unDato;

	int espacioAOcupar,entradasAOcupar;

	log_info(logger,"COMPACTAR: compactando storage");

	for(int i = 0 ; i < tamanio ; i++){


		log_info(logger,".");

		unDato = list_get(*tabla,i);

		entradasAOcupar = calcular_cantidad_entradas(unDato->cantidadDeBytes,tamEntrada);

		espacioAOcupar = entradasAOcupar * tamEntrada;

		memmove(posicionInsercion,(const char*)unDato->posicionMemoria,unDato->cantidadDeBytes);

		unDato->posicionMemoria = *posicionInsercion;

		posicionInsercion += espacioAOcupar;

	}

	*posicionDeLectura = posicionInsercion;

	log_info(logger,"COMPACTAR: Se compacto el storage");

return 0;
}
