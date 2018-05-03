#include "circular.h"
/*
 * circular.c
 *
 *  Created on: 26 abr. 2018
 *      Author: Pablo S Monti
 */

bool no_hay_lugar(int tamanio,char* l,char* fin){

return ((fin-l)<tamanio);
}


void agrego_circular(char** posicionDeLectura,char* stringAGuardar,char* primeraPosicionMemoria,char* posicionFinalMemoria){

	int longitudS = strlen(stringAGuardar);

	if(*posicionDeLectura==posicionFinalMemoria){

		*posicionDeLectura=primeraPosicionMemoria;

	}
	else if( no_hay_lugar( longitudS, *posicionDeLectura, posicionFinalMemoria) ){

		int espacioRestante = posicionFinalMemoria-*posicionDeLectura;

		memcpy(*posicionDeLectura,stringAGuardar,espacioRestante);

		*posicionDeLectura=primeraPosicionMemoria;

		memcpy(*posicionDeLectura,(stringAGuardar+(espacioRestante)),longitudS-espacioRestante);

		*posicionDeLectura += (longitudS-espacioRestante);

		return;

	}

	memcpy(*posicionDeLectura,stringAGuardar,longitudS);

	*posicionDeLectura += longitudS;

}

