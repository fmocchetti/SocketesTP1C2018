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

void registrar_acceso_a_entrada(t_list** registro,char* primeraPosicionMemoria,char* posicionDeLectura,int tamanioEntrada,int cantidadEntradasAOcupar){

	int numeroEntrada = calcular_entrada(primeraPosicionMemoria,posicionDeLectura,tamanioEntrada);
	int cantidadEntradas = list_size(*registro);
	for(int i = 0 ; i < cantidadEntradasAOcupar ; i++){
		if(numeroEntrada+i < cantidadEntradas){
			struct Registro* reg = obtener_registro(*registro, numeroEntrada+i);
			reg->referenciado += 1;
		}
	}
}
/*
 * Retorna cero si pudo hacer la insercion o -1 en caso contrario (las entradas que hay son todas no atomicas)
 * */

int SET_LRU(t_list** registro,t_list** tabla,char* primeraPosicionMemoria,
		char** posicionDeLectura,char* posicionFinalMemoria,struct ClaveValor* claveValor){


	struct Dato unDato;

	int longitudS = strlen(claveValor->valor)+1;
	int cantidadEntradasAOcupar = calcular_cantidad_entradas(longitudS,claveValor->tamanioEntrada);
	int espacioAOcupar = cantidadEntradasAOcupar*(claveValor->tamanioEntrada);



	if(existe_la_clave(*tabla,claveValor->clave)){


		log_info(logger,"LRU: Se encontro una clave ya existente");

		int check = reemplazar_valor_de_clave_ya_existente(tabla,registro,primeraPosicionMemoria,claveValor);
		if(check < 0){

			log_info(logger,"LRU: No se realizo el reemplazo, el valor nuevo ocupa mas entradas que el actual");
			return -1;

		}
		log_info(logger,"LRU: Se reemplazo su valor por %s: ",claveValor->valor);

	return 0;
	}
	if(no_hay_lugar(espacioAOcupar, *posicionDeLectura, posicionFinalMemoria)){

		log_info(logger,"LRU: No hay lugar para el valor actual, se buscaran entradas libres");

		if(hay_espacio_fragmentado_para_el_valor(tabla,claveValor)){

			log_info(logger,"LRU: Hay entradas libres disponibles, se evaluara si son contiguas");

			char* punteroEntradaLibre = 0;

				if(son_contiguas(tabla,claveValor,cantidadEntradasAOcupar,&punteroEntradaLibre, primeraPosicionMemoria)){

					log_info(logger,"LRU: Las entradas son contiguas, se realizara la inserccion");

					memcpy(*punteroEntradaLibre,claveValor->valor,longitudS);
					log_info(logger,"LRU: Se guardo el valor en memoria");
					cargar_info_en_dato(&unDato,*punteroEntradaLibre,claveValor);
					registrar_dato_en_tabla(tabla,&unDato);

					//TODO: ARREGLAR SI SON MAS DE UNA ENTRADA
					registrar_acceso_a_entrada(registro,primeraPosicionMemoria,punteroEntradaLibre,claveValor->tamanioEntrada,cantidadEntradasAOcupar);


				    return 0;

				}
				else{

					log_info(logger,"LRU: Las entradas no son contiguas, se procedera a compactar");

					//compactar ajusta el puntero posicionDeLectura
					compactar(tabla,primeraPosicionMemoria,posicionDeLectura,posicionFinalMemoria,claveValor->tamanioEntrada);


				}

		}
		else{

				log_info(logger,"LRU: No se encontraron entradas libres");
	            char * posicionReemplazoEntrada = obtener_entrada_menos_accedida(registro,primeraPosicionMemoria,claveValor->tamanioEntrada);
	            log_info(logger,"LRU: Guardara el valor en entrada menos accedida, se pisara el valor: %s", posicionReemplazoEntrada);
				memcpy(posicionReemplazoEntrada,claveValor->valor,longitudS);
				cargar_info_en_dato(&unDato,*posicionReemplazoEntrada,claveValor);
				registrar_dato_en_tabla(tabla,&unDato);
				registrar_acceso_a_entrada(registro,primeraPosicionMemoria,posicionReemplazoEntrada,claveValor->tamanioEntrada,cantidadEntradasAOcupar);


		}


	return 2;
	}
	//guardo el dato entero en memoria si no entro en los if anteriores
	memcpy(*posicionDeLectura,claveValor->valor,longitudS);
	log_info(logger,"LRU:Se guardo el valor: %s",claveValor->valor);
	cargar_info_en_dato(&unDato,*posicionDeLectura,claveValor);
	registrar_dato_en_tabla(tabla,&unDato);
	registrar_acceso_a_entrada(registro,primeraPosicionMemoria,*posicionDeLectura,claveValor->tamanioEntrada,cantidadEntradasAOcupar);

	*posicionDeLectura += espacioAOcupar;




return 0;
}






