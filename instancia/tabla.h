#ifndef TABLA_H_
#define TABLA_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/node.h>
#include "Dato.h"
#include "socket_client.h"


void freeDeClaves(void*);

void ordenar_tabla(t_list** tabla,char* primeraPosicion);

t_list* copiar(t_list* tabla);

void* buscar(t_list *tabla,char* claveBuscada);

int calcular_cant_entradas_libres(t_list *tabla,int tamEntrada,int cantEntradas);

void liberar_recursos(t_list** tabla);

struct Dato* buscar_dato_por_posicion(t_list *tabla,char* posicion);

void registrar_dato_en_tabla(t_list** tabla,struct Dato* unDato);

int actualizarTabla(t_list** tabla, int bytesDeDatosASobreescribir);

bool existe_la_clave(t_list *tabla,char* claveBuscada);

int obtener_posicion_del_dato(t_list* tabla,struct Dato* unDato);

void ordenar_tabla_por_valores_de_mayor_bytes(t_list** tabla);

int borrar_un_dato(t_list** tabla,struct Dato* unDato);

int borrar_un_dato_y_liberar(t_list** tabla,struct Dato* unDato);

int calcular_cant_entradas(int longitudS,int tamEntrada);//<-- esto no deberia estar aca

#endif /* TABLA_H_ */
