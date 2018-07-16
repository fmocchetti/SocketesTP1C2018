#include "BSU.h"




//a la hora de reemplazar me fijo cuantas entradas ocupa el nuevo valor,
//busco en la tabla una entrada del mismo tamanio (tengo funcion que calcula cantidad de entradas) y reemplazo
//Si no son contiguas, compacto y luego reemplazo

void ordenar_tabla_por_bytes(t_list** tabla){


	bool mas_bytes_al_comienzo(struct Dato* a, struct Dato* b ){

		return (a->cantidadDeBytes > b->cantidadDeBytes);
	}

	list_sort(*tabla,(void*)mas_bytes_al_comienzo);
}


void liberar_entrada_por_bytes(t_list** tabla, int cantidadEntradasDato){

		int sizeTabla = list_size(*tabla);
		ordenar_tabla_por_bytes(tabla);
		int contadorEntradasUtilizadas = 0;
		int i = 0;

		while(cantidadEntradasDato<contadorEntradasUtilizadas && i < sizeTabla){
			struct Dato * datoAAnalizar=get(tabla,i);
			int tipoDeEntrada = calcular_cantidad_entradas(datoAAnalizar); //si es atomica o no
			if(tipoDeEntrada==1){
				void liberar_dato(struct Dato* unDato){
						free(unDato);
				list_remove_and_destroy_element(tabla,i,liberar_dato);
			}
		}


}





int SET_BSU(t_list** registro,t_list** tabla,char* primeraPosicionMemoria,char** posicionDeLectura,
			char* posicionFinalMemoria,struct ClaveValor* claveValor){

	struct Dato unDato;

		int longitudS = strlen(claveValor->valor)+1;
		int cantidadEntradasAOcupar = calcular_cantidad_entradas(longitudS,claveValor->tamanioEntrada);
		int espacioAOcupar = cantidadEntradasAOcupar*(claveValor->tamanioEntrada);



		if(existe_la_clave(*tabla,claveValor->clave)){


			log_info(logger,"BSU: Se encontro una clave ya existente");

			int check = reemplazar_valor_de_clave_ya_existente(tabla,NULL,primeraPosicionMemoria,claveValor);
			if(check < 0){

				log_info(logger,"BSU: No se realizo el reemplazo, el valor nuevo ocupa mas entradas que el actual");
				return -1;

			}
			log_info(logger,"BSU: Se reemplazo su valor por %s: ",claveValor->valor);

		return 0;
		}
		if(no_hay_lugar(espacioAOcupar, *posicionDeLectura, posicionFinalMemoria)){

			log_info(logger,"BSU: No hay lugar para el valor actual, se buscaran entradas libres");

			if(hay_espacio_fragmentado_para_el_valor(tabla,claveValor)){

				log_info(logger,"BSU: Hay entradas libres disponibles, se evaluara si son contiguas");

				char* punteroEntradaLibre = 0;

					if(son_contiguas(tabla,claveValor,cantidadEntradasAOcupar,&punteroEntradaLibre, primeraPosicionMemoria)){

						log_info(logger,"BSU: Las entradas son contiguas, se realizara la inserccion");

						memcpy(*punteroEntradaLibre,claveValor->valor,longitudS);
						log_info(logger,"BSU: Se guardo el valor en memoria");
						cargar_info_en_dato(&unDato,*punteroEntradaLibre,claveValor);
						registrar_dato_en_tabla(tabla,&unDato);

						//TODO: ARREGLAR SI SON MAS DE UNA ENTRADA
						//registrar_acceso_a_entrada(registro,primeraPosicionMemoria,punteroEntradaLibre,claveValor->tamanioEntrada,cantidadEntradasAOcupar);


					    return 0;

					}
					else{

						log_info(logger,"BSU: Las entradas no son contiguas, se procedera a compactar");

						//compactar ajusta el puntero posicionDeLectura
						compactar(tabla,primeraPosicionMemoria,posicionDeLectura,posicionFinalMemoria,claveValor->tamanioEntrada);


					}

			}
			else{

					log_info(logger,"BSU: No se encontraron entradas libres");
		            liberar_entradas_por_bytes(tabla,cantidadEntradasAOcupar);

		            log_info(logger,"BSU: Guardara el valor en entrada menos accedida, se pisara el valor: %s", posicionReemplazoEntrada);
					memcpy(posicionReemplazoEntrada,claveValor->valor,longitudS);
					cargar_info_en_dato(&unDato,*posicionReemplazoEntrada,claveValor);
					registrar_dato_en_tabla(tabla,&unDato);
					//registrar_acceso_a_entrada(registro,primeraPosicionMemoria,posicionReemplazoEntrada,claveValor->tamanioEntrada,cantidadEntradasAOcupar);


			}

		}
		//guardo el dato entero en memoria si no entro en los if anteriores
		memcpy(*posicionDeLectura,claveValor->valor,longitudS);
		log_info(logger,"BSU:Se guardo el valor: %s",claveValor->valor);
		cargar_info_en_dato(&unDato,*posicionDeLectura,claveValor);
		registrar_dato_en_tabla(tabla,&unDato);
		//registrar_acceso_a_entrada(registro,primeraPosicionMemoria,*posicionDeLectura,claveValor->tamanioEntrada,cantidadEntradasAOcupar);

		*posicionDeLectura += espacioAOcupar;

	return 0;
	}

}
