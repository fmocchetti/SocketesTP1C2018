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

int distribuir(char *, char *, unsigned char);
int modificar_valor_clave(char *, char *, int);
int store_clave(char *, int);

#endif /* DISTRIBUCION_H_ */
