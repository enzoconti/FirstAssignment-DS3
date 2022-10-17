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

void inputDataRecord(DATARECORD* dr){
    int flagSequence[7] ={2,7,8,3,4,5,6}; // This constant array helds the sequence of the fields inputted on CSV
    // the sequece is defined as: idConecta, nomePoPs, nomePais, siglaPais, idPoPsConectado, unidadeMedida, velocidade
    // this fields have the following fieldFlags(defined on the sequence of declaration on struct and used on other functions such as readFile):
    // idConecta = 2, nomePoPs = 7, nomePais = 8, siglaPais = 3, idPoPsConectado = 4, unidadeMedida = 5, velocidade = 6
    for(int i=0;i<7;i++){
        inputDataField(dr,flagSequence[i]);
    }
    // this two fields are not inputted via terminal so we hard code them
    dr->removido = '0';
    dr->encadeamento = -1;
}

void inputDataField(DATARECORD* dr, int fieldFlag){
    char buffStr[MAX_VARSTRINGSIZE];
    scan_quote_string(buffStr);  

    // this switch case puts the buffStr to it corresponding field
    // in each one, there is an if to make the right empty case of each field
    switch(fieldFlag){
        case 2: // idConecta Field
            if(strcmp(buffStr,"") == 0){
                dr->idConecta = -1;
            }else{
                dr->idConecta = atoi(buffStr);
            }
            break;
        case 3: // siglaPais Field
            if(strcmp(buffStr,"") == 0){
                strcpy(dr->siglaPais,"$$");
            }else{
                strcpy(dr->siglaPais,buffStr);
            }
            break;
        case 4: // idPoPsConectado Field
            if(strcmp(buffStr,"") == 0){
                dr->idPoPsConectado = -1;
            }else{
                dr->idPoPsConectado = atoi(buffStr);
            }
            break;
        case 5: //unidadeMedida Field
            if(strcmp(buffStr,"") == 0){
                dr->unidadeMedida = '$';
            }else{
                dr->unidadeMedida = buffStr[0];
            }
            break;
        case 6: // velocidade Field
            if(strcmp(buffStr,"") == 0){
                dr->velocidade = -1;
            }else{
                dr->velocidade = atoi(buffStr);
            }
            break;
        case 7: // nomePoPs Field
            strcpy(dr->nomePoPs,buffStr);
            break;
        case 8: // nomePais Field
            strcpy(dr->nomePais,buffStr);
            break;
    }

}
