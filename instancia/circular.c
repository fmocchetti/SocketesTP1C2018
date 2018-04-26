#include "circular.h"
/*
 * circular.c
 *
 *  Created on: 26 abr. 2018
 *      Author: Pablo S Monti
 */

bool no_hay_lugar(int tamanio,char* l,char* fin){

return ((int)(fin-l)<tamanio) ? true : false;
}


void agrego_circular(char** l,char* s,char* ppio,char* fin){

	int longitudS = strlen(s);

	if(*l==fin){

		*l=ppio;

	}
	else if( no_hay_lugar( longitudS, *l, fin) ){

		int espacioRestante = fin-*l;
		memcpy(*l,s,espacioRestante);
		*l=ppio;
		memcpy(*l,(s+(espacioRestante)),longitudS-espacioRestante);
		*l += (longitudS-espacioRestante);
		return;

	}

	memcpy(*l,s,longitudS);
	*l += longitudS;

}

