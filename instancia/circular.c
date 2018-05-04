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

void cargar_info_en_dato(struct Dato** unDato,char* posicionDeLectura,int longitudS){

	(*unDato)->posicionMemoria = posicionDeLectura;
	(*unDato)->cantidadDeBytes = longitudS;


}

//ingreso un valor en memoria con logica circular y registro en la tabla de entradas dicha insercion
void SET_circular(char** posicionDeLectura,t_list** tabla,char* stringAGuardar,char* primeraPosicionMemoria,char* posicionFinalMemoria){

	struct Dato* unDato;

	int longitudS = strlen(stringAGuardar);
	//si se termino la memoria vuelvo al principio
	if(*posicionDeLectura==posicionFinalMemoria){

		*posicionDeLectura=primeraPosicionMemoria;

	}
	//si no hay lugar para todo el string lo parto y coloco lo que entra y el resto al principio
	//de la memoria
	if( no_hay_lugar( longitudS, *posicionDeLectura, posicionFinalMemoria) ){

		int espacioRestante = posicionFinalMemoria-*posicionDeLectura;

		memcpy(*posicionDeLectura,stringAGuardar,espacioRestante);

		cargar_info_en_dato(&unDato,*posicionDeLectura,longitudS);

		registrar_dato_en_tabla(tabla,unDato);

		*posicionDeLectura=primeraPosicionMemoria;

		memcpy(*posicionDeLectura,(stringAGuardar+(espacioRestante)),longitudS-espacioRestante);

		*posicionDeLectura += (longitudS-espacioRestante);

		return;

	}
	//guardo el dato entero en memoria si no entro en los if anteriores
	memcpy(*posicionDeLectura,stringAGuardar,longitudS);

	cargar_info_en_dato(&unDato,*posicionDeLectura,longitudS);
	registrar_dato_en_tabla(tabla,unDato);

	*posicionDeLectura += longitudS;

}

