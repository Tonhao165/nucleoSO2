#include <stdio.h>
#include <string.h>
#include "system.h"

/* FUNCOES */
extern void far cria_processo(void far (*proc)(), char nome[35], int max_fila);
extern void far escalador();
extern void far dispara_sistema();
extern void far termina_processo();
extern int far envia(char *nome_destino, char *p_info);
extern void far recebe(char *nome_emissor, char *msg);
