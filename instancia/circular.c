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

int calcular_cantidad_entradas(int longitudS,int tamEntrada){

	int resto = longitudS % tamEntrada;
	int resultado = longitudS / tamEntrada;
	if(tamEntrada > longitudS){

		return 1;
	}
	else if(resto > 0){

		return resultado + 1;
	}
return resultado;
}

void cargar_info_en_dato(struct Dato* unDato,char* posicionDeLectura,struct ClaveValor* claveValor){

	int longitudS = strlen(claveValor->valor);

	(unDato)->posicionMemoria = posicionDeLectura;
	(unDato)->cantidadDeBytes = longitudS;
	(unDato)->frecuenciaUso = 0;
	strcpy((char*)(unDato->clave),claveValor->clave);
	//memcpy((unDato->clave),claveValor->clave,40);
}

//ingreso un valor en memoria con logica circular y registro en la tabla de entradas dicha insercion
void SET_circular(char** posicionDeLectura,t_list** tabla,struct ClaveValor* claveValor,char* primeraPosicionMemoria,char* posicionFinalMemoria){

	struct Dato unDato;

	int longitudS = strlen(claveValor->valor);
	int cantidadEntradasAOcupar = calcular_cantidad_entradas(longitudS,claveValor->tamanioEntrada);
	int espacioAOcupar = cantidadEntradasAOcupar*(claveValor->tamanioEntrada);
	//si se termino la memoria vuelvo al principio
	if(*posicionDeLectura==posicionFinalMemoria){

		*posicionDeLectura=primeraPosicionMemoria;

	}
	//si no hay lugar para todo el string lo parto y coloco lo que entra y el resto al principio
	//de la memoria
	if( no_hay_lugar( espacioAOcupar, *posicionDeLectura, posicionFinalMemoria) ){

		int espacioRestante = posicionFinalMemoria-*posicionDeLectura;

		memcpy(*posicionDeLectura,claveValor->valor,espacioRestante);

		cargar_info_en_dato(&unDato,*posicionDeLectura,claveValor);

		actualizarTabla(tabla,longitudS - espacioRestante);

		registrar_dato_en_tabla(tabla,&unDato);

		*posicionDeLectura=primeraPosicionMemoria;

		memcpy(*posicionDeLectura,((claveValor->valor)+espacioRestante),espacioAOcupar-espacioRestante);

		*posicionDeLectura += (espacioAOcupar-espacioRestante);

		return;

	}
	//guardo el dato entero en memoria si no entro en los if anteriores
	memcpy(*posicionDeLectura,claveValor->valor,espacioAOcupar);

	cargar_info_en_dato(&unDato,*posicionDeLectura/*,longitudS*/,claveValor);
	registrar_dato_en_tabla(tabla,&unDato);

	*posicionDeLectura += espacioAOcupar;

}

