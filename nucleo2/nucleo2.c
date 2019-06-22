/*************************************************
    Projeto de SO2

    Antônio Eugênio Domingues Silva     RA: 161021336
    Thiago Hoffart Vieira               RA: 161026524
    Lucas Vinhas Gianoglio              RA: 161023584
 
**************************************************/

#include <stdio.h>
#include <string.h>
#include "system.h"

/* pego dos slides */
typedef struct registros
{
	unsigned bx1, es1;
}regis;

typedef union k
{
	regis x;
	char far *y;
}APONTA_REG_CRIT;

APONTA_REG_CRIT a;


/* DESCRITOR DE PROCESSOS */
typedef struct desc_p{
    char nome[35];
    enum{ativo, terminado, bloq_P} estado;
    PTR_DESC contexto;
    struct desc_p *prox_desc;
    struct desc_p *fila_sem;
}DESCRITOR_PROC;

typedef DESCRITOR_PROC *PTR_DESC_PROC;


/*SEMAFORO pego dos slides */
typedef struct {
    int s;
    PTR_DESC_PROC Q; /* fila */
}semaforo;


/* variaveis globais */
PTR_DESC_PROC prim = NULL; /*Vai ser usada como lista circular
                           pro round robin, para manter
                            a ordem dos processos */
PTR_DESC d_esc;


PTR_DESC_PROC procura_proximo_ativo(){
    PTR_DESC_PROC aux = prim;
    if (aux == NULL){
        return NULL;
    }
    /* Round robin */
    while(aux->prox_desc){
        aux = aux->prox_desc;
        if (aux == prim){ /* Se voltou no incio, */
            if (aux->estado == ativo){ /* verifica se esta ativo */
                return aux;                
            }
            return NULL;
        } 
        if (aux->estado == ativo) /* achou ativo */
            return aux;
    }
}

void far volta_dos(){
    disable();
	setvect(8, p_est->int_anterior);
	enable();
    printf("\nVOLTANDO PARA DOS, APERTE QUALQUER TECLA!");
	getch();
	exit(0);
}


/* cria_processo */
void far cria_processo(proc, nome)
void far (*proc)();
char nome[35];
{
    PTR_DESC_PROC processo;
    PTR_DESC_PROC p;

    if((processo=(PTR_DESC_PROC)malloc(sizeof(DESCRITOR_PROC)))==NULL)
    {
        printf("\n\tMemoria Insuficiente para alocacao de descritor\n");
        exit(1);
    }
    strcpy(processo->nome, nome);
    processo->estado = ativo;
    processo->contexto = cria_desc();
    processo->fila_sem = NULL;
    newprocess(proc, processo->contexto);

    if (prim){ /* Se tiver lista ja,*/
        processo->prox_desc = prim;
        
        p = prim;
        while(p->prox_desc != prim){ /*encontra o ultimo da lista */
            p = p->prox_desc;
        }
        p->prox_desc = processo;
    }
    else{ 
        prim = processo;
        prim->prox_desc = prim;
    }

    return;
}


/*escalador*/
void far escalador(){
    /* pego dos slides */
    p_est->p_origem = d_esc;
    p_est->p_destino = prim->contexto;
    p_est->num_vetor = 8;
    _AH = 0x34;
    _AL = 0x00;
    geninterrupt(0x21);
    a.x.bx1 = _BX;
    a.x.es1 = _ES;
    while(1){
        iotransfer();
        disable();
        if((prim = procura_proximo_ativo()) == NULL)
            volta_dos();
        p_est->p_destino = prim->contexto;
        enable();
    }
}

void far dispara_sistema(){
    /* pego dos slides */
    PTR_DESC desc_dispara;

	d_esc = cria_desc();

	desc_dispara = cria_desc();
	
	newprocess(escalador, d_esc);

	transfer(desc_dispara, d_esc);
}

void far termina_processo(){
    PTR_DESC_PROC p_aux, p1;
    disable();
    prim->estado = terminado;
    
    p_aux = procura_proximo_ativo();
    p1 = prim;
    prim = p_aux;
	enable();
    if (prim == NULL){
        volta_dos();
    }
    transfer(p1->contexto,prim->contexto);
}

void far inicia_semaforo(sem, n)
semaforo *sem;
int n;
{
    sem->s = n;
    sem->Q = NULL;
}

void far down(sem)
semaforo *sem;
{
    PTR_DESC_PROC aux, aux2;
    /*printf("\nSEMAFORO\nS==%d",sem->s);
    aux = sem->Q;
    printf("  sem->Q = ");
    while(aux){
        printf("%s -> ", aux->nome);
        aux = aux->fila_sem;
    }*/
    disable();
    if(sem->s > 0){
        sem->s--;
        enable();
    }
    else{
        prim->estado = bloq_P; /* Bloqueia processo atual */
        if (sem->Q == NULL){
            sem->Q = prim; /* insere na fila */
        }
        else{
            aux = sem->Q;
            while(aux->fila_sem){
                aux = aux->fila_sem;
            }
            aux->fila_sem = prim; /* insere na fila */
        }

        aux2 = prim;
        prim = procura_proximo_ativo();
        transfer(aux2->contexto, prim->contexto);
    }
}

void far up(sem)
semaforo *sem;
{
    PTR_DESC_PROC aux;
    disable();
    if (sem->Q == NULL){
        sem->s++;
    }
    else{
        aux = sem->Q; /* pega primeiro da fila*/
        aux->estado = ativo;
        sem->Q = aux->fila_sem; /*passa a fila pro proximo*/
        aux->fila_sem = NULL; /*anula o campo desse processo*/
    }
    enable();
}
