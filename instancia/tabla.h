#ifndef TABLA_H_
#define TABLA_H_

#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/node.h>
#include "Dato.h"


//void agregar_a_tabla(t_list** tabla,struct Dato* unDato);

void* buscar(t_list *tabla,char* claveBuscada);

/*
 * get_key busca por clave en la memoria y devuelve la posicion de memoria en donde
 * se encuentra dicho valor de la clave
 * */
//char* obtener_valor(char* memoria, char* posicionDeMemoria,unsigned int longBytes,char* valor);

//char* get_key(char* memoria,t_list* tabla,char* clave);

void registrar_dato_en_tabla(t_list** tabla,struct Dato* unDato);


#endif /* TABLA_H_ */
