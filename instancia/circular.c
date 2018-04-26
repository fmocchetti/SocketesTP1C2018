/*
 * circular.c
 *
 *  Created on: 26 abr. 2018
 *      Author: Pablo S Monti
 */

void agrego_circular(char** l,char* s,char* ppio,char* fin){

	int longitudS = strlen(s);

	if(*l==fin){

		*l=ppio;
	}
	else{
		// chequear si no entra y asignar

	}

	memcpy(*l,s,longitudS);
	*l += longitudS;

}

