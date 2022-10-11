#include "../include/fileRoutines.h"

// this function will read the CSV from CSVfp line by line creating a temporary data record on RAM to be stored on the binary file binfp
void readCSV_writeBin(FILE *CSVfp, FILE *binfp, HEADER *head){
    char buffTrash[LINESIZE];
    int countRecords=0; // this counter will count how many lines there are in the csv(= number of records in the bin file) to be used on the header
    DATARECORD tempData;

    fgets(buffTrash,LINESIZE,CSVfp); // this simply reads the first line of the csv that consists of the sequence of fields
                                   // as the sequence is constant in this project, this is not used by the program (it's just a default for csv files)

    while(readCSVRecord(CSVfp, &tempData) != 0){
        // this fields are not inputted in the csv so we hard-code them here
        tempData.removido = '0';
        tempData.encadeamento = -1;

        // with the temporary data record set-up, we write it onto the binary file
        printf("\ngoing into writeDataRecord with the record set as:\n");
        printRecord(tempData);
        writeDataRecord(binfp, &tempData);
        countRecords++;
    }

    // now that we have the whole information about the data record we can update our header
    head->nroPagDisco = countRecords*DATARECORDSIZE / CLUSTERSIZE + 1; // 1 from header cluster
    if(countRecords*DATARECORDSIZE % CLUSTERSIZE != 0) head->nroPagDisco++;
    //head->nroPagDisco=0;
    head->proxRRN = countRecords ;
    head->status = '1';

    printf("\nthe header has been set as:\n");
    printHeader(*head);
}

int readCSVRecord(FILE* CSVfp, DATARECORD* dr){
    int flagSequence[7] = {2,7,8,3,4,5,6}; // This constant array helds the sequence of the fields inputted on CSV
    // the sequece is defined as: idConecta, nomePoPs, nomePais, siglaPais, idPoPsConectado, unidadeMedida, velocidade
    // this fields have the following fieldFlags(defined on the sequence of declaration on struct and used on other functions such as readFile):
    // idConecta = 2, nomePoPs = 7, nomePais = 8, siglaPais = 3, idPoPsConectado = 4, unidadeMedida = 5, velocidade = 6
    

    int endFlag;
    // this loop will simply get every single field
    for(int i=0;i<7;i++){
        endFlag = readCSVField(CSVfp,dr,flagSequence[i]);
        if(endFlag == 0) return 0;
    }

    return 1;
}

int readCSVField(FILE *CSVfp, DATARECORD*dr, int fieldFlag){

    char buffChar, nullFlag=1, buffStr[MAX_VARSTRINGSIZE];

    // first, this loop reads the file until an invalid character and puts it onto a string
    int i=0;
    while(1){
        nullFlag = fread(&buffChar,sizeof(char),1,CSVfp);
        if(nullFlag == 0) return 0;

        if(isValid(CSVfp,buffChar) == 0){
            break;
        }

        buffStr[i] = buffChar;
        i++;
    }
    buffStr[i] = '\0';
    
    // this switch case puts the read value onto the right field
    // there are some ifs that check if the buffStr has only '\0'
    // that means an empty field, and the ifs deal with that to put the empty flag on every field
    switch(fieldFlag){
        case 2: // idConecta field (fixed size as an int)
            if(buffStr[0] == '\0') {
                dr->idConecta = -1;
                break;
                }
            dr->idConecta = atoi(buffStr);
            break;
        case 3: // siglaPais field (fixed size as static array of lenght 3)
            if(buffStr[0] == '\0'){
                dr->siglaPais[0] = '$';
                dr->siglaPais[1] = '$';
                dr->siglaPais[2] = '\0';
                break;
            }
            strcpy(dr->siglaPais,buffStr);
            dr->siglaPais[2] = '\0';
            break;

        case 4: // idPoPsConectado field(fixed size as an int)
            if(buffStr[0] == '\0'){
                dr->idPoPsConectado = -1;
                break;
            }
            dr->idPoPsConectado = atoi(buffStr);
            break;

        case 5: // unidadeMedida field(fixed size as a char)
            if(buffStr[0] == '\0'){
                dr->unidadeMedida = '$';
                break;
            }
            dr->unidadeMedida = buffStr[0];
            break;

        case 6: // velocidade field(fixed size as an int)
            if(buffStr[0] == '\0'){
                dr->velocidade = -1;
                break;
            }
            dr->velocidade = atoi(buffStr);
            break;

        // in the variable size cases there is no if bc the empty is the '\0' itself, so we put the buffStr anyway
        case 7: // nomePoPs field(variable size string)
            strcpy(dr->nomePoPs , buffStr);
            break;
        case 8: // nomePais field(variable size string)
            strcpy(dr->nomePais , buffStr);
            break;
    }

    return 1;
}

int isValid(FILE *fp,char c){
    char nextChar;
    if(c == '\n' || c == ','){
        return 0;
    }

    else if(c == '\r'){
        nextChar = fgetc(fp);
        if(nextChar == '\n'){
            return 0;
        }else{
            fseek(fp,-1,SEEK_CUR);
            return 0;
        }
    }

    return 1;
}


void readFile(FILE* fp){
    int countRecords=0;
    DATARECORD tempData;
    // HEADER tempHeader;

    // readHeader(fp, &tempHeader); // AINDA FALTA IMPLEMENTAR

    while( readDataRecord(fp, &tempData) != 0){ // this reads a Record from fp and puts its data into tempData
        // printData(tempData);     // AINDA FALTA IMPLEMENTAR      
        countRecords++;
    }

    return ;

}
// this will read a header and load it into RAM
HEADER readHeader(FILE* fp){
    HEADER outHeader;
    for(int i=0;i<6;i++){
        readHeaderField(fp,&outHeader,i); // basically reading each field
    }
}
// this reads one field of the header depending on fieldFlag
// it also sets the fp ready to later data record reading by fseeking it until the end of the cluster
void readHeaderField(FILE* fp, HEADER* outh, int fieldFlag){
    switch(fieldFlag){
        case 0: // status field
            fread(&(outh->status), sizeof(char),1,fp);
            break;
        case 1: // topoStack fiel
            fread(&(outh->topoStack), sizeof(int),1,fp);
            break;
        case 2: // proxRRN field
            fread(&(outh->proxRRN), sizeof(int),1,fp);
            break;
        case 3: // nroRegRem field
            fread(&(outh->nroRegRem),sizeof(int),1,fp);
            break;
        case 4: // nroPagDisco field
            fread(&(outh->nroPagDisco),sizeof(int),1,fp);
            break;
        case 5: // qttCompacta field
            fread(&(outh->qttCompacta),sizeof(int),1,fp);
            break;
    }

    fseek(fp,CLUSTERSIZE - HEADERSIZE, SEEK_CUR); // this jumps the trash so that the readRecord that may follow can start by the data records
}

// this will read an entire data record and put it into the outData instance
// isso lerá um registro inteiro e o colocará na instância outData
int readDataRecord(FILE *fp, DATARECORD* outData){

    int countFieldsSize = 0, buff=0, fieldFlag = 0; // the fieldFlag indicates which f the 5 possible fields we are reading - to know its size and where to put it onto PERSON struct
    //o fieldFlag indica quais dos 5 campos possíveis estamos lendo - para saber seu tamanho e onde colocá-lo na estrutura PERSON

    while(countFieldsSize < DATARECORDSIZE){
        buff = readDataField(fp, fieldFlag, outData);

        if(buff == 0){ // this indicates the file has ended
            //isso indica que o arquivo terminou
            return 0;
        }else{
            countFieldsSize+=buff; // we accumulate the non-zero buffer to know how much of the record we have already read
        }//acumulamos o buffer diferente de zero para saber quanto do registrador já lemos

        fieldFlag = (fieldFlag + 1)%9; // this makes the fieldFlag loop through 0 -> 1 -> 2 -> 3 -> ... -> 8 -> 0 ...
        //isso faz com que o fieldFlag passe por 0 -> 1 -> 2 -> 3 -> .. -> 8 -> 0 ...
    }

    return 1; // if the Record has ended and the file still not, we return 1 to indicate to readFile that it can read another record
    //se o registro terminou e o arquivo ainda não, retornamos 1 para indicar ao readFile que ele pode ler outro registro
}

// this function will read one of the 9 possible data fields according to the fieldFlag and puts it onto a field of outData
int readDataField(FILE* fp, int fieldFlag, DATARECORD* outData){
    
    int outSizeCounter=0;
    int nullFlag=1; // this flag will indicate when fread fails(meaning the file has ended)
    int i=0;
    char buffChar = 'S';

    switch(fieldFlag){
        case 0: // removido Field
            nullFlag = fread(&(outData->removido),1,1,fp);
            outSizeCounter = 1;
            break;

        case 1: // encadeamento Field
            nullFlag = fread(&(outData->encadeamento),sizeof(int),1,fp);
            outSizeCounter = sizeof(int);
            break;

        case 2: // idConecta Field
            nullFlag = fread(&(outData->idConecta),sizeof(int),1,fp);
            outSizeCounter = sizeof(int);
            break;

        case 3: // siglaPais Field
            nullFlag = fread(&(outData->siglaPais),2,1,fp);
            outData->siglaPais[2] = '\0';
            outSizeCounter = 2;
            break;

        case 4: // idPoPsConectdo Field
            nullFlag = fread(&(outData->idPoPsConectado),sizeof(int),1,fp);
            outSizeCounter = sizeof(int);
            break;
        case 5: // unidade Medida Field
            nullFlag = fread(&(outData->unidadeMedida),sizeof(char),1,fp);
            outSizeCounter = sizeof(char);
            break;
        
        case 6: // velocidade Field
            nullFlag = fread(&(outData->idPoPsConectado),sizeof(int),1,fp);
            outSizeCounter = sizeof(int);
            break;
        
        case 7: // nomePoPs Field
            i=0;
            while(1){
                nullFlag = fscanf(fp, "%c", &buffChar);
                if(nullFlag == 0) break;
            

                if(buffChar != '|'){
                    outData->nomePoPs[i] = buffChar;
                    i++;
                }else{
                    outData->nomePoPs[i] = '\0';
                    i++;
                    break;
                }

            }
            break;
        
        case 8: // nomePais Field
            i=0;
            while(1){
                nullFlag = fscanf(fp, "%c", &buffChar);
                if(nullFlag == 0) break;
            

                if(buffChar != '|'){
                    outData->nomePais[i] = buffChar;
                    i++;
                }else{
                    outData->nomePais[i] = '\0';
                    i++;
                    break;
                }

            }
            break;
    }

    if(nullFlag == 0) return 0; // this indicates that the file has ended

    return outSizeCounter;
}


// FUNCOES DO CODIGO ANTIGO ----- AINDA TEM QUE SER ADAPTADAS
/*
void RRNread(char* filepath, int RRN){
    int byte_offset = RRN * DATARECORDSIZE, fseekFlag=0; // this calculates the byte_offset to find the RRNth record
    PERSON personRecord;


    FILE* fp = fopen(filepath, "rb"); // tries to open a binary file for read
    if(fp == NULL){
        printErrorFileOpening(); // print error if unsuceeded
        //printf("Error");
        return;
    }

    fseekFlag = fseek(fp, byte_offset, SEEK_SET); // this sets the file pointer to the desired position to read the record

    if(fseekFlag != 0){
        printErrorSeek();
        return;
    }

    
    readRecord(fp, &personRecord);

    printPerson(personRecord);

    fclose(fp);

    return ;
}
*/

// this function writes the entire header record by writing it field by field 
// and later adding the trash to make the header occupy exactly one cluster
void writeHeaderRecord(FILE *fp, HEADER* hr){
    for(int i=0;i<6;i++){
        writeHeaderField(fp, hr, i);
    }

    // now we need to write the entire trash with const size to make the header occupy one whole cluster
    char* trash;
    int trashsize = CLUSTERSIZE - HEADERSIZE;
    trash = malloc( trashsize * sizeof(char));
    for(int i=0;i<trashsize;i++){
        trash[i] = '$';
    }

    fwrite(trash, trashsize * sizeof(char), 1, fp);
}

// writes one field of the header depending on fieldFlag
void writeHeaderField(FILE *fp, HEADER* hr, int fieldFlag){
    switch(fieldFlag){
        case 0: // status field
            fwrite(&(hr->status), sizeof(char),1,fp);
            break;
        case 1: // topoStack field
            fwrite(&(hr->topoStack), sizeof(int),1,fp);
            break;
        case 2: // proxRRN field
            fwrite(&(hr->proxRRN), sizeof(int),1,fp);
            break;
        case 3: // nroRegRem field
            fwrite(&(hr->nroRegRem), sizeof(int),1,fp);
            break;
        case 4: // nroPagDisco field
            fwrite(&(hr->nroPagDisco), sizeof(int),1,fp);
            break;
        case 5: // qttCompacta field
            fwrite(&(hr->qttCompacta), sizeof(int),1,fp);
            break;
    }

    return ;
}

// this writes an entire data record by writing each field
// and adding the trash after the variable size fields
void writeDataRecord(FILE *fp, DATARECORD* dr){
    int sizeWritten=0;
    // simply writes the 9 different field options
    for(int i=0;i<9;i++){
        sizeWritten += writeDataField(fp,dr,i);
    }
    int trashsize = DATARECORDSIZE - sizeWritten;
    char *trash;
    trash = malloc((trashsize+1) * sizeof(char));
    for(int i=0;i<=trashsize;i++){
        trash[i] = '$';
    }

    fwrite(trash, trashsize * sizeof(char), 1, fp); // writes the remaining trash onto the file
}

// this funciton writes a data field based on fieldFlag
// it also returns the size that has been written so that the upper function
// can deal with how much trash it must write
int writeDataField(FILE *fp, DATARECORD* dr,int fieldFlag){
    int sizeWritten=0,i=0;
    char delim = '|'; // this is set as the delimiter and will be used for both variable size fields
    switch(fieldFlag){
        case 0: // removido Field
            fwrite(&(dr->removido), sizeof(char),1, fp);
            sizeWritten = sizeof(char);
            break;

        case 1: // encadeamento Field
            fwrite(&(dr->encadeamento), sizeof(int),1, fp);
            sizeWritten = sizeof(int);
            break;

        case 2: // idConecta Field
            fwrite(&(dr->idConecta), sizeof(int), 1, fp);
            sizeWritten = sizeof(int);
            break;

        case 3: // siglaPais Field
            fwrite(&(dr->siglaPais),sizeof(char),2,fp); // writes 2 chars bc we do not write '\0' 
            sizeWritten = 2* sizeof(char);
            break;

        case 4: // idPoPsConectdo Field
            fwrite(&(dr->idPoPsConectado), sizeof(int),1,fp);
            sizeWritten = sizeof(int);
            break;
        case 5: // unidade Medida Field
            fwrite(&(dr->unidadeMedida), sizeof(char), 1, fp);
            sizeWritten = sizeof(char);
            break;
        
        case 6: // velocidade Field
            fwrite(&(dr->velocidade), sizeof(int),1, fp);
            sizeWritten = sizeof(int);
            break;
        
        case 7: // nomePoPs Field
            i=0;
            while(i < strlen(dr->nomePoPs)){
                fwrite(&(dr->nomePoPs[i]),sizeof(char),1,fp);
                sizeWritten+=sizeof(char);
                i++;
            }
            fwrite(&delim,sizeof(char),1,fp); // after the variable size field we write the delimiter
            sizeWritten+=sizeof(char); // +1 byte written for '|'
            break;
        
        case 8: // nomePais Field
            i=0;
            while(i < strlen(dr->nomePais)){
                fwrite(&(dr->nomePais[i]),sizeof(char),1,fp);
                sizeWritten+=sizeof(char);
                i++;
            }
            fwrite(&delim,sizeof(char),1,fp); // after the variable size field we write the delimiter
            sizeWritten+=sizeof(char); // +1 byte written for '|'
            break;
    }

    return sizeWritten;
    
}

DATARECORD searchFile(FILE* fp,char* searchedField, char* searchKey){
    int fieldFlag;
    fieldFlag = getFlag_fromDataField(searchedField);
    
    // bc we have strong types, we declare the two possible types of keys (but we will use only one)
    int IntegerKey;
    char StrKey[MAX_VARSTRINGSIZE];
    int isKeyInt; // and this flag will hold wheter the key is an integer or not

    switch(fieldFlag){
        case 2: // idConecta Field
            IntegerKey = atoi(searchKey);
            isKeyInt = 1;
            break;
        case 3: // siglaPais Field
            strcpy(StrKey,searchKey);
            isKeyInt = 0;
            break;
        case 4: // idPoPsConectado Field
            IntegerKey = atoi(searchKey);
            isKeyInt = 1;
            break;
        case 5: //unidadeMedida Field
            strcpy(StrKey,searchKey);
            isKeyInt = 0;
            break;
        case 6: // velocidade Field
            IntegerKey = atoi(searchKey);
            isKeyInt = 1;
            break;
        case 7: // nomePoPs Field
            strcpy(StrKey,searchKey);
            isKeyInt = 0;
            break;
        case 8: // nomePais Field
            strcpy(StrKey,searchKey);
            isKeyInt = 0;
            break;
    }

    if(isKeyInt == 1){
        searchIntOnFile(fp, fieldFlag, IntegerKey);
    }else{
        searchStrOnFile(fp, fieldFlag, StrKey);
    }

}


static int getFlag_fromDataField(char* searchedField){
    if(strcmp(searchedField, "idConecta") == 0){
        return 2;
    }
    else if(strcmp(searchedField, "siglaPais") == 0){
        return 3;
    }
    else if(strcmp(searchedField, "idPoPsConectado") == 0){
        return 4;
    }
    else if(strcmp(searchedField, "unidadeMedida") == 0){
        return 5;
    }
    else if(strcmp(searchedField, "velocidade") == 0){
        return 6;
    }
    else if(strcmp(searchedField, "nomePoPs") == 0){
        return 7;
    }
    else if(strcmp(searchedField, "nomePais") == 0){
        return 8;
    }
    else{
        return -1; // ERROR flag
    }
}