

#ifndef BSU_H_
#define BSU_H_

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


void ordenar_tabla_por_bytes(t_list** tabla);

bool no_hay_lugar(int tamanio,char* posicionDeLectura,char* posicionFinalMemoria);

#endif /* BSU_H_ */
