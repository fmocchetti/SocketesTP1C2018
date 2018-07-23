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
	struct Dato* unDato;
	//int entradasOcupadasPorValor = 0,entradasIntermedias = 0;
	int entradasLibresContiguas = 0;

	char* principioMemoria = primeraPosicionMemoria;

	char* finDeMemoria = primeraPosicionMemoria + (tamanioEntrada*cantidadEntradas);

	int i = 0,cantidadDeEntradasQueOcupa = 0;

	*punteroEntradaLibre = primeraPosicionMemoria;

	while(i < cantidadEntradas){

		unDato = buscar_dato_por_posicion(*tabla,primeraPosicionMemoria + (tamanioEntrada*i));


		if(unDato==NULL){

			if(cantidadDeEntradasQueOcupa > 0){

				cantidadDeEntradasQueOcupa -= 1;


			}
			else{

				entradasLibresContiguas += 1;

			}


		}
		else{

			entradasLibresContiguas = 0;
			cantidadDeEntradasQueOcupa = calcular_cant_entradas(unDato->cantidadDeBytes,tamanioEntrada)-1;
		}
		if(entradasLibresContiguas==cantidadEntradasAOcupar){

			*punteroEntradaLibre = primeraPosicionMemoria + (tamanioEntrada*(i+1))-(tamanioEntrada*entradasLibresContiguas);
			return true;

		}
		i++;
	}

return false;


















	//ordenar_tabla(tabla,primeraPosicionMemoria);
/*
	for(int i =0;i<tamanio;i++){
		unDato = list_get(*tabla,i);
		char* a = (char*) malloc(unDato->cantidadDeBytes+1);
		memcpy(a,unDato->posicionMemoria,unDato->cantidadDeBytes);
		a[unDato->cantidadDeBytes+1] = '\0';
		log_error(logger,"%s",a);

	}
	if(cantidadEntradasAOcupar == 1){
		return true;
	}

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
*/
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

int compactar(t_list** tabla,char* storage,char** posicionDeLectura,int tamEntrada){

	t_list* tablaAux = copiar(*tabla);



	char* posicionInsercion = storage;


	log_info(logger,"COMPACTAR: Voy a ordenar la tabla");

	ordenar_tabla(&tablaAux,posicionInsercion);

	log_info(logger,"COMPACTAR: -------------------------------------");

	ordenar_tabla(&tablaAux,posicionInsercion);

	int tamanio = list_size(*tabla);

	struct Dato* unDato;

	int espacioAOcupar,entradasAOcupar;

	log_info(logger,"COMPACTAR: compactando storage");

	for(int i = 0 ; i < tamanio ; i++){


		unDato = list_get(tablaAux,i);

		entradasAOcupar = calcular_cantidad_entradas(unDato->cantidadDeBytes,tamEntrada);

		espacioAOcupar = entradasAOcupar * tamEntrada;
/*
		char* a = malloc(unDato->cantidadDeBytes+1);
		a[unDato->cantidadDeBytes]='\0';
		memcpy(a,unDato->posicionMemoria,unDato->cantidadDeBytes);

*/

		if(storage != unDato->posicionMemoria){

			char* a = malloc(unDato->cantidadDeBytes+1);
			memcpy(a,unDato->posicionMemoria,unDato->cantidadDeBytes);
			a[unDato->cantidadDeBytes]='\0';

			log_info(logger,"COMPACTAR: moviendo valor %s",a);

			memmove(posicionInsercion,(const char*)a,unDato->cantidadDeBytes);



			unDato->posicionMemoria = posicionInsercion;
			free(a);

		}







		posicionInsercion += espacioAOcupar;

	//	free(a);

	}

	*posicionDeLectura = posicionInsercion;

	log_info(logger,"COMPACTAR: Se compacto el storage");

	liberar_recursos(&tablaAux);

return 0;
}
