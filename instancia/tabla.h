#ifndef TABLA_H_
#define TABLA_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/node.h>
#include "Dato.h"

void* freeDeClaves(char*);

void* buscar(t_list *tabla,char* claveBuscada);

void* liberar_recursos(t_list** tabla);

char* buscar_dato_por_posicion(t_list *tabla,char* posicion);

void registrar_dato_en_tabla(t_list** tabla,struct Dato* unDato);

int actualizarTabla(t_list** tabla, int bytesDeDatosASobreescribir);

int actualizar_tabla_LRU(t_list** tabla, int bytesDeDatosASobreescribir);


int obtener_posicion_del_dato(t_list* tabla,struct Dato* unDato);

int borrar_un_dato(t_list** tabla,struct Dato* unDato);

int borrar_un_dato_y_liberar(t_list** tabla,struct Dato* unDato);

int calcular_cant_entradas(int longitudS,int tamEntrada);//<-- esto no deberia estar aca

#endif /* TABLA_H_ */
