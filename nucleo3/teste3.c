#include "nucleo3.h"
FILE *arq_saida;

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
        printf("[EMISSOR] RECEBEU %s DO PROCESSO %s\n", msgrecebida, emissor);
        fprintf(arq_saida, "[EMISSOR] RECEBEU %s DO PROCESSO %s\n", msgrecebida, emissor);
    }
    fprintf(arq_saida, "[EMISSOR] FIM\n");
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
        printf("[RECEPTOR] RECEBEU %s DO PROCESSO %s\n", msg, emissor);
        fprintf(arq_saida, "[RECEPTOR] RECEBEU %s DO PROCESSO %s\n", msg, emissor);
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
    fprintf(arq_saida, "[RECEPTOR] FIM\n");
    fclose(arq_saida);
    termina_processo();
}

/* Programa Principal */
main()
{
    if ((arq_saida = fopen("TESTE3.TXT", "w")) == NULL) {
		printf("\nO arquivo nao pode ser aberto...");
		exit(1);
	}
    /* cria processos*/
    cria_processo(proc_emissor,"proc_em", 10);
    cria_processo(proc_receptor,"proc_rec", 10);
    /* transfere controle para o escalador */
    dispara_sistema();
}