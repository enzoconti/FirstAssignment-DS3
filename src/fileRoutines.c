#include "../include/fileRoutines.h"

int attHeader(FILE* fp);

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
        //printf("\ngoing into writeDataRecord with the record set as:\n");
        //printRecord(tempData);
        writeDataRecord(binfp, &tempData);
        countRecords++;
    }

    // now that we have the whole information about the data record we can update our header
    head->nroPagDisco = calculateNroPagDisco(countRecords);
    //head->nroPagDisco=0;
    head->proxRRN = countRecords;
    head->status = '1';

    //printf("\nthe header has been set as:\n");
    //printHeader(*head);
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

    // there is a special case in which the last or first characters are whitespaces
    // so we need a function to eliminate them
    strcpy(buffStr,removeSpaces(buffStr));
    
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

int isValid(FILE *fp,char c){ //UFA o que essa funcao faz???
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

    while(readDataRecord(fp, &tempData) != 0){ // this reads a Record from fp and puts its data into tempData
        // printData(tempData);     // AINDA FALTA IMPLEMENTAR      
        countRecords++;
    }

    return ;

}

// this will read a header and load it into RAM
void readHeader(FILE* fp, HEADER* outHeader){
    for(int i=0;i<6;i++){
        readHeaderField(fp,outHeader,i); // basically reading each field
    }

    fseek(fp,CLUSTERSIZE - HEADERSIZE, SEEK_CUR); // this jumps the trash so that the readRecord that may follow can start by the data records

    return ;
}
// this reads one field of the header depending on fieldFlag
// it also sets the fp ready to later data record reading by fseeking it until the end of the cluster
void readHeaderField(FILE* fp, HEADER* outh, int fieldFlag){
    switch(fieldFlag){
        case 0: // status field
            //printf("outh->status started as %d\n",(int) outh->status);
            fread(&(outh->status), sizeof(char),1,fp);
            //printf("got outh->status as %c\n", outh->status);
            break;
        case 1: // topoStack field
            //printf("outh->topoStack started as %d\n", outh->topoStack);
            fread(&(outh->topoStack), sizeof(int),1,fp);
            //printf("got outh->topoStack as %x\n",outh->topoStack);
            break;
        case 2: // proxRRN field
            fread(&(outh->proxRRN), sizeof(int),1,fp);
            //printf("got outh->proxRRN as %d\n",outh->proxRRN);
            break;
        case 3: // nroRegRem field
            fread(&(outh->nroRegRem),sizeof(int),1,fp);
            //printf("got outh->nroRegTem as %d\n", outh->nroRegRem);
            break;
        case 4: // nroPagDisco field
            fread(&(outh->nroPagDisco),sizeof(int),1,fp);
            //printf("got outh->nroPagDisco as %d\n", outh->nroPagDisco);
            break;
        case 5: // qttCompacta field
            fread(&(outh->qttCompacta),sizeof(int),1,fp);
            //printf("got outh->qttCompacta as %d\n", outh->qttCompacta);
            break;
    }

}

// this will read an entire data record and put it into the outData instance
// isso lerá um registro inteiro e o colocará na instância outData
int readDataRecord(FILE *fp, DATARECORD* outData){
    //printf("readDataRecord has been called\n");
    int countFieldsSize = 0, buff=0, fieldFlag = 0,trashsize=0; // the fieldFlag indicates which of the 5 possible fields we are reading - to know its size and where to put it onto PERSON struct
    //o fieldFlag indica quais dos 5 campos possíveis estamos lendo - para saber seu tamanho e onde colocá-lo na estrutura PERSON


    while(countFieldsSize < DATARECORDSIZE){
        // tentei fazer isso pra nao ler tudo mas ele precisa ler o encadeamento, entao nao posso pular direto
        // senao da errado nas insercoes. se por acaso der problema reimplemento pra ler depois do segundo, mas sem ele parece funcionar
        /*if(fieldFlag == 1){ // on the second iteration we check if the previous one read that the record is removed, if it is we stop reading
            if(outData->removido == '1'){
                fseek(fp,DATARECORDSIZE - countFieldsSize,SEEK_CUR);
                return 1;
            }
        }*/

        
        buff = readDataField(fp, fieldFlag, outData);
        //printf("inside the loop of readDataRecord, got buff=%d for fieldFlag=%d\n",buff,fieldFlag);
        //printf("data record currently as:\n\n");
        //printRecord(*outData);
        //printf("\n");

        if(buff == 0){ // this indicates the file has ended
            //isso indica que o arquivo terminou
            return 0;
        }else{
            countFieldsSize+=buff;// we accumulate the non-zero buffer to know how much of the record we have already read
        }//acumulamos o buffer diferente de zero para saber quanto do registrador já lemos

        fieldFlag = fieldFlag + 1; // this makes the fieldFlag loop through 0 -> 1 -> 2 -> 3 -> ... -> 8 
        if(fieldFlag == 9){ // this means all the 9 fields have been readen, but not necessairly we have reached the size of record(bc it can be trash at the end)
            trashsize = DATARECORDSIZE - countFieldsSize;
            break;
        }
    }
    fseek(fp,trashsize,SEEK_CUR); // this makes the function jump the trash that may be on the end, making it possible to read the next record on a eventual next call

    return 1; // if the Record has ended and the file still not, we return 1 to indicate to readFile that it can read another record
    //se o registro terminou e o arquivo ainda não, retornamos 1 para indicar ao readFile que ele pode ler outro registro
}

// this function will read one of the 9 possible data fields according to the fieldFlag and puts it onto a field of outData
int readDataField(FILE* fp, int fieldFlag, DATARECORD* outData){
    
    int outSizeCounter = 0;
    int nullFlag = 1; // this flag will indicate when fread fails(meaning the file has ended)
    int i = 0;
    char buffChar = 'S';
    //printf("\nValos do fieldFlag %d\n\n", fieldFlag);
    switch(fieldFlag){

        case 0: // removido Field
            nullFlag = fread(&(outData->removido),sizeof(char),1,fp);
            outSizeCounter = 1;
            break;

        case 1: // encadeamento Field
            nullFlag = fread(&(outData->encadeamento),sizeof(int),1,fp);
            outSizeCounter = sizeof(int);
            //printf("got outhData->encadeamento as %d\n", outData->encadeamento);
            break;

        case 2: // idConecta Field
            nullFlag = fread(&(outData->idConecta),sizeof(int),1,fp);
            outSizeCounter = sizeof(int);
            //printf("got outhData->idConecta as %d\n", outData->idConecta);
            break;

        case 3: // siglaPais Field
            nullFlag = fread(&(outData->siglaPais),2*sizeof(char),1,fp);
            outData->siglaPais[2] = '\0';
            outSizeCounter = 2;
            //printf("got outhData->siglaPais as %s\n", outData->siglaPais);
            break;

        case 4: // idPoPsConectdo Field
            nullFlag = fread(&(outData->idPoPsConectado),sizeof(int),1,fp);
            outSizeCounter = sizeof(int);
            //printf("got outhData->idPoPsConectado as %d\n", outData->idPoPsConectado);
            break;
        case 5: // unidadeMedida Field
            nullFlag = fread(&(outData->unidadeMedida),sizeof(char),1,fp);
            outSizeCounter = sizeof(char);
            //printf("got outhData->unidadeMedida as %c\n", outData->unidadeMedida);
            break;
        
        case 6: // velocidade Field
            nullFlag = fread(&(outData->velocidade),sizeof(int),1,fp);
            outSizeCounter = sizeof(int);
            break;
        
        case 7: // nomePoPs Field
            i=0;
            //printf("inside case 7(nomePoPs) of readDataField\n");
            while(1){
                nullFlag = fread(&buffChar,sizeof(char),1,fp);
                if(nullFlag == 0) break;
                outSizeCounter++;
                //printf("with i=%d got buffChar=%c and nullFlag=%d\n",i,buffChar,nullFlag);
            
                if(i >= MAX_VARSTRINGSIZE){
                    outData->nomePoPs[MAX_VARSTRINGSIZE-1] = '\0';
                    break;
                }
                else if(buffChar != '|'){
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
            //printf("inside case 8(nomePais of readDataField\n");
            while(1){
                nullFlag = fread(&buffChar,sizeof(char),1,fp);
                //printf("with i=%d got buffChar=%c and nullFlag=%d\n",i,buffChar,nullFlag);
                if(nullFlag == 0) break;
                outSizeCounter++;
            
                if(i >= MAX_VARSTRINGSIZE){
                    outData->nomePais[MAX_VARSTRINGSIZE-1] = '\0';
                    break;
                }
                else if(buffChar != '|'){
                    outData->nomePais[i] = buffChar;
                    i++;
                }else{
                    outData->nomePais[i] = '\0';
                    i++;
                    break;
                }

            }
            //printf("got outhData->nomePais as %s\n", outData->nomePais);
            break;
    }
    //printf("fieldFlag: %d\n nullFlag %d\n", fieldFlag, nullFlag);

    if(nullFlag == 0) return 0; // this indicates that the file has ended

    return outSizeCounter;
}

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
    free(trash);
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
    free(trash);
}

// this funciton writes a data field based on fieldFlag
// it also returns the size that has been written so that the upper function
// can deal with how much trash it must write
int writeDataField(FILE *fp, DATARECORD* dr, int fieldFlag){
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
//UFA leio o campo identificao do que eh o dado e vejo se eh um campo de char ou int
int searchFileAndPrint(FILE* fp,int fieldFlag, int func){
    // bc we have strong types, we declare the two possible types of keys (but we will use only one)
    int IntegerKey;
    char StrKey[MAX_VARSTRINGSIZE];
    int isKeyInt; // and this flag will hold wheter the key is an integer or not
    int nRecords; // this will hold the number of records that were searched

    //printf("getting inside switch case of searchFileAndPrint\n");
    // this switch case will input the right key according to the fieldFlag and set isKeyInt 
    // so that we can know the difference between the valid key that was setted and the key that only holds an old non-significant memory value
    switch(fieldFlag){ //esse switch case ta servindo para que eu possa pegar o nome do campo digitado
        case 2: // idConecta Field
            scanf("%d", &IntegerKey);
            isKeyInt = 1;
            break;
        case 3: // siglaPais Field
            scan_quote_string(StrKey); //UFA essa funcao eh dada pela prof para ler o dado de fato
            isKeyInt = 0;
            break;
        case 4: // idPoPsConectado Field
            scanf("%d", &IntegerKey);
            isKeyInt = 1;
            break;
        case 5: //unidadeMedida Field
            scan_quote_string(StrKey); //UFA essa funcao eh dada pela prof para ler o dado de fato
            isKeyInt = 0;
            break;
        case 6: // velocidade Field
            scanf("%d", &IntegerKey); 
            isKeyInt = 1;
            break;
        case 7: // nomePoPs Field
            scan_quote_string(StrKey); //UFA essa funcao eh dada pela prof para ler o dado de fato
            isKeyInt = 0;
            break;
        case 8: // nomePais Field
            scan_quote_string(StrKey); //UFA essa funcao eh dada pela prof para ler o dado de fato
            isKeyInt = 0;
            break;
    }

    //printf("outsssssssside switch case with isKeyInt=%d, IntegerKey=%d and StrKey=%s\n", isKeyInt, IntegerKey, StrKey);
    // this two subfunctions are responsible for searching the values that were set-up b4
    if(func == 3){
        if(isKeyInt == 1){
            //printf("calling searchIntOnFile\n");
             nRecords = searchIntOnFile(fp, fieldFlag, IntegerKey);
        }else{
            //printf("calling searchStrOnFile\n");
            nRecords = searchStrOnFile(fp, fieldFlag, StrKey);
        }
    }else{
        if(isKeyInt == 1){
            //printf("calling searchIntOnFile\n");
             nRecords = removeIntOnFile(fp, fieldFlag, IntegerKey);
        }else{
            //printf("calling searchStrOnFile\n");
            nRecords = removeStrOnFile(fp, fieldFlag, StrKey);
        }
    }
    return nRecords;
}
//UFA recebe o nome do campo do dado tipo int
int searchIntOnFile(FILE* fp, int fieldFlag, int key){
    DATARECORD dr;
    int countRecords=0,hasFound=0;

    //printf("inside searchIntOnFIle and looking for key=%d for fieldFlag=%d\n", key, fieldFlag);

    
    while(readDataRecord(fp, &dr) != 0){
        countRecords++;
        //printf("searchIntOnFile has gotten data record:\n");
        //printRecord(dr);
        //printf("countRecords currently as %d\n", countRecords);
        //printf("inside the loop of searchIntOnFile for %dth time\n",i);
        switch(fieldFlag){ // there are 3 integer data fields, idConecta(2), idPoPsConectado(4) and velocidade(6)
            case 2: // idConecta field
                //printf("inside case 2 of searchIntOnFile\nlooking for key=%d and got this record with dr.idConecta=%d\n'n",key,dr.idConecta);
                if(dr.idConecta == key){
                    //printRecord(dr);
                    hasFound=1;
                }
                break;
            case 4: // idPoPsConectado field
                if(dr.idPoPsConectado == key){
                    //printRecord(dr);
                    hasFound=1;
                }
                break;
            case 6: // velocidade field
                if(dr.velocidade == key){
                    //printRecord(dr);
                    hasFound=1;
                }
            break;
        }
    }

    if(hasFound == 0) printNoRecordError();
    return countRecords;
}

//UFA recebe o nome do campo do dado tipo char e o dado em si para encontrar o registro
int searchStrOnFile(FILE*fp, int fieldFlag, char* key){
    DATARECORD dr;
    int countRecords=0;
    int hasFound=0;

    //printf("header has been readen as:\n");
    //printHeader(h);
    //printf("inside searchStrOnFile with key=%s and fieldFlag=%d\nftell is currently on %ld",key,fieldFlag, ftell(fp));
    

    while(readDataRecord(fp, &dr) != 0){
        countRecords++;
        //printf("searchStrOnFile has gotten data record:\n");
        //printRecord(dr);
        //printf("countRecords currently as %d\n", countRecords);
        //printf("inside loop of searchStrOnFile for %dth time",i);
        //printf("%dth data record has been readen as:\n",i);
        //printRecord(dr);
        switch(fieldFlag){ // there are 4 char/char* data fields, siglaPais(3), unidadeMedida(5), nomePoPs(7), nomePais(8)
            case 3: // siglaPais field
                if(strcmp(dr.siglaPais,key) == 0){
                    //printRecord(dr);
                    hasFound=1;
                }
                break;
            case 5: // unidadeMedida field
                if(dr.unidadeMedida == key[0]){
                    //printRecord(dr);
                    hasFound=1;
                }
                break;
            case 7: // nomePoPs field
                if(strcmp(dr.nomePoPs,key) == 0){
                    //printRecord(dr);
                    hasFound=1;
                }
                break;
            case 8: // nomePais field
                if(strcmp(dr.nomePais,key) == 0){
                    //printRecord(dr);
                    hasFound=1;
                }   
                break;
        }
    }

    if(hasFound == 0) printNoRecordError();

    return countRecords;
}


int getFlag_fromDataField(char* searchedField){ //UFA o que essa funcao faz???
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

int getRRN4Insertion(FILE* fp, int*RRN,HEADER* h){ //UFA o que essa funcao faz???
    int insertFlag;
    
    if(h->topoStack != -1){
        *RRN = h->topoStack;
        insertFlag = 1; // this flag means that there was a removed record, so, when inserting, we must put the "encadeamento" field on the topoStack
    }
    else{
        *RRN = h->proxRRN;
        insertFlag = 0; // this flag means that we will only insert on the end
    }

    return insertFlag;
}

void insert(FILE* fp, int addRRN, DATARECORD* inputDr,HEADER *h, int inputFlag){
    int byteoffset = addRRN * DATARECORDSIZE + CLUSTERSIZE; // we skip the first cluster for header and sum it to the byteoffset from the data records
    fseek(fp,byteoffset,SEEK_SET);
    DATARECORD removedDataRecord;

    if(inputFlag == 1){ // this will update the header topoStack field by popping the RRN2 from the stack, in the case that we insert on a removed spot and not on end
        readDataRecord(fp, &removedDataRecord);
        h->topoStack = removedDataRecord.encadeamento;
        h->nroRegRem--; // descrease the number of removed records
        fseek(fp,byteoffset,SEEK_SET); // back to the record to rewrite it
    }else{
        h->proxRRN++; // if we add to the end we need to count this RRN on proxRRN
    }

    writeDataRecord(fp,inputDr);
}

//this function removes the fields identified as of type int
int removeIntOnFile(FILE* fp, int fieldFlag, int key){
    DATARECORD dr; //struct destined to receive the record to be removed
    HEADER h; //struct intended to read the header
    int countRecords=0; //is a variable that counts how many times the loop ran
    int hasFound=0; //is a variable that counts how many times a record has been removed

    //printf("inside searchIntOnFIle and looking for key=%d for fieldFlag=%d\n", key, fieldFlag);
    readHeader(fp,&h); //header reading

    
    while(readDataRecord(fp, &dr) != 0){ //this loop is reading the data from the file and passing it to the struct as long as there is data in the file
        countRecords++;
        //printf("inside the loop of searchIntOnFile for %dth time\n",i);
        switch(fieldFlag){ // there are 3 integer data fields, idConecta(2), idPoPsConectado(4) and velocidade(6)
            case 2: // idConecta field
                //printf("inside case 2 of searchIntOnFile\nlooking for key=%d and got this record with dr.idConecta=%d\n'n",key,dr.idConecta);
                if(dr.idConecta == key){
                    //printRecord(dr);
                    removeRegister(fp, countRecords);
                    hasFound=1;
                }
                break;
            case 4: // idPoPsConectado field
                if(dr.idPoPsConectado == key){
                    //printRecord(dr);
                    removeRegister(fp, countRecords);
                    hasFound=1;
                }
                break;
            case 6: // velocidade field
                if(dr.velocidade == key){
                    //printRecord(dr);
                    removeRegister(fp, countRecords);
                    hasFound=1;
                }
            break;
        }
    }

    if(hasFound == 0) printNoRecordError(); //UFA (conferir) if no records were removed, that means no records were found, so I return an error
    return countRecords; //UFA, isso retorna a quantidade de vezes que o loop foi percorrido, mas não faz muito sentido....
}

//UFA recebe o nome do campo do dado tipo char e o dado em si para encontrar o registro
//this function removes the fields identified as of type char (string)
int removeStrOnFile(FILE*fp, int fieldFlag, char* key){
    DATARECORD dr;  //struct destined to receive the record to be removed
    HEADER h;  //struct intended to read the header
    int countRecords=0; //is a variable that counts how many times the loop ran
    int hasFound=0; //is a variable that counts how many times a record has been

    readHeader(fp,&h); //header reading //UFA
    //printf("header has been readen as:\n");
    //printHeader(h);
    //printf("inside searchStrOnFile with key=%s and fieldFlag=%d\nftell is currently on %ld",key,fieldFlag, ftell(fp));
    //writeDataRecord(fp, &dr);

    while(readDataRecord(fp, &dr) != 0){ //this loop is reading the data from the file and passing it to the struct as long as there is data in the file
        countRecords++;
        //printf("inside loop of searchStrOnFile for %dth time",i);
        //printf("th data record has been readen as:\n");
        //printRecord(dr);
        switch(fieldFlag){ // there are 4 char/char* data fields, siglaPais(3), unidadeMedida(5), nomePoPs(7), nomePais(8)
            case 3: // siglaPais field
                if(strcmp(dr.siglaPais,key) == 0){
                    //printRecord(dr);
                    removeRegister(fp, countRecords);
                    hasFound=1;
                }
                break;
            case 5: // unidadeMedida field
                if(dr.unidadeMedida == key[0]){
                    //printRecord(dr);
                    removeRegister(fp, countRecords);
                    hasFound=1;
                }
                break;
            case 7: // nomePoPs field
                if(strcmp(dr.nomePoPs,key) == 0){
                    //printRecord(dr);
                    removeRegister(fp, countRecords);
                    hasFound=1;
                }
                break;
            case 8: // nomePais field
                if(strcmp(dr.nomePais,key) == 0){
                    //printRecord(dr);
                    removeRegister(fp, countRecords);
                    hasFound=1;
                }   
                break;
        }
    }

    if(hasFound == 0) printNoRecordError();

    return countRecords;
}

//this function actually removes the registry after it is found
void removeRegister(FILE *fp, int count){
    //printf("Entrei na função removeRegister");
    //HEADER h;
    //DATARECORD d;

    //fseek(fp, -64, SEEK_CUR);
    fseek(fp,0,SEEK_SET);
    updateHeader(fp,count);

    fseek(fp,(count*64)+960,SEEK_SET);
    
    //printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA 1\n");
    //readDataRecord(fp,&d);
    //printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA 2\n");
    //printRecord(d);
    //fseek(fp,(count*64)+960,SEEK_SET);
    const char* lixo = "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"; //will be used to fill the entire registry space
    char removido = '1'; 
    int encadeamento = 100;//h.topoStack; //aqui tem que ser passado o topo
    
    fwrite(&removido, 1,1, fp); // UFA aqui eu to add o removido no cabeçalho meio que a força MELHORAR ISSO AQUI
    fwrite(&encadeamento, sizeof(int),1, fp); // UFA aqui eu to add o encadeamento no cabeçalho meio que a força MELHORAR ISSO AQUI
    fwrite(lixo, 1, 59, fp); //here the record you want to remove is being filled by $
    //fseek(fp, -64, SEEK_CUR);
    //readDataRecord(fp, &d);
    //printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA 3\n");
    //printRecord(d);
    //printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA 4\n");
    //updateHeader(fp,count);
    //printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
    //printRecord(d);
    //printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA 5\n");
}

void updateHeader(FILE *fp, int count){
    HEADER h;
    //fseek(fp, 0, SEEK_SET);
    readHeader(fp, &h);
    h.topoStack = count*64;
    //printf("h.proxRRN %d\n", h.proxRRN);
    h.proxRRN = h.proxRRN+1;
    h.nroRegRem = h.nroRegRem+1;
    fseek(fp, 0, SEEK_SET);
    /*printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
    printf("h.topoStack %d\n", h.topoStack);
    printf("h.proxRRN %d\n", h.proxRRN);
    printf("h.nroRegRem %d\n", h.nroRegRem);*/
    writeHeaderRecord(fp,&h);
    /*printf("h.topoStack %d\n", h.topoStack);
    printf("h.proxRRN %d\n", h.proxRRN);
    printf("h.nroRegRem %d\n", h.nroRegRem);*/
}

//UFA TEM QUE FAZER ESSA FUNÇÃO DIREITO SOCORRO AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
/*int attHeader(FILE* fp){
    int seekAtual = ftell(fp);
    fseek(fp, 1, SEEK_SET);
    //(proxRRN * TAM_REGITRO + T_CABEÇALHO)
    int contaa =  (seekAtual - 960)/64;
    int aqui;
    fread(&aqui, sizeof(int),1, fp);
    fseek(fp, 1, SEEK_SET);
    fwrite(&contaa, sizeof(int), 1, fp);
    fseek(fp, seekAtual, SEEK_SET);
    return aqui;    
<<<<<<< HEAD
}*/
=======
}
/*
>>>>>>> 922392d17b08b169700114f8d5580c32ff9cba5f
//UFA ISSO EH UM TESTE 
int void quantidadeRegistros(FILE *fp){
    DATARECORD registro;
    int contador = 0;
    while(readDataRecord(fp, &registro) != 0){
        contador++;
    }
    //printf("Contador: %d\n", contador);
    return contador;
}
*/

//UFA ESSA MERDA TA UM GRANDE CAOS, PQP!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void compact(FILE *OriginalFp,FILE* auxCompact,HEADER* currentHeader){ //UFA o numRemovidos esta no header, que é uma coisa que eu tenho que arrumar
    //UFA - ABRIR O ARQUIVO AI...
    DATARECORD tempData;
    HEADER newH;

    int countRecords=0;

    // we skip the header of auxCompact bc we dont have all information about it
    fseek(auxCompact,CLUSTERSIZE,SEEK_SET); 
    while(readDataRecord(OriginalFp,&tempData) != 0){
        if(tempData.removido == '0'){ // if it is not removed we rewrite on auxCompact
            countRecords++;
            writeDataRecord(auxCompact,&tempData);
        }
    }

    newH.status = '1';
    newH.topoStack = -1;
    newH.nroRegRem = 0;
    newH.nroPagDisco = calculateNroPagDisco(countRecords);
    newH.proxRRN = countRecords;
    newH.qttCompacta = currentHeader->qttCompacta+1;

    fseek(auxCompact,0,SEEK_SET);
    writeHeaderRecord(auxCompact,&newH);

    return;
    
}

// this function removes spaces from start and end of a string
// it is needed on functionality1 to treat some special cases of the csv input
char* removeSpaces(char* originalStr){
    //printf("removeSpaces called with str=%s\n", originalStr);
    int firstChar=0, lastChar=strlen(originalStr);
    //printf("firstChar started as %d and lastchar as %d\n",firstChar,lastChar);
    char *newStr;
    //printf("going to allocate newStr\n");
    newStr = malloc(MAX_VARSTRINGSIZE * sizeof(char));
    //printf("newStr allocated\n");

    for(int i=0;i<strlen(originalStr);i++){
        if(!isspace(originalStr[i])){
            firstChar = i; // this gets the firstChar that is not space
            break;
        }
    }
    //printf("outside 1st loop\n");

    for(int j=strlen(originalStr)-1;j>=0;j--){
        if(!isspace(originalStr[j])){
            lastChar = j; // this gets the last character that is not space
            break;
        }
    }
    //printf("outside 2nd loop\n");

    //printf("firstChar gotten as %d and lastchar as %d\n",firstChar,lastChar);
    for(int i=firstChar;i<=lastChar;i++){
        newStr[i-firstChar] = originalStr[i]; // this copies everything between the removed spaces onto a new string that is returned
    }   
    //printf("outside 3rd loop\n");

    //printf("removeSpaces returning with str=START%sEND\n", newStr);
    return newStr;
}