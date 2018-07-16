/*
 * LRU.h
 *
 *  Algoritmo de insercion Last Recently Used
 */

#ifndef LRU_H_
#define LRU_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include "ClaveValor.h"
#include "Dato.h"
#include "tabla.h"
#include "circular.h"
#include "socket_client.h"
#include "Registro.h"


t_list* crear_registro(int cantEntradas);

void* obtener_registro(t_list* registro,int numEntrada);

void ordenar_registro(t_list** registro);

bool no_hay_lugar(int tamanio,char* posicionDeLectura,char* posicionFinalMemoria);

int obtener_numero_entrada(t_list** registro);

int calcular_entrada(char* primerPoscionMemoria,char* posicionDeLectura,int tamanioEntrada);

void registrar_acceso_a_entrada(t_list** registro,char* primeraPosicionMemoria,char* posicionDeLectura,int tamanioEntrada,int cantidadEntradasAOcupar);

int SET_LRU(t_list** r,t_list** t,char* primPosMem,char** posLectura,char* posFinMem,struct ClaveValor* cV);

char* obtener_entrada_menos_accedida(t_list** registro,char* primeraPosicionMemoria,int tamanioEntrada);

int liberar_entradas_atomicas_menos_accedidas(t_list** registro,t_list** tabla, char*  primeraPosicionMemoria, int tamanioEntrada,int cantidadEntradasNecesariasLiberar);

#endif /* LRU_H_ */
