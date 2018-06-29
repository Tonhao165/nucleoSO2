/*************************************************
        Projeto de SO2

    Antônio Eugênio Domingues Silva RA: 161021336
    Denis Akira Ise Washio          RA: 161024181
    Lucas Vinhas
 
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

typedef mensagem *PTR_MENSAGEM;

/* DESCRITOR DE PROCESSOS */

typedef struct desc_p {
    char nome[35];
    enum { ativo, bloqrec, bloqenv, terminado, bloq_P } estado;
    PTR_DESC contexto;
    PTR_MENSAGEM ptr_msg;
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

PTR_MENSAGEM far cria_mensagem(){ /*cria msg com flag =0*/
    PTR_MENSAGEM auxmsg;
    if ((auxmsg = (PTR_MENSAGEM)malloc(sizeof(mensagem))) == NULL) {
        printf("\n\tMemoria Insuficiente para alocacao de mensagem\n");
        exit(1);
    }
    auxmsg->flag = 0;
    auxmsg->ptr_msg = NULL;
    return auxmsg;
}

PTR_MENSAGEM far cria_fila_mensa(max_fila)
int max_fila;
{
    PTR_MENSAGEM auxmsg, primeiramsg, mensagem;
    int i;
    
    primeiramsg = cria_mensagem();
    auxmsg = primeiramsg;
    for(i=0;i<max_fila-1;i++){
        auxmsg->ptr_msg = cria_mensagem();
        auxmsg = auxmsg->ptr_msg;
    }

    return primeiramsg;
}

void far cria_processo(proc, nome, max_fila) 

void far (*proc)();
char nome[35];
int max_fila;
{
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
    processo->tam_fila = max_fila;
    processo->qtde_msg_fila = 0;
    processo->ptr_msg = cria_fila_mensa(max_fila); /* inicia fila  de mensagems*/
    newprocess(proc, processo->contexto);

    if (prim) { /* Se tiver lista ja,*/
        processo->prox_desc = prim;

        p = prim;
        while(p->prox_desc != prim) { /*encontra o ultimo da lista */
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
        /*printf("\nPROCESSO ATUAL  == %s", prim->nome);*/
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

int far envia(nome_destino, p_info)
char *nome_destino;
char *p_info;
{
    PTR_DESC_PROC q=prim ;
    PTR_DESC_PROC p_aux, p1;
    PTR_MENSAGEM msg;
    /* desabilita interrupções;*/
    disable();
    /* procura descritor do destino da mensagem na fila dos prontos;*/
    
	while(q){
        if(strcmp(nome_destino,q->nome) == 0){
            break;
        }
        q = q->prox_desc;
        if (q == prim){ /* se não achou, habilita as interrupções retorna 0; fracasso: não achou */
            enable();
            return 0;
        }
    }
    if(q->qtde_msg_fila == q->tam_fila){
        enable();
        return 1;
    }
    /* localiza uma mensagem vazia (flag==0);*/
    msg = q->ptr_msg;
    while(msg){
        if (msg->flag == 0)
            break;
        msg = msg->ptr_msg;
    }
    /*- completa a mensagem*/
    strcpy(msg->mensa, p_info);
    /* copia nome_emissor*/
    strcpy(msg->nome_emissor,prim->nome);
    /* flag=1*/
    msg->flag = 1;
    /* incrementa a qtde_msg_fila;*/
    q->qtde_msg_fila++;
    /* se estado do destino == "bloqrec", muda-o para "ativo";*/
    if(q->estado == bloqrec){
        q->estado = ativo;
    }
    /* muda estado do processo atual para “bloqenv”;*/
    prim->estado=bloqenv;
    /* acha próximo processo pronto (p_aux=procura_proximo_ativo());*/
    p_aux = procura_proximo_ativo();
    /* p1=prim;*/
    p1 = prim;
    /*prim=p_aux*/
    prim = p_aux;
    /* transfer(p1->contexto,prim->contexto);*/
    transfer(p1->contexto,prim->contexto);
    /* retorna 2; /*sucesso */
    return 2;
}

void far recebe(nome_emissor, msg)
char *nome_emissor;
char *msg;
{
    PTR_DESC_PROC p_aux, p1;
    PTR_MENSAGEM p;
    PTR_DESC_PROC desc_emissor = prim;

    /* desabilita as interrupções;*/
    disable();
    /* se fila de msg estiver vazia então:*/
    if(prim->qtde_msg_fila == 0){
        /* muda estado do processo atual para “bloqrec”;*/
        prim->estado = bloqrec;
        /* acha próximo processo pronto(p_aux=procura_próximo_ativo());*/
        
        p_aux = procura_proximo_ativo();
        /* p1=prim;*/
        p1=prim;
        /* prim=p_aux;*/
        prim=p_aux;
        /* transfer(p1->contexto,prim->contexto);*/
        transfer(p1->contexto,prim->contexto);
    }

    /* localiza a primeira mensagem cheia (flag== 1);*/
    
    p = prim->ptr_msg;
    while(p){
        if (p->flag == 1)
            break;
        p = p->ptr_msg;
    }


    /* copia nome do emissor ;*/
    strcpy(nome_emissor, p->nome_emissor);
    /* copia informação da mensagem para msg;*/
    strcpy(msg, p->mensa);
    /* decrementa qtde_msg_fila;*/
    prim->qtde_msg_fila--; /* TODO Nao sei se tem isso no turboC */
    /* faz flag=0;*/
    p->flag=0;
    /* localiza descritor do emissor;*/
    while(desc_emissor){
        if (strcmp(desc_emissor->nome,nome_emissor) == 0)
            break;
        desc_emissor = desc_emissor->prox_desc;
    }
    
    /* se estado do emissor == “bloqenv”, muda-o para “ativo”;*/
    if(desc_emissor->estado == bloqenv){
        desc_emissor->estado = ativo;
    }
    /* habilita as interrupções;*/
    enable();
}