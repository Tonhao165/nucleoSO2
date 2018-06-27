#include "nucleo1.h"

int i1,i2,i3;

void far processo1(){
    for(;i1<4999;i1++){
        printf("1");
    }
    termina_processo();
}

void far processo2(){
    for(;i2<4999;i2++){
        printf("2");
    }
    termina_processo();
}

void far processo3(){
    for (;i3<5999;i3++){
        printf("3");
    }
    termina_processo();
}


main(){
    cria_processo(processo1, "proc1");
    cria_processo(processo2, "proc2");
    cria_processo(processo3, "proc3");
    dispara_sistema();
}