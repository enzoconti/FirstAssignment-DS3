#include "../include/IOFile.h"

// this is a simple function to input a filepath into a dynamically allocated string and reallocate it not to waste memory
char* inputStr(){
    char* outStr;
    outStr = malloc(100 * sizeof(char));
    scanf("%s", outStr);
    outStr = realloc(outStr, (strlen(outStr) + 1 ) * sizeof(char) );

    return outStr;
}

void printRecord(DATARECORD dr){
    //printf("removido=%d\n", dr.removido);
    //printf("encadeamento=%d\n", dr.encadeamento);
    printf("Identificador do ponto: %d\n", dr.idConecta);
    printf("Nome do ponto: %s\n", dr.nomePoPs);
    printf("Pais de localizacao: %s\n", dr.nomePais);
    printf("Sigla do pais: %s\n", dr.siglaPais);
    printf("Identificador do ponto conectado: %d\n", dr.idPoPsConectado);
    printf("Velocidade de transmissao: %d %cbps\n", dr.velocidade,dr.unidadeMedida);
    printf("\n");
    
}

void printHeader(HEADER h){
    printf("status=%c\n",h.status);
    printf("topoStack=%d\n",h.topoStack);
    printf("proxRRN=%d\n",h.proxRRN);
    printf("nroRegRem=%d\n",h.nroRegRem);
    printf("nroPagDisco=%d\n",h.nroPagDisco);
    printf("qttCompacta=%d\n\n",h.qttCompacta);
}

void printOpenError(){
    printf("Falha no processamento do arquivo\n");
}

void printNoRecordError(){
    printf("Registro inexistente.\n");
}
