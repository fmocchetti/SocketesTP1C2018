/*
 * distribucion.h
 *
 *  Created on: May 28, 2018
 *      Author: fmocchetti
 */

#ifndef DISTRIBUCION_H_
#define DISTRIBUCION_H_

enum {EL , KE, LSU};
int algoritmo_elegido;
int ultima_instancia;
int instanciaLSU;
int letras_instancia;

int informar_planificador(char * clave, unsigned char status);
t_instancia *  distribuir(char *, char *, int);
int modificar_valor_clave(char * clave, char * valor, int instancia);
int store_clave(char * clave, int instancia);
int buscarMasLibre(int totalInstancias);

#endif /* DISTRIBUCION_H_ */
