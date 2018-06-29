/*************************************************
        Projeto de SO2

    Antônio Eugênio Domingues Silva RA: 161021336
    Denis Akira Ise Washio          RA:161024181
 
**************************************************/


#include <nucleo3.h>

int i1, i2, i3;

void far proc_emissor()
{ 
    int i;
    char msg[30];
    while (1){
        printf("enviando");
        strcpy(msg, "Mensagem"); /* produz uma mensagem */
        i = envia("proc_rec",msg);
        if (i==0) {
            printf("Não achou destino! Abortar");
            termina_processo();
        }
        else if (i ==1)/* fila do destino cheia */
        { 
            while (i == 1) { 
                i = envia("proc_rec",msg);
            }
        }
        else if (i==2) {
            printf("Sucesso");
        }
    }
}

void far proc_receptor()
{
    char emissor[35];
    char msg[25];
    while(1)
    {
        recebe(emissor,msg);
        printf("O processo destino recebeu %s do processo %s\n", msg, emissor);
    }
}

main()
{
    /* cria fila dos prontos vazia */
    /*inicia_fila_prontos();*/
    /* cria processos*/
    cria_processo(proc_emissor,"proc_em",10);
    cria_processo(proc_receptor,"proc_rec",10);
    /* transfere controle para o escalador */
    dispara_sistema();
}