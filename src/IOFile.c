#include "../include/IOFile.h"
// this file has every formatted input or output

// this is a simple function to input a filepath into a dynamically allocated string and reallocate it not to waste memory
char* inputStr(){
    char* outStr;
    outStr = malloc(100 * sizeof(char));
    scanf("%s", outStr);
    outStr = realloc(outStr, (strlen(outStr) + 1 ) * sizeof(char) );

    return outStr;
}

// this function prints an inputted record dr field by field
// it only prints a field after checking if it is not empty/trash
void printRecord(DATARECORD dr){
    if(dr.idConecta != -1) printf("Identificador do ponto: %d\n", dr.idConecta);
    if(strlen(dr.nomePoPs) != 0) printf("Nome do ponto: %s\n", dr.nomePoPs);
    if(strlen(dr.nomePais) != 0) printf("Pais de localizacao: %s\n", dr.nomePais);
    if(dr.siglaPais[0] != '$')printf("Sigla do pais: %s\n", dr.siglaPais);
    if(dr.idPoPsConectado != -1) printf("Identificador do ponto conectado: %d\n", dr.idPoPsConectado);
    if(dr.velocidade != -1) printf("Velocidade de transmissao: %d %cbps\n", dr.velocidade,dr.unidadeMedida);
    printf("\n");
    
}

// this function prints an inputted header h field by field
// there is no conditionals bc header is never empty
void printHeader(HEADER h){
    printf("status=%c\n",h.status);
    printf("topoStack=%d\n",h.topoStack);
    printf("proxRRN=%d\n",h.proxRRN);
    printf("nroRegRem=%d\n",h.nroRegRem);
    printf("nroPagDisco=%d\n",h.nroPagDisco);
    printf("qttCompacta=%d\n\n",h.qttCompacta);
}

void printOpenError(){
    printf("Falha no processamento do arquivo.\n");
}

void printNoRecordError(){
    printf("Registro inexistente.\n\n");
}

// this function inputs a data record dr from keyboard by inputting field by field
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

// this function inputs a data field to datarecord dr according to the fieldFlag it receives
// it gets a field between quotes and removes it(scan_quote_string function)
// and then it puts either the inputted value or an empty value on dr.field
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
                dr->siglaPais[0] = buffStr[0];
                dr->siglaPais[1] = buffStr[1];
                dr->siglaPais[2] = '\0';
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
