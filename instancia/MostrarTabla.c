/*
 * MostrarTabla.c
 *
 *  Created on: 3 ago. 2018
 *      Author: pablomonti
 */

#include "MostrarTabla.h"


void mostrar_tabla(t_list* tabla, int cantEntradas,int tamEntradas,char*  primeraPos){

	t_list* tablaAux = list_duplicate(tabla);
	printf("\n");
	log_info(logger,"ESTADO DE TABLA:");
	printf("\n");
	ordenar_tabla(&tablaAux,primeraPos);
	int sizeTabla = list_size(tablaAux);
	int vector[cantEntradas];

	for(int i = 0;i<cantEntradas;i++){

		vector[i]=0;
	}
	//cargo bitmap

	for(int i = 0;i<sizeTabla;i++){

		struct Dato* unDato = list_get(tablaAux,i);
		int entrada = calcular_entrada(primeraPos,unDato->posicionMemoria,tamEntradas);

		vector[entrada]=1;

	}

	int i =0,j=0;
	while(i<cantEntradas){


		if(vector[i]==1 && j<sizeTabla){

			struct Dato* unDato = list_get(tablaAux,j);
			char* a = (char*)malloc(unDato->cantidadDeBytes+1);
			int cantEntradasDato = calcular_cant_entradas(unDato->cantidadDeBytes,tamEntradas);
			//int entrada = calcular_entrada(primeraPos,unDato->posicionMemoria,tamEntradas);

			if(cantEntradasDato>1){

					for(int j = 0;j<cantEntradasDato;j++){

						if(j==(cantEntradasDato-1)){
							int resto = unDato->cantidadDeBytes % tamEntradas;
							memcpy(a,unDato->posicionMemoria+(tamEntradas*j),resto);
							a[resto] = '\0';
						}
						else{

							memcpy(a,unDato->posicionMemoria+(tamEntradas*j),tamEntradas);
							a[tamEntradas] = '\0';
						}
						printf("Entrada %d. %s \n",i,a);
						i++;
					}
					free(a);
					j++;
					continue;
			}
			else{
					memcpy(a,unDato->posicionMemoria,unDato->cantidadDeBytes);
					a[unDato->cantidadDeBytes] = '\0';
					printf("Entrada %d. %s \n",i,a);
					free(a);
					j++;

			}



		}
		else{

			printf("Entrada %d. %s \n",i,"");

		}

		i++;


	}



	free(tablaAux);

}

