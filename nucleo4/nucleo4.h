#include <stdio.h>
#include <string.h>
#include "system.h"
/* FUNCOES */
extern void far cria_processo(void far (*proc)(), char nome[35], int prior);
extern void far escalador();
extern void far dispara_sistema();
extern void far termina_processo();
