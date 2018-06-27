#include <stdio.h>
#include <string.h>
#include "system.h"

typedef struct desc_p{
    char nome[35];
    enum{ativo, terminado, bloq_P} estado;
    PTR_DESC contexto;
    struct desc_p *prox_desc;
    struct desc_p *fila_sem;
}DESCRITOR_PROC;

typedef DESCRITOR_PROC *PTR_DESC_PROC;

typedef struct {
    int s;
    PTR_DESC_PROC Q; /* fila */
}semaforo;

/* FUNCOES */
extern void far cria_processo(void far (*proc)(), char nome[35]);
extern void far escalador();
extern void far dispara_sistema();
extern void far termina_processo();
extern void far down();
extern void far up();
extern void far inicia_semaforo();