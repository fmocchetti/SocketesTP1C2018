#include "tabla.h"


void* buscar(t_list *tabla,char* claveBuscada){

	bool elemento_buscado(struct Dato* unDato) {

	     return (!strcmp((const char*)unDato->clave,(const char*)claveBuscada));


	}
	if (list_size(tabla) <= 0){
		return NULL;
	}
	struct Dato* dato = (struct Dato*)list_find(tabla,elemento_buscado);

return dato;
}


char* buscar_dato_por_posicion(t_list *tabla,char* posicion){
	bool elemento_buscado(struct Dato* unDato) {

	     return (unDato->posicionMemoria == posicion);


	}
	if (list_size(tabla) <= 0){
		return NULL;
	}
	struct Dato* dato = (struct Dato*)list_find(tabla,elemento_buscado);

return dato;

}


int calcular_cant_entradas(int longitudS,int tamEntrada){

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

void* ordenar_tabla(t_list** tabla,char* primeraPosicion){

	bool mas_proximas_al_comienzo(struct Dato* a,struct Dato* b){

		int distanciaA = a->posicionMemoria - primeraPosicion;
		int distanciaB = b->posicionMemoria - primeraPosicion;

		return(distanciaA < distanciaB);
	}
	list_sort(*tabla,mas_proximas_al_comienzo);

}


void* ordenar_tabla_por_valores_de_mayor_bytes(t_list** tabla){

	bool mas_grandes_al_comienzo(struct Dato* a,struct Dato* b){

		int catBytesA = a->cantidadDeBytes;
		int catBytes = b->cantidadDeBytes;

		return(catBytesA > catBytes);
	}
	list_sort(*tabla,mas_grandes_al_comienzo);

}


bool existe_la_clave(t_list *tabla,char* claveBuscada){


return (buscar(tabla,claveBuscada) != NULL);
}

// voy recorriendo la tabla y sumando los bytes que ocupa cada dato y sacandolo de la tabla cuando
//llegue al tamaño del dato que voy a guardar termina
int actualizarTabla(t_list** tabla, int bytesDeDatosASobreescribir){

	int bytesQueOcupaElDatoLeido,i;
	bytesQueOcupaElDatoLeido = i = 0;

	struct Dato* unDato;

	while ( bytesQueOcupaElDatoLeido < bytesDeDatosASobreescribir ){

		unDato = list_get(*tabla,i);
		bytesQueOcupaElDatoLeido += unDato->cantidadDeBytes;
		i++;
	}
	for(int j = 0; j <= (i-1); j++){

		list_remove(*tabla,j);
	}

return 0;

}


int actualizar_tabla_LRU(t_list** tabla, int bytesDeDatosASobreescribir){




return 0;
}

void registrar_dato_en_tabla(t_list** tabla,struct Dato* unDato){

	struct Dato* otroDato = (struct Dato*)malloc(sizeof(struct Dato));
	otroDato->cantidadDeBytes = unDato->cantidadDeBytes;
	memcpy(otroDato->clave, unDato->clave,sizeof(unDato->clave));
	otroDato->posicionMemoria = unDato->posicionMemoria;

	list_add(*tabla,otroDato);

}

int obtener_posicion_del_dato(t_list* tabla,struct Dato* unDato){

	int tamanio = list_size(tabla);
	int i = 0,posicion = -1;

	struct Dato* dato;
	while( i < tamanio ){

		dato = list_get(tabla,i);
		if(!strcmp((const char*)dato->clave,(const char*)unDato->clave)){
			posicion = i;
			i=tamanio +1;
		}
		i++;
	}
return posicion;
}

int borrar_un_dato(t_list** tabla,struct Dato* unDato){

	int posicion = obtener_posicion_del_dato(*tabla,unDato);

	if(posicion >= 0){

		list_remove(*tabla,posicion);
	return 0;
	}

return -1;
}

void* freeDeClaves(char * clave){


	free(clave);


}

int borrar_un_dato_y_liberar(t_list** tabla,struct Dato* unDato){

	int posicion = obtener_posicion_del_dato(*tabla,unDato);

	void* liberar_dato(struct Dato* unDato){

		free(unDato);

	}

	if(posicion >= 0){

		list_destroy_and_destroy_elements(*tabla,liberar_dato);

	return 0;
	}

return -1;
}


void* liberar_recursos(t_list** tabla){

	void* liberar_dato(struct Dato* unDato){
		free(unDato);

	}

	list_destroy_and_destroy_elements(*tabla,liberar_dato);

}


