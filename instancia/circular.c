#include "circular.h"

/*
 * circular.c
 *
 *  Created on: 26 abr. 2018
 *      Author: Pablo S Monti
 */
// chequeo si el espacio libre es menor al tamaño requerido
bool no_hay_lugar(int tamanio,char* posicionDeLectura,char* posicionFinalMemoria){

return ((posicionFinalMemoria - posicionDeLectura)<tamanio);
}

void cargar_info_en_dato(struct Dato** unDato,char* posicionDeLectura,struct ClaveValor* claveValor){

	int longitudS = strlen(claveValor->valor);
	(*unDato)->posicionMemoria = posicionDeLectura;
	(*unDato)->cantidadDeBytes = longitudS;
	//*((*unDato)->clave) = *(claveValor->clave);
	strcpy((*unDato)->clave,claveValor->clave);



}

//ingreso un valor en memoria con logica circular y registro en la tabla de entradas dicha insercion
void SET_circular(char** posicionDeLectura,t_list** tabla,struct ClaveValor* claveValor,char* primeraPosicionMemoria,char* posicionFinalMemoria){

	struct Dato* unDato;

	int longitudS = strlen(claveValor->valor);
	//si se termino la memoria vuelvo al principio
	if(*posicionDeLectura==posicionFinalMemoria){

		*posicionDeLectura=primeraPosicionMemoria;

	}
	//si no hay lugar para todo el string lo parto y coloco lo que entra y el resto al principio
	//de la memoria
	if( no_hay_lugar( longitudS, *posicionDeLectura, posicionFinalMemoria) ){

		int espacioRestante = posicionFinalMemoria-*posicionDeLectura;

		memcpy(*posicionDeLectura,claveValor->valor,espacioRestante);

		cargar_info_en_dato(&unDato,*posicionDeLectura,claveValor);

		registrar_dato_en_tabla(tabla,unDato);

		*posicionDeLectura=primeraPosicionMemoria;

		memcpy(*posicionDeLectura,(claveValor->valor+(espacioRestante)),longitudS-espacioRestante);

		*posicionDeLectura += (longitudS-espacioRestante);

		return;

	}
	//guardo el dato entero en memoria si no entro en los if anteriores
	memcpy(*posicionDeLectura,claveValor->valor,longitudS);

	cargar_info_en_dato(&unDato,*posicionDeLectura/*,longitudS*/,claveValor);
	registrar_dato_en_tabla(tabla,unDato);

	*posicionDeLectura += longitudS;

}

