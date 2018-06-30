#include "LRU.h"

int obtener_numero_entrada(t_list** registro){

		ordenar_registro(registro);
		struct Registro* reg = list_get(*registro,0);

return (reg->numeroEntrada);
}


char* obtener_entrada_menos_accedida(t_list** registro,char* primeraPosicionMemoria,int tamanioEntrada){

	int numeroEntrada = obtener_numero_entrada(registro);

return primeraPosicionMemoria + (numeroEntrada * tamanioEntrada);
}


int calcular_entrada(char* primerPoscionMemoria,char* posicionDeLectura,int tamanioEntrada){

return ((posicionDeLectura - primerPoscionMemoria) / tamanioEntrada);
}

void registrar_acceso_a_entrada(t_list** registro,char* primeraPosicionMemoria,char* posicionDeLectura,int tamanioEntrada){

	int numeroEntrada = calcular_entrada(primeraPosicionMemoria,posicionDeLectura,tamanioEntrada);
	struct Registro* reg = obtener_registro(*registro, numeroEntrada);
	reg->referenciado += 1;

}
/*
 * Retorna cero si pudo hacer la insercion o -1 en caso contrario (las entradas que hay son todas no atomicas)
 * */

int SET_LRU(t_list** registro,t_list** tabla,char* primeraPosicionMemoria,
		char** posicionDeLectura,char* posicionFinalMemoria,struct ClaveValor* claveValor){


	struct Dato unDato;

	int longitudS = strlen(claveValor->valor);
	int cantidadEntradasAOcupar = calcular_cantidad_entradas(longitudS,claveValor->tamanioEntrada);
	int espacioAOcupar = cantidadEntradasAOcupar*(claveValor->tamanioEntrada);



	if(existe_la_clave(*tabla,claveValor->clave)){

		reemplazar_valor_de_clave_ya_existente(tabla,registro,primeraPosicionMemoria,claveValor);
		log_info(logger,"LRU:Se encontro una clave ya existente y se reemplazo su valor");

	return 1;
	}
	if(no_hay_lugar(espacioAOcupar, *posicionDeLectura, posicionFinalMemoria)){

	//	if(hay_espacio_fragmentado(tabla,claveValor->tamanioEntrada,))

			char * posicionReemplazoEntrada = obtener_entrada_menos_accedida(registro,primeraPosicionMemoria,claveValor->tamanioEntrada);
			memcpy(posicionReemplazoEntrada,claveValor->valor,longitudS);
			cargar_info_en_dato(&unDato,*posicionReemplazoEntrada,claveValor);
			registrar_dato_en_tabla(tabla,&unDato);
			registrar_acceso_a_entrada(registro,primeraPosicionMemoria,posicionReemplazoEntrada,claveValor->tamanioEntrada);

	return 2;
	}
	//guardo el dato entero en memoria si no entro en los if anteriores
	memcpy(*posicionDeLectura,claveValor->valor,longitudS);
	log_info(logger,"LRU:Se guardo el valor: %s",claveValor->valor);
	cargar_info_en_dato(&unDato,*posicionDeLectura,claveValor);
	registrar_dato_en_tabla(tabla,&unDato);
	registrar_acceso_a_entrada(registro,primeraPosicionMemoria,*posicionDeLectura,claveValor->tamanioEntrada);

	*posicionDeLectura += espacioAOcupar;




return 0;
}






