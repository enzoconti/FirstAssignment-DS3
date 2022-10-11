#include "../include/IOFile.h"

// this is a simple function to input a filepath into a dynamically allocated string and reallocate it not to waste memory
char* inputFilepath(){
    char* outFilepath;
    outFilepath = malloc(100 * sizeof(char));
    scanf("%s", outFilepath);
    outFilepath = realloc(outFilepath, (strlen(outFilepath) + 1 ) * sizeof(char) );

    return outFilepath;
}

void printRecord(DATARECORD dr){
    printf("removido=%d\n", dr.removido);
    printf("encadeamento=%d\n", dr.encadeamento);
    printf("idConecta=%d\n", dr.idConecta);
    printf("siglaPais=%s\n", dr.siglaPais);
    printf("idPoPsconectaco=%d\n", dr.idPoPsConectado);
    printf("unidadeMedida=%c\n", dr.unidadeMedida);
    printf("velocidade=%d\n", dr.velocidade);
    printf("nomePoPs=%s\n", dr.nomePoPs);
    printf("nomePais=%s\n\n", dr.nomePais);
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