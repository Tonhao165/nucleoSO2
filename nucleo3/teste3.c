#include "nucleo3.h"

void far proc_emissor()
{ 
    int i;
    int k;
    char msg[35];

    char emissor[35];
    char msgrecebida[35];

    for(k=0;k<100;k++){
        /* produz mesnagem*/
        sprintf(msg, "MSG %d", k);
        i = envia("proc_rec",msg);
        if (i==0) {
            printf("Não achou destino! Abortar");
            termina_processo();
        }
        else if (i ==1){
            /* fila do destino cheia */ 
            while (i == 1) { 
                printf("\nLoop enviando mensagem!");
                i = envia("proc_rec",msg);
            };
        }

        recebe(emissor, msgrecebida);
        printf("\nEMISSOR recebeu %s do processo %s",msgrecebida,emissor);
    }
    termina_processo();
}

void far proc_receptor()
{
    char emissor[35];
    char msg[25];
    int i,k;
    
    for(i=0;i<100;i++)
    {
        recebe(emissor,msg);
        printf("\nRECEPTOR recebeu %s do processo %s",msg,emissor);
        k = envia("proc_em",msg);
        if (k==0) {
            printf("Não achou destino! Abortar");
            termina_processo();
        }
        else if (k ==1){
            /* fila do destino cheia */ 
            while (k == 1) { 
                printf("\nLoop enviando mensagem!");
                k = envia("proc_rec",msg);
            };
        }
    }
    termina_processo();
}

/* Programa Principal */
main()
{
    /* cria processos*/
    cria_processo(proc_emissor,"proc_em", 10);
    cria_processo(proc_receptor,"proc_rec", 10);
    /* transfere controle para o escalador */
    dispara_sistema();
}