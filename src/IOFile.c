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
    printf("nomePais=%s\n", dr.nomePais);
}

char *zstrtok(char *str, const char *delim) {
    static char *s_str = NULL;   /* var to store last address */
    char *p;

    if (delim == NULL || (str == NULL && s_str == NULL)){
        return NULL;
    }

    if (str == NULL){
        str = s_str;
    }

    /* if delim is not contained in str, return str */
    if ((p = strstr(str,delim)) == NULL) {
        s_str = NULL;
        return str;
    }

    /* 
    * check for consecutive delimiters
    * if first char is delim, return delim
    */
    if (str[0] == delim[0]) {
        s_str++;
        return (char *)delim;
    }

    /* terminate the string */
    *p = '\0';

    /* save the rest of the string */
    if ((p+1) != NULL) {
        s_str = (p + 1);
    } else {
        s_str = NULL;
    }

        return str;
}

void printOpenError(){
    printf("Falha no processamento do arquivo\n");
}