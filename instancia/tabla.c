#include "tabla.h"


/*
void agregar_a_tabla(t_list** tabla,struct Dato* unDato){

	list_add(*tabla,unDato);

}
*/
void* buscar(t_list *tabla,char* claveBuscada){

	bool elemento_buscado(struct Dato* unDato) {

	     return ((unDato->clave) == claveBuscada);

	}
	struct Dato* dato = (struct Dato*)list_find(tabla, elemento_buscado);

	// Si no lo encontro devuelve NULL
	if(dato == NULL){
		return NULL;
	}

return dato;
}
/*
char* obtener_valor(char* memoria, char* posicionDeMemoria,unsigned int longBytes,char* valor){
	//veeeeeeeeeeeer solo funciona con malloc


	//memcpy(*valor,posicionDeMemoria,longBytes);
	memcpy(*valor,posicionDeMemoria,longBytes);

return (*valor);
}

char* get_key(char* memoria,t_list* tabla,char* clave){

	struct Dato* datoEncontrado;

	datoEncontrado = buscar(tabla,clave);



	if(datoEncontrado != NULL){
		char valor[datoEncontrado->cantidadDeBytes];

		return (obtener_valor(memoria,datoEncontrado->posicionMemoria,datoEncontrado->cantidadDeBytes,&valor));
	}
	else{
		return NULL; //*****Aborta el ESI******
	}
}
*/

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

void registrar_dato_en_tabla(t_list** tabla,struct Dato* unDato){

	struct Dato* unDatos = (struct Dato*)malloc(sizeof(struct Dato));
	unDatos->cantidadDeBytes = unDato->cantidadDeBytes;
	memcpy(unDatos->clave, unDato->clave,sizeof(unDato->clave));
	unDatos->frecuenciaUso = unDato->frecuenciaUso;
	unDatos->posicionMemoria = unDato->posicionMemoria;

	list_add(*tabla,unDatos);

}

void* liberar_recursos(t_list** tabla){

	void liberar_dato(struct Dato* unDato){
		free(unDato);
		return;

	}

	list_destroy_and_destroy_elements(*tabla,liberar_dato);

}


