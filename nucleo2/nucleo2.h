#include <stdio.h>
#include <string.h>
#include "system.h"

/* FUNCOES */
extern void far cria_processo(void far (*proc)(), char nome[35]);
extern void far escalador();
extern void far dispara_sistema();
extern void far termina_processo();
extern void far down();
extern void far up();
extern void far inicia_semaforo();