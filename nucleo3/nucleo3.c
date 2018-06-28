/*************************************************
        Projeto de SO2

    Antônio Eugênio Domingues Silva RA: 161021336
    Denis Akira Ise Washio          RA: 161024181
    Lucas Vinhas
 
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


/* MENSAGEM */
typedef struct address{
    int flag;
    char nome_emissor[35];
    char msg[25];
    struct address *ptr_msg;
}mensagem;

typedef mensagem
    *PTR_MENSAGEM;

/* DESCRITOR DE PROCESSOS */
typedef struct desc_p{
    char nome[35];
    enum{ativo, terminado, bloq_P, bloqenv, bloqrec} estado;
    PTR_DESC contexto;
    struct desc_p *prox_desc;
    struct desc_p *fila_sem;
    PTR_MENSAGEM ptr_msg;
    int tam_fila;
    int qtd_msg_fila;

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

PTR_MENSAGEM cria_fila_mensa(max_fila)
int max_fila;
{
    PTR_MENSAGEM aux=NULL;
    return NULL;

}

/* cria_processo */
void far cria_processo(proc, nome, max_fila)
void far (*proc)();
char nome[35];
int max_fila;
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
    processo->tam_fila=max_fila;
    processo->qtd_msg_fila = 0;
    processo->ptr_msg=cria_fila_mensa(max_fila);
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

int far envia(nome_destino, p_info)
char *nome_destino;
char *p_info;
{
    PTR_DESC_PROC aux, p_aux, p1;
    PTR_MENSAGEM m_aux, mensagem;
    disable();
    if (aux = procura_proximo_ativo() == NULL) {
        enable();
        printf("lista vazia");
        return 0;
    } else {
        /* Fica em loop enquanto o prxóximo descritor não for o primeiro da fila. */
        while(aux->prox_desc) {
            printf("está em loop");
            if(strcmp(nome_destino,aux->nome)){
                printf("encontrou destino");
                if(aux->tam_fila==aux->qtd_msg_fila) {
                    printf("Fila cheia");
                    enable();
                    return 1;
                }

                /**
                 * O loop percorre as mensagens e busca uma com flag=0.
                 * Se encontra, troca o flag para 1 e salva o conteúdo da mensagem.
                 */
                m_aux = aux->ptr_msg;
                while(m_aux) {
                    if(m_aux->flag==0){
                        m_aux->flag=1;
                        strcpy(m_aux->nome_emissor, prim->nome);
                        strcpy(m_aux->msg, p_info);
                        aux->qtd_msg_fila++;
                        if(aux->estado==bloqrec) {
                            aux->estado=ativo;
                        }
                        prim->estado=bloqenv;
                        p_aux = procura_proximo_ativo();
                        p1=prim;
                        prim=p_aux;
                        enable();
                        transfer(p1->contexto, prim->contexto);
                        return 2;                   
                    }
                    m_aux = m_aux->ptr_msg;
                }
                
                if((mensagem=(PTR_MENSAGEM)malloc(sizeof(mensagem)))==NULL)
                {
                    printf("\n\tMemoria Insuficiente para alocacao de mensagem\n");
                    exit(1);
                }
                mensagem->flag=1;
                strcpy(mensagem->nome_emissor, prim->nome);
                strcpy(mensagem->msg, p_info);
                m_aux = aux->ptr_msg;
                mensagem->ptr_msg = NULL;
                while(m_aux->ptr_msg) {
                    m_aux = m_aux->ptr_msg;
                }
                m_aux->ptr_msg = mensagem;
                aux->qtd_msg_fila++;
                if(aux->estado==bloqrec) {
                    aux->estado=ativo;
                }
                prim->estado=bloqenv;
                p_aux = procura_proximo_ativo();
                p1=prim;
                prim=p_aux;
                enable();
                transfer(p1->contexto, prim->contexto);
                return 2;

            }
            aux = aux->prox_desc;
        }
        enable();
    }

}

void far recebe(nome_emissor, msg)
char *nome_emissor;
char *msg;
{
    PTR_DESC_PROC p1, p_aux, aux;
    PTR_MENSAGEM m_aux;
    disable();
    if(prim->qtd_msg_fila==prim->tam_fila) {
        prim->estado = bloqrec;
        p_aux = procura_proximo_ativo();
        p1=prim;
        prim=p_aux;
        transfer(p1->contexto, prim->contexto);
    } else {
        m_aux = prim->ptr_msg;
        while(m_aux) {

            if(m_aux->flag==1) {
                strcpy(nome_emissor,m_aux->nome_emissor);
                strcpy(msg, m_aux->msg);
                m_aux->flag=0;
                if (aux = procura_proximo_ativo() == NULL) {
                    enable();
                    printf("lista vazia no recebimento");
                    return;
                } else {
                    /* Fica em loop enquanto o prxóximo descritor não for o primeiro da fila. */
                    while(aux->prox_desc != prim) {
                        if(nome_emissor==aux->nome){
                            printf("encontrou emissor");
                            if(aux->estado==bloqenv){
                                aux->estado = ativo;
                            }
                        }
                    }
                }
                enable();
                return;
            }

            m_aux = m_aux->ptr_msg;
        }
    }
    enable();

}