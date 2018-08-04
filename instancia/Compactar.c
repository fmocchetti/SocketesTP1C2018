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
	//int tamanio = list_size(*tabla);
	struct Dato* unDato;
	//int entradasOcupadasPorValor = 0,entradasIntermedias = 0;
	int entradasLibresContiguas = 0;

	//char* principioMemoria = primeraPosicionMemoria;

	//char* finDeMemoria = primeraPosicionMemoria + (tamanioEntrada*cantidadEntradas);

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

int compactar(t_list** tabla,char* storage,char** posicionDeLectura,int tamEntrada,int cantEntradas){

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

			struct Dato dato;

			char* a = malloc(unDato->cantidadDeBytes+1);
			memcpy(a,unDato->posicionMemoria,unDato->cantidadDeBytes);
			a[unDato->cantidadDeBytes]='\0';

			log_info(logger,"COMPACTAR: moviendo valor %s",a);

			memmove(posicionInsercion,(const char*)a,unDato->cantidadDeBytes);
			borrar_un_dato(tabla,unDato);
			dato.clave=unDato->clave;
			dato.cantidadDeBytes=unDato->cantidadDeBytes;
			dato.posicionMemoria = posicionInsercion;
			registrar_dato_en_tabla(tabla,&dato);
			//unDato->posicionMemoria = posicionInsercion;
			//log_error(logger,"COMPACTAR LE ASIGNO %p",posicionInsercion);
			free(a);

		posicionInsercion += espacioAOcupar;
		//log_error(logger,"COMPACTAR punteroLectura %p",posicionInsercion);


	}

	*posicionDeLectura = posicionInsercion;

	log_info(logger,"COMPACTAR: Se compacto el storage");

	log_info(logger,"COMPACTAR: El estado de la tabla es:");

	printf("\n");

	mostrar_tabla(*tabla,cantEntradas,tamEntrada,storage);

	printf("\n");



return 0;
}
