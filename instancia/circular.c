#include "circular.h"



// chequeo si el espacio libre es menor al tamaño requerido
bool no_hay_lugar(int tamanio,char* posicionDeLectura,char* posicionFinalMemoria){

return ((posicionFinalMemoria - posicionDeLectura) < tamanio);
}

int calcular_cantidad_entradas(int longitudS,int tamEntrada){

	int resto = longitudS % tamEntrada;
	int resultado = longitudS / tamEntrada;
	if(longitudS == 0){

		return 0;
	}
	if(tamEntrada > longitudS){

		return 1;
	}
	else if(resto > 0){

		return resultado + 1;
	}
return resultado;
}


void cargar_info_en_dato(struct Dato* unDato,char* posicionDeLectura,struct ClaveValor* claveValor){

	unsigned int longitudS = strlen(claveValor->valor);

	(unDato)->posicionMemoria = posicionDeLectura;
	(unDato)->cantidadDeBytes = longitudS;
	//strcpy((char*)(unDato->clave),claveValor->clave);
	unDato->clave = claveValor->clave;

}

int liberar_entradas_atomicas(t_list**tabla,char*  primeraPosicionMemoria,int tamanioEntrada,int cantidadEntradasNecesariasLiberar){

	  int sizeTabla = list_size(*tabla);
	  int i = 0, entradasLiberadas = 0;
	  struct Dato* unDato;
	  while(i< sizeTabla && entradasLiberadas < cantidadEntradasNecesariasLiberar){

		    unDato  = list_get(*tabla,i);
			if(calcular_cantidad_entradas(unDato->cantidadDeBytes,tamanioEntrada) == 1){ //es atomica

				borrar_un_dato_y_liberar(tabla,unDato);
				sizeTabla = list_size(*tabla);// chequear si no cambia el size cuando borro el dato
				entradasLiberadas++;

			}
			i++;


	  }
	  log_info(logger,"CIRC: Se liberaron %d entradas atomicas",entradasLiberadas);

	  if(entradasLiberadas == cantidadEntradasNecesariasLiberar){

	  	return 0;
	  }
	  else{

	  	return -1;
	  }

}




//ingreso un valor en memoria con logica circular y registro en la tabla de entradas dicha insercion
int SET_circular(int server,char** posicionDeLectura,t_list** tabla,struct ClaveValor* claveValor,char* primeraPosicionMemoria,char* posicionFinalMemoria){


	struct Dato unDato;

	int longitudS = strlen(claveValor->valor);
	int cantidadEntradasAOcupar = calcular_cantidad_entradas(longitudS,claveValor->tamanioEntrada);
	int espacioAOcupar = cantidadEntradasAOcupar*(claveValor->tamanioEntrada);

	if(existe_la_clave(*tabla,claveValor->clave)){

		log_info(logger,"CIRC: Se encontro una clave ya existente");

		int check = reemplazar_valor_de_clave_ya_existente(tabla,NULL,primeraPosicionMemoria,claveValor);
		if(check < 0){

			log_info(logger,"CIRC: No se realizo el reemplazo, el valor nuevo ocupa mas entradas que el actual");
			return -1;

		}
		log_info(logger,"CIRC: Se reemplazo su valor");

	return 1;
	}
	/*
	//si se termino la memoria vuelvo al principio
	if(*posicionDeLectura==posicionFinalMemoria){

		*posicionDeLectura = primeraPosicionMemoria;


	}
	*/


	//if( no_hay_lugar( espacioAOcupar, *posicionDeLectura, posicionFinalMemoria) ){

		log_info(logger,"CIRC: Se buscaran entradas libres");

		if(hay_espacio_fragmentado_para_el_valor(tabla,claveValor)){

			log_info(logger,"CIRC: Hay entradas libres disponibles, se evaluara si son contiguas");

			char* punteroEntradaLibre = 0;

				if(son_contiguas(tabla,claveValor,cantidadEntradasAOcupar,&punteroEntradaLibre, primeraPosicionMemoria)){

					log_info(logger,"CIRC: Las entradas son contiguas, se realizara la inserccion");

					memmove(punteroEntradaLibre,claveValor->valor,longitudS);
					log_info(logger,"CIRC: Se guardo el valor en memoria");
					cargar_info_en_dato(&unDato,punteroEntradaLibre,claveValor);
					registrar_dato_en_tabla(tabla,&unDato);


				    return 0;

				}
				else{

					log_info(logger,"CIRC: Las entradas no son contiguas, se procedera a compactar, le aviso al cordi");

					/*//compactar ajusta el puntero posicionDeLectura
					if(server >=0 ){
						unsigned char id = 200;
						send(server,&id,1,0);

					}
*/
					compactar(tabla,primeraPosicionMemoria,posicionDeLectura,claveValor->tamanioEntrada);


				}


		}
		else{

				log_info(logger,"CIRC: No hay entradas libres suficientes, se liberaran entradas atomicas");

				int entradasLibres = calcular_cant_entradas_libres(*tabla,claveValor->tamanioEntrada,claveValor->cantidadEntradas);

				int control = liberar_entradas_atomicas(tabla,primeraPosicionMemoria,claveValor->tamanioEntrada,cantidadEntradasAOcupar-entradasLibres);
				if(control < 0 ){

					log_error(logger,"CIRC: No se encontraron entradas atomicas para liberar");
					exit(-1);


				}
				log_info(logger,"CIRC: Se liberaron entradas, se comprobara si son contiguas");

				char* punteroEntradaLibre = 0;

				if(son_contiguas(tabla,claveValor,cantidadEntradasAOcupar,&punteroEntradaLibre, primeraPosicionMemoria)){

					log_info(logger,"CIRC: Las entradas son contiguas, se realizara la inserccion");

					memmove(punteroEntradaLibre,claveValor->valor,longitudS);
					log_info(logger,"CIRC: Se guardo el valor en memoria");
					cargar_info_en_dato(&unDato,punteroEntradaLibre,claveValor);
					registrar_dato_en_tabla(tabla,&unDato);


				    return 0;

				}
				else{

					log_info(logger,"CIRC: Las entradas no son contiguas, se procedera a compactar, le aviso al cordi");
			/*		if(server >=0 ){
						unsigned char id = 200;
						send(server,&id,1,0);

					}
*/

					//compactar ajusta el puntero posicionDeLectura
					compactar(tabla,primeraPosicionMemoria,posicionDeLectura,claveValor->tamanioEntrada);


				}



			/*


				int espacioRestante = posicionFinalMemoria-*posicionDeLectura;

				memcpy(*posicionDeLectura,claveValor->valor,espacioRestante);

				cargar_info_en_dato(&unDato,*posicionDeLectura,claveValor);

				actualizarTabla(tabla,longitudS - espacioRestante);

				registrar_dato_en_tabla(tabla,&unDato);

				*posicionDeLectura=primeraPosicionMemoria;

				memcpy(*posicionDeLectura,((claveValor->valor)+espacioRestante),espacioAOcupar-espacioRestante);

				*posicionDeLectura += (espacioAOcupar-espacioRestante);

				return 0;
*/
		}
//	}



	memcpy(*posicionDeLectura,claveValor->valor,longitudS);

	cargar_info_en_dato(&unDato,*posicionDeLectura,claveValor);
	registrar_dato_en_tabla(tabla,&unDato);

	*posicionDeLectura += espacioAOcupar;

	log_info(logger,"CIRC: Se inserto el valor %s",claveValor->valor);
/*
	//si se termino la memoria vuelvo al principio
	if(*posicionDeLectura==posicionFinalMemoria){

		log_info(logger,"CIRC: El puntero llego a fin de memoria");

		*posicionDeLectura = primeraPosicionMemoria;


	}

*/


return 0;
}

