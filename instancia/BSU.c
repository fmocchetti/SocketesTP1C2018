/*
 * BSU.c
 *
 *  Created on: 17 jul. 2018
 *      Author: PabloSMonti
 */
#include "BSU.h"



int liberar_entradas_atomicas_con_valores_mas_grandes(t_list** tabla, char*  primeraPosicionMemoria
		, int tamanioEntrada,int cantidadEntradasNecesariasLiberar){



	  int sizeTabla = list_size(*tabla);
	  int i = 0, entradasLiberadas = 0;
	  struct Dato* unDato;
	  ordenar_tabla_por_valores_de_mayor_bytes(tabla);
	  while(i< sizeTabla && entradasLiberadas < cantidadEntradasNecesariasLiberar){

			unDato  = list_get(*tabla,i);
			if(calcular_cantidad_entradas(unDato->cantidadDeBytes,tamanioEntrada)==1){ //es atomica

				borrar_un_dato_y_liberar(tabla,unDato);
				sizeTabla = list_size(*tabla);// chequear si no cambia el size cuando borro el dato

			}
			i++;
			entradasLiberadas++;

	  }
	  log_info(logger,"BSU: Se liberaron %d entradas atomicas de valor de mayor bytes",entradasLiberadas);
return 0;
}





int SET_BSU(int server,t_list** tabla,char* primeraPosicionMemoria,
		char** posicionDeLectura,char* posicionFinalMemoria,struct ClaveValor* claveValor){


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



				    return 0;

				}
				else{

					log_info(logger,"BSU: Las entradas no son contiguas, se procedera a compactar, le aviso al cordi");
					if(server >=0 ){
						unsigned char id = 200;
						send(server,&id,1,0);

					}


					//compactar ajusta el puntero posicionDeLectura
					compactar(tabla,primeraPosicionMemoria,posicionDeLectura,posicionFinalMemoria,claveValor->tamanioEntrada);


				}

		}
		else{

				log_info(logger,"BSU: No se encontraron entradas libres suficientes, se liberaran entradas atomicas con mas bytes");
				int entradasLibres = calcular_cant_entradas(posicionFinalMemoria - *posicionDeLectura,claveValor->tamanioEntrada);
				liberar_entradas_atomicas_con_valores_mas_grandes(tabla,primeraPosicionMemoria,claveValor->tamanioEntrada,cantidadEntradasAOcupar-entradasLibres);
	            log_info(logger,"BSU: Se liberaron entrasdas, se chequara si son contiguas");
				char* punteroEntradaLibre = 0;

				if(son_contiguas(tabla,claveValor,cantidadEntradasAOcupar,&punteroEntradaLibre, primeraPosicionMemoria)){

						log_info(logger,"BSU: Las entradas son contiguas, se realizara la inserccion");

						memcpy(*punteroEntradaLibre,claveValor->valor,longitudS);
						log_info(logger,"BSU: Se guardo el valor en memoria");
						cargar_info_en_dato(&unDato,*punteroEntradaLibre,claveValor);
						registrar_dato_en_tabla(tabla,&unDato);


					    return 0;

				}
				else{

						log_info(logger,"BSU: Las entradas no son contiguas, se procedera a compactar, le aviso al cordi");

						if(server >=0 ){
							unsigned char id = 200;
							send(server,&id,1,0);

						}

						//compactar ajusta el puntero posicionDeLectura
						compactar(tabla,primeraPosicionMemoria,posicionDeLectura,posicionFinalMemoria,claveValor->tamanioEntrada);


				}


				memcpy(posicionDeLectura,claveValor->valor,longitudS);
				cargar_info_en_dato(&unDato,*posicionDeLectura,claveValor);
				registrar_dato_en_tabla(tabla,&unDato);
				log_info(logger,"BSU: Se sobre-escribieron las entradas");
				return 0;

		}

	}
	//guardo el dato entero en memoria si no entro en los if anteriores
	memcpy(*posicionDeLectura,claveValor->valor,longitudS);
	log_info(logger,"BSU:Se guardo el valor: %s",claveValor->valor);
	cargar_info_en_dato(&unDato,*posicionDeLectura,claveValor);
	registrar_dato_en_tabla(tabla,&unDato);


	*posicionDeLectura += espacioAOcupar;




return 0;
}
