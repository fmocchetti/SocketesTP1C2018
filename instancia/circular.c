#include "circular.h"



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

	int longitudS = strlen(claveValor->valor)+1;

	(unDato)->posicionMemoria = posicionDeLectura;
	(unDato)->cantidadDeBytes = longitudS;
	strcpy((char*)(unDato->clave),claveValor->clave);
	//memcpy((unDato->clave),claveValor->clave,40);
}

//ingreso un valor en memoria con logica circular y registro en la tabla de entradas dicha insercion
int SET_circular(char** posicionDeLectura,t_list** tabla,struct ClaveValor* claveValor,char* primeraPosicionMemoria,char* posicionFinalMemoria){


	struct Dato unDato;

	int longitudS = strlen(claveValor->valor)+1;
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
	//si se termino la memoria vuelvo al principio
	if(*posicionDeLectura==posicionFinalMemoria){

		*posicionDeLectura=primeraPosicionMemoria;

	}
	//si no hay lugar para todo el string lo parto y coloco lo que entra y el resto al principio
	//de la memoria
	if( no_hay_lugar( espacioAOcupar, *posicionDeLectura, posicionFinalMemoria) ){

		log_info(logger,"CIRC:No hay lugar para el valor actual, se buscaran entradas libres");

		if(hay_espacio_fragmentado_para_el_valor(tabla,claveValor)){

			log_info(logger,"CIRC:Hay entradas libres disponibles, se evaluara si son contiguas");

			char* punteroEntradaLibre = 0;

				if(son_contiguas(tabla,claveValor,cantidadEntradasAOcupar,&punteroEntradaLibre, primeraPosicionMemoria)){

					log_info(logger,"CIRC:Las entradas son contiguas, se realizara la inserccion");

					memcpy(*punteroEntradaLibre,claveValor->valor,longitudS);
					log_info(logger,"CIRC:Se guardo el valor en memoria");
					cargar_info_en_dato(&unDato,punteroEntradaLibre,claveValor);
					registrar_dato_en_tabla(tabla,&unDato);


				    return 0;

				}
				else{

					log_info(logger,"CIRC:Las entradas no son contiguas, se procedera a compactar");

					//compactar ajusta el puntero posicionDeLectura
					compactar(tabla,primeraPosicionMemoria,posicionDeLectura,posicionFinalMemoria,claveValor->tamanioEntrada);


				}


		}
		else{


				int espacioRestante = posicionFinalMemoria-*posicionDeLectura;

				memcpy(*posicionDeLectura,claveValor->valor,espacioRestante);

				cargar_info_en_dato(&unDato,*posicionDeLectura,claveValor);

				actualizarTabla(tabla,longitudS - espacioRestante);

				registrar_dato_en_tabla(tabla,&unDato);

				*posicionDeLectura=primeraPosicionMemoria;

				memcpy(*posicionDeLectura,((claveValor->valor)+espacioRestante),espacioAOcupar-espacioRestante);

				*posicionDeLectura += (espacioAOcupar-espacioRestante);

				return 0;

		}
	}



	memcpy(*posicionDeLectura,claveValor->valor,longitudS);

	log_info(logger,"CIRC:Se guardo el valor en memoria");


	cargar_info_en_dato(&unDato,*posicionDeLectura,claveValor);
	registrar_dato_en_tabla(tabla,&unDato);

	*posicionDeLectura += espacioAOcupar;

	log_info(logger,"CIRC: Se inserto el valor %s",claveValor->valor);

return 0;
}

