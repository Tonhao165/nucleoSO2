/*************************************************
        Projeto de SO2

    Antônio Eugênio Domingues Silva RA: 161021336

**************************************************/

#include "system.h"
#include <stdio.h>
#include <string.h>

/* pego dos slides */
typedef struct registros {
    unsigned bx1, es1;
} regis;

typedef union k {
    regis x;
    char far *y;
} APONTA_REG_CRIT;

APONTA_REG_CRIT a;

typedef struct address {
    int flag;
    char nome_emissor[35];
    char mensa[25];
    struct address *ptr_msg;

} mensagem;

typedef mensagem *ptr_msg;

/* DESCRITOR DE PROCESSOS */

typedef struct desc_p {
    char nome[35];
    enum { ativo, bloqrec, bloqenv, terminado, bloq_P } estado;
    PTR_DESC contexto;
    /* PTR_MENSAGEM ptr_msg; */
    ptr_msg ptr_msg_p;
    int tam_fila; /* tam. Max fila*/
    int qtde_msg_fila;
    struct desc_p *prox_desc;
    struct desc_p *fila_sem;
} DESCRITOR_PROC;

typedef DESCRITOR_PROC *PTR_DESC_PROC;

/* variaveis globais */
PTR_DESC_PROC prim = NULL; /*Vai ser usada como lista circular
                           pro round robin, para manter
                            a ordem dos processos */
PTR_DESC d_esc;

PTR_DESC_PROC procura_proximo_ativo() {
    PTR_DESC_PROC aux = prim;
    if (aux == NULL) {
        return NULL;
    }
    /* Round robin */
    while (aux->prox_desc) {
        aux = aux->prox_desc;
        if (aux == prim) {              /* Se voltou no incio, */
            if (aux->estado == ativo) { /* verifica se esta ativo */
                return aux;
            }
            return NULL;
        }
        if (aux->estado == ativo){ /* achou ativo */
            return aux;
        }


    }
}

void far volta_dos() {
    disable();
    setvect(8, p_est->int_anterior);
    enable();
    printf("\nVOLTANDO PARA DOS, APERTE QUALQUER TECLA!");
    getch();
    exit(0);
}

void far cria_processo(proc, nome) void far (*proc)();
char nome[35];
int max_fila;
{

    dp_aux->tam_fila = max_fila;
    dp_aux->qtde_msg_fila = 0; /* numero de mensagems recebidas=0  */
    /* cria a fila, todos com flag=0 */
    dp_aux->ptr_msg = cria_fila_mensa(max_fila);

    PTR_DESC_PROC processo;
    PTR_DESC_PROC p;

    if ((processo = (PTR_DESC_PROC)malloc(sizeof(DESCRITOR_PROC))) == NULL) {
        printf("\n\tMemoria Insuficiente para alocacao de descritor\n");
        exit(1);
    }
    strcpy(processo->nome, nome);
    processo->estado = ativo;
    processo->contexto = cria_desc();
    processo->fila_sem = NULL;
    newprocess(proc, processo->contexto);

    if (prim) { /* Se tiver lista ja,*/
        processo->prox_desc = prim;

        p = prim;
        while (p->prox_desc != prim) { /*encontra o ultimo da lista */
            p = p->prox_desc;
        }
        p->prox_desc = processo;
    } else {
        prim = processo;
        prim->prox_desc = prim;
    }

    return;
}

/*escalador*/
void far escalador() {
    /* pego dos slides */
    p_est->p_origem = d_esc;
    p_est->p_destino = prim->contexto;
    p_est->num_vetor = 8;
    _AH = 0x34;
    _AL = 0x00;
    geninterrupt(0x21);
    a.x.bx1 = _BX;
    a.x.es1 = _ES;
    while (1) {
        iotransfer();
        disable();
        if ((prim = procura_proximo_ativo()) == NULL)
            volta_dos();
        p_est->p_destino = prim->contexto;
        enable();
    }
}

void far dispara_sistema() {
    /* pego dos slides */
    PTR_DESC desc_dispara;

    d_esc = cria_desc();

    desc_dispara = cria_desc();

    newprocess(escalador, d_esc);

    transfer(desc_dispara, d_esc);
}

void far termina_processo() {
    PTR_DESC_PROC p_aux, p1;
    disable();
    prim->estado = terminado;

    p_aux = procura_proximo_ativo();
    p1 = prim;
    prim = p_aux;
    enable();
    if (prim == NULL) {
        volta_dos();
    }
    transfer(p1->contexto, prim->contexto);
}

void far proc_emissor() { // faltou colocar o far do void que nao ta funcionando
    int i;
    while (1) {
        strcpy(msg, ...); /* produz uma mensagem */
        i = envia("proc_rec", msg);
        if (i == 0) {
            printf("Não achou destino !Abortar ");
            termina_processo();
        } else if (i == 1) /* fila do destino cheia */
        {
            while (i == 1) {
                i = envia("proc_rec", msg)
            };
        };
    }
}

void far proc_receptor() {
    char emissor[35];
    char msg[25];
    while (1) {
        recebe(emissor, msg);
        printf("o processo destino processo % s\n", msg, emissor);
    }
}


int verify(node* lista,int valor)
{
	node q=(*lista);
	do
	{
        if(q->valor==valor)
            return 1;
		q=q->proximo;
	}while(q!=(*lista));

    return 0;
}
int far envia(nome_destino, p_info) char *nome_destino;
char *p_info;
{

    //- desabilita interrupções;
    disable();
    //- procura descritor do destino da mensagem na fila dos prontos;
    PTR_DESC_PROC q=prim ;
    unsigned int result=0;
	do
	{
        if(strcmp(nome_destino,q->nome))
            result=1;
		q=q->proximo;
	}while(q!=(*lista));

    //- se não achou, habilita as interrupções retorna 0; /*fracasso: não achou
    if(result==0){
        enable();
        return 0;
    }

    //- se fila de mensagens do destino estiver cheia,habilita as interrupções e
    //retorna 1; /* fracasso: fila cheia */
    if(p_info->qtde_msg_fila==p_info->tam_fila){
        p_info->estado = ativo;
        return 1;
    }

    //- localiza uma mensagem vazia (flag==0);
    ptr_msg q=nome_destino->ptr_msg_p ;
    unsigned int result=0;
	do
	{
        if(q->flag==0){
            result=1;
            break;
        }
		q=q->proximo;
	}while(q!=(*lista));

 //!- completa a mensagem
    //- flag=1
        q->flag=1;

    //- copia nome_emissor
    strcpy(nome_emissor,prim->nome);

    //- copia mensagem
    strcpy(mensa, p_info);

    //- incrementa a qtde_msg_fila;
    p_info->qtde_msg_fila++; /* TODO Nao sei se tem isso no turboC */

    //- se estado do destino == "bloqrec", muda-o para "ativo";
    if(nome_destino->estado == bloqrec){
        nome_destino->estado = ativo
    }

    //- muda estado do processo atual para “bloqenv”;
    p_info->estado=bloqenv

    //- acha próximo processo pronto (p_aux=procura_proximo_ativo());
    PTR_DESC_PROC p_aux, p1;
    p_aux = procura_proximo_ativo();

    //- p1=prim;
    p1 = prim;
    //-prim=p_aux
    prim = p_aux;
    //- transfer(p1->contexto,prim->contexto);
    transfer(p1->contexto,prim->contexto);
    //- retorna 2; /*sucesso*/
    return 2;
}
void far recebe(nome_emissor, msg)
char *nome_emissor;
char *msg;
{
//- desabilita as interrupções;
    p_info->estado = bloqenv
//- se fila de msg estiver cheia então:
    if(p_info->qtde_msg_fila==p_info->tam_fila){
        //- muda estado do processo atual para “bloqrec”;
        p_info->estado = bloqrec;
        //- acha próximo processo pronto(p_aux=procura_próximo_ativo());
        (p_aux=procura_próximo_ativo());
        //- p1=prim;
         p1=prim;
        //- prim=p_aux;
        - prim=p_aux;
        //- transfer(p1->contexto,prim->contexto);
         transfer(p1->contexto,prim->contexto);
    }

/* existe mensagem na fila */
//- desabilita as interrupções;
    p_info->estado = bloqenv
//- localiza a primeira mensagem cheia (flag== 1);
    ptr_msg q=nome_destino->ptr_msg_p ;
    unsigned int result=0;
	do
	{
        if(q->flag==1){
            result=1;
            break;
        }
		q=q->proximo;
	}while(q!=(*lista));
//- copia nome do emissor ;
    strcpy(nome_emissor,prim->nome);
//!- copia informação da mensagem para msg;
//- decrementa qtde_msg_fila;
    p_info->qtde_msg_fila--; /* TODO Nao sei se tem isso no turboC */
//- faz flag=0;
        q->flag=0;
//!- localiza descritor do emissor;
//- se estado do emissor == “bloqenv”, muda-o para “ativo”;
    if(nome_emissor->estado == bloqenv){
        nome_emissor->estado = ativo
    }
//- habilita as interrupções;
    p_info->estado = ativo
}
//! Problemas
// nao entendi oque e[ esse completa mensagem
// e acho que vai ter que dar malloc para salvar essa mensagem









