#include "LRU.h"



t_list* crear_registro(int cantEntradas){

	t_list* lista = list_create();

	for(int i = 0; i< cantEntradas; i++){
		struct Registro* registro = (struct Registro*) malloc(sizeof(struct Registro));
		registro->numeroEntrada = i;
		registro->referenciado = 0;
		list_add(lista,registro);
	}

return lista;
}

void* liberar_registros(t_list** registro){

	void liberar_dato(struct Registro* unDato){
		free(unDato);
		return;

	}

	list_destroy_and_destroy_elements(*registro,liberar_dato);

}

void* obtener_registro(t_list* registro,int numEntrada){

	bool buscar_registro(struct Registro* reg) {

	     return ((reg->numeroEntrada) == numEntrada);

	}
	struct Registro*  reg = (struct Registro*)list_find(registro,buscar_registro);

return reg;
}

void ordenar_registro(t_list** registro){

	bool menos_accedida_al_comienzo(struct Registro* a,struct Registro* b){

		return(a->referenciado < b->referenciado);
	}
	list_sort(*registro,menos_accedida_al_comienzo);

}

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

	//reemplazo
	if(no_hay_lugar(espacioAOcupar, *posicionDeLectura, posicionFinalMemoria)){

		if(cantidadEntradasAOcupar == 1){// <------...si es atomico... ver esto

			char * posicionReemplazoEntrada = obtener_entrada_menos_accedida(registro,primeraPosicionMemoria,claveValor->tamanioEntrada);
			memcpy(posicionReemplazoEntrada,claveValor->valor,espacioAOcupar);
			cargar_info_en_dato(&unDato,*posicionReemplazoEntrada,claveValor);
			registrar_dato_en_tabla(tabla,&unDato);
			registrar_acceso_a_entrada(registro,primeraPosicionMemoria,posicionReemplazoEntrada,claveValor->tamanioEntrada);

		return 0;
		}
		else{
			puts("No se pudo hacer reemplazo");
			return -1;
		}


	}
	//guardo el dato entero en memoria si no entro en los if anteriores
	memcpy(*posicionDeLectura,claveValor->valor,espacioAOcupar);

	cargar_info_en_dato(&unDato,*posicionDeLectura,claveValor);
	registrar_dato_en_tabla(tabla,&unDato);
	registrar_acceso_a_entrada(registro,primeraPosicionMemoria,*posicionDeLectura,claveValor->tamanioEntrada);

	*posicionDeLectura += espacioAOcupar;




return 0;
}






