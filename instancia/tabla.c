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
void registrar_dato_en_tabla(t_list** tabla,struct Dato* unDato){

	list_add(*tabla,unDato);

}


