#include "nucleo2.c"


int i1,i2;

int buffer_inicio = 0; /*comeco do buffer circular*/
int buffer_final = 0; /*final do buffer circular*/
int buffer[100]; /*max=100*/
semaforo mutex;
semaforo cheio;
semaforo vazio;

void far produtor(){
    for(;i1<200;i1++){ /*i1 sera usado como int para buffer*/
        down(&vazio); 
        down(&mutex); 
        buffer[buffer_final] = i1;
        printf("\nproduziu %d", i1);
        buffer_final++;
        if (buffer_final == 99)
            buffer_final = 0;

        up(&mutex);
        up(&cheio);
    }
    termina_processo();
}

void far consumidor(){
    int info;

    for(;i2<300;i2++){
        if (i2 > 100){
            down(&cheio);
            down(&mutex);
            info = buffer[buffer_inicio]; /* pega dado do buffer */
            buffer_inicio++;
            if (buffer_inicio == 99)
                buffer_inicio = 0;
            printf("\nconsumiu %d ", info);

            up(&mutex);
            up(&vazio);
        }
    }
    termina_processo();
}



main(){
    inicia_semaforo(&mutex, 1); /* iniciado como aberto pra acesso*/
    inicia_semaforo(&cheio, 0);
    inicia_semaforo(&vazio, 100); /* inicia como maximo do buffer, */
                                        /* pois esta vazio */
    cria_processo(produtor, "produtor"); 
    cria_processo(consumidor, "consumidor");
    
    dispara_sistema();
}