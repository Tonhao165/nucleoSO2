#include "nucleo2.h"


int i1,i2;

int buffer_inicio = 0; /*comeco do buffer circular*/
int buffer_final = 0; /*final do buffer circular*/
int buffer[100]; /*max=100*/
semaforo mutex;
semaforo cheio;
semaforo vazio;
FILE *arq_saida;

void far produtor(){
    for(;i1<100;i1++){ /*i1 sera usado como int para buffer*/
        down(&vazio); 
        down(&mutex); 
        buffer[buffer_final] = i1;
        fprintf(arq_saida, "[PRODUTOR] Buffer[%d] = %d\n", buffer_final, i1 );
        printf("[PRODUTOR] Buffer[%d] = %d\n", buffer_final, i1 );
        buffer_final++;
        if (buffer_final == 99)
            buffer_final = 0;

        up(&mutex);
        up(&cheio);
    }
    printf("[PRODUTOR] FIM\n");
    fprintf(arq_saida, "[PRODUTOR] FIM\n");
    termina_processo();
}

void far consumidor(){
    int info;

    for(;i2<150;i2++){
        if (i2 > 50){
            down(&cheio);
            down(&mutex);
            info = buffer[buffer_inicio]; /* pega dado do buffer */
            buffer_inicio++;
            if (buffer_inicio == 99)
                buffer_inicio = 0;
            fprintf(arq_saida, "[CONSUMIDOR] Buffer[%d] = %d\n", buffer_inicio, info);
            printf("[PRODUTOR] Buffer[%d] = %d\n", buffer_final, i1 );

            up(&mutex);
            up(&vazio);
        }
    }
    printf("[CONSUMIDOR] FIM\n");
    fprintf(arq_saida, "[CONSUMIDOR] FIM\n");
    printf("[ARQUIVO TESTE2.TXT GERADO]\n");
    fclose(arq_saida);
    termina_processo();
}



main(){
    /* Abre o arquivo resultado */
    if ((arq_saida = fopen("TESTE2.TXT", "w")) == NULL) {
		printf("\nO arquivo não pode ser aberto");
		exit(1);
	}

    inicia_semaforo(&mutex, 1); /* iniciado como aberto pra acesso*/
    inicia_semaforo(&cheio, 0);
    inicia_semaforo(&vazio, 100); /* inicia como maximo do buffer, */
                                        /* pois esta vazio */
    cria_processo(produtor, "produtor"); 
    cria_processo(consumidor, "consumidor");

    dispara_sistema();
}