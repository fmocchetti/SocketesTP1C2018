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

	int longitudS = strlen(claveValor->valor);

	(unDato)->posicionMemoria = posicionDeLectura;
	(unDato)->cantidadDeBytes = longitudS;
	strcpy((char*)(unDato->clave),claveValor->clave);
	//memcpy((unDato->clave),claveValor->clave,40);
}

//ingreso un valor en memoria con logica circular y registro en la tabla de entradas dicha insercion
int SET_circular(char** posicionDeLectura,t_list** tabla,struct ClaveValor* claveValor,char* primeraPosicionMemoria,char* posicionFinalMemoria){


	struct Dato unDato;

	int longitudS = strlen(claveValor->valor);
	int cantidadEntradasAOcupar = calcular_cantidad_entradas(longitudS,claveValor->tamanioEntrada);
	int espacioAOcupar = cantidadEntradasAOcupar*(claveValor->tamanioEntrada);

	if(existe_la_clave(*tabla,claveValor->clave)){

		reemplazar_valor_de_clave_ya_existente(tabla,NULL,primeraPosicionMemoria,claveValor);
		log_info(logger,"CIRC:Se encontro una clave ya existente y se reemplazo su valor");

	return 1;
	}
	//si se termino la memoria vuelvo al principio
	if(*posicionDeLectura==posicionFinalMemoria){

		*posicionDeLectura=primeraPosicionMemoria;

	}
	//si no hay lugar para todo el string lo parto y coloco lo que entra y el resto al principio
	//de la memoria
	if( no_hay_lugar( espacioAOcupar, *posicionDeLectura, posicionFinalMemoria) ){

			log_info(logger,"CIRC:Se prueba compactar");

			compactar(tabla,primeraPosicionMemoria,posicionDeLectura,posicionFinalMemoria,claveValor->tamanioEntrada);

			int espacioRestante = posicionFinalMemoria-*posicionDeLectura;

			memcpy(*posicionDeLectura,claveValor->valor,espacioRestante);

			cargar_info_en_dato(&unDato,*posicionDeLectura,claveValor);

			actualizarTabla(tabla,longitudS - espacioRestante);

			registrar_dato_en_tabla(tabla,&unDato);

			*posicionDeLectura=primeraPosicionMemoria;

			memcpy(*posicionDeLectura,((claveValor->valor)+espacioRestante),espacioAOcupar-espacioRestante);

			*posicionDeLectura += (espacioAOcupar-espacioRestante);


			return 0;
/*		}
		else{

			log_info(logger,"El valor no es Atomico, no se guardo el valor con CIRCULAR");
			return -1;
		}
*/
	}
	//guardo el dato entero en memoria si no entro en los if anteriores
	memcpy(*posicionDeLectura,claveValor->valor,longitudS/*espacioAOcupar*/);

	log_info(logger,"CIRC:Se guardo el valor en memoria");


	cargar_info_en_dato(&unDato,*posicionDeLectura/*,longitudS*/,claveValor);
	registrar_dato_en_tabla(tabla,&unDato);

	*posicionDeLectura += espacioAOcupar;


return 0;
}

