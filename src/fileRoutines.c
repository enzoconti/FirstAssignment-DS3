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
    head->nroPagDisco = countRecords*DATARECORDSIZE / CLUSTERSIZE + 1; // 1 from header cluster
    if(countRecords*DATARECORDSIZE % CLUSTERSIZE != 0) head->nroPagDisco++;
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
            nullFlag = fread(&(outData->siglaPais),2,1,fp);
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
            //printf("inside case 8(nomePais of readDataField\n");
            while(1){
                nullFlag = fread(&buffChar,sizeof(char),1,fp);
                //printf("with i=%d got buffChar=%c and nullFlag=%d\n",i,buffChar,nullFlag);
                if(nullFlag == 0) break;
                outSizeCounter++;
            

                if(buffChar != '|'){
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
        //printf("inside the loop of searchIntOnFile for %dth time\n",i);
        switch(fieldFlag){ // there are 3 integer data fields, idConecta(2), idPoPsConectado(4) and velocidade(6)
            case 2: // idConecta field
                //printf("inside case 2 of searchIntOnFile\nlooking for key=%d and got this record with dr.idConecta=%d\n'n",key,dr.idConecta);
                if(dr.idConecta == key){
                    printRecord(dr);
                    hasFound=1;
                }
                break;
            case 4: // idPoPsConectado field
                if(dr.idPoPsConectado == key){
                    printRecord(dr);
                    hasFound=1;
                }
                break;
            case 6: // velocidade field
                if(dr.velocidade == key){
                    printRecord(dr);
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
    

    while( readDataRecord(fp, &dr) != 0){
        countRecords++;
        //printf("inside loop of searchStrOnFile for %dth time",i);
        //printf("%dth data record has been readen as:\n",i);
        //printRecord(dr);
        switch(fieldFlag){ // there are 4 char/char* data fields, siglaPais(3), unidadeMedida(5), nomePoPs(7), nomePais(8)
            case 3: // siglaPais field
                if(strcmp(dr.siglaPais,key) == 0){
                    printRecord(dr);
                    hasFound=1;
                }
                break;
            case 5: // unidadeMedida field
                if(dr.unidadeMedida == key[0]){
                    printRecord(dr);
                    hasFound=1;
                }
                break;
            case 7: // nomePoPs field
                if(strcmp(dr.nomePoPs,key) == 0){
                    printRecord(dr);
                    hasFound=1;
                }
                break;
            case 8: // nomePais field
                if(strcmp(dr.nomePais,key) == 0){
                    printRecord(dr);
                    hasFound=1;
                }   
                break;
        }
    }

    if(hasFound == 0) printNoRecordError();

    return countRecords;
}


int getFlag_fromDataField(char* searchedField){
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

int getRRN4Insertion(FILE* fp, int*RRN,HEADER* h){
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

    if(inputFlag == 1){ // this will update the header topoStack field by popping the RRN2 from the stack
        readDataRecord(fp, &removedDataRecord);
        h->topoStack = removedDataRecord.encadeamento;
        fseek(fp,byteoffset,SEEK_SET); // back to the record to rewrite it
    }

    writeDataRecord(fp,inputDr);
}

int removeIntOnFile(FILE* fp, int fieldFlag, int key){
    DATARECORD dr;
    HEADER h;
    int countRecords=0,hasFound=0;

    //printf("inside searchIntOnFIle and looking for key=%d for fieldFlag=%d\n", key, fieldFlag);
    readHeader(fp,&h);

    
    while(readDataRecord(fp, &dr) != 0){
        countRecords++;
        //printf("inside the loop of searchIntOnFile for %dth time\n",i);
        switch(fieldFlag){ // there are 3 integer data fields, idConecta(2), idPoPsConectado(4) and velocidade(6)
            case 2: // idConecta field
                //printf("inside case 2 of searchIntOnFile\nlooking for key=%d and got this record with dr.idConecta=%d\n'n",key,dr.idConecta);
                if(dr.idConecta == key){
                    //printRecord(dr);
                    removeRegister(fp);
                    hasFound=1;
                }
                break;
            case 4: // idPoPsConectado field
                if(dr.idPoPsConectado == key){
                    //printRecord(dr);
                    removeRegister(fp);
                    hasFound=1;
                }
                break;
            case 6: // velocidade field
                if(dr.velocidade == key){
                    //printRecord(dr);
                    removeRegister(fp);
                    hasFound=1;
                }
            break;
        }
    }

    if(hasFound == 0) printNoRecordError();
    return countRecords;
}

//UFA recebe o nome do campo do dado tipo char e o dado em si para encontrar o registro
int removeStrOnFile(FILE*fp, int fieldFlag, char* key){
    DATARECORD dr;
    HEADER h;
    int countRecords=0;
    int hasFound=0;

    readHeader(fp,&h);
    //printf("header has been readen as:\n");
    //printHeader(h);
    //printf("inside searchStrOnFile with key=%s and fieldFlag=%d\nftell is currently on %ld",key,fieldFlag, ftell(fp));
    

    while( readDataRecord(fp, &dr) != 0){
        countRecords++;
        //printf("inside loop of searchStrOnFile for %dth time",i);
        //printf("%dth data record has been readen as:\n",i);
        //printRecord(dr);
        switch(fieldFlag){ // there are 4 char/char* data fields, siglaPais(3), unidadeMedida(5), nomePoPs(7), nomePais(8)
            case 3: // siglaPais field
                if(strcmp(dr.siglaPais,key) == 0){
                    //printRecord(dr);
                    removeRegister(fp);
                    hasFound=1;
                }
                break;
            case 5: // unidadeMedida field
                if(dr.unidadeMedida == key[0]){
                    //printRecord(dr);
                    removeRegister(fp);
                    hasFound=1;
                }
                break;
            case 7: // nomePoPs field
                if(strcmp(dr.nomePoPs,key) == 0){
                    //printRecord(dr);
                    removeRegister(fp);
                    hasFound=1;
                }
                break;
            case 8: // nomePais field
                if(strcmp(dr.nomePais,key) == 0){
                    //printRecord(dr);
                    removeRegister(fp);
                    hasFound=1;
                }   
                break;
        }
    }

    if(hasFound == 0) printNoRecordError();

    return countRecords;
}

void removeRegister(FILE *fp){ 
    //vou remover direto dentro do arquivo
    fseek(fp, -64, SEEK_CUR);

    //UFA atualizar o header aqui!

    const char* lixo = "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"; 
    char removido = '1';
    int encadeamento = attHeader(fp); //UFA mexer melhor aqui!
    fwrite(&removido, 1,1, fp);
    fwrite(&encadeamento, sizeof(int),1, fp);
    //for(int i = 0; i < 64, i++){
    fwrite(lixo, 1, 59, fp);
    //}
}

HEADER updateHeader(FILE *fp, HEADER *he){
    //UFA entrar com o file mesmo?
    static HEADER newHeader;
    newHeader = *he;
    //UFA topo eu tenho que mudar, nele fica o RRN do registro removido
    //UFA proxRRN vai ter que mudar para apontar para o rrn disponivel para
    //UFA  inserção, no caso o ultima (?)
    //UFA nroRegRem é tipo um contador para falar quantos registros foram
    //UFA   removidos

    //newHeader.encadeamento = 2;
    //newHeader.nroRegRem = 5;

    return newHeader;
}

int attHeader(FILE* fp){
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
}

void compact(FILE *fp){
    DATARECORD achaRemovido;
    //DATARECORD achaProximoRegistro;
    //HEADER cabecalho;
    int countRecords=0;
    //int hasFound=0;
    int lugarRemovido = 0;
    char auxiliaRemovido;
    printf("Entrou na função compact\n");
    //será que no header tem a quantidade de registros removidos???
    //primeiro eu confiro se tem registro removido
    //se tiver eu começo a compactar
    //caso nao eu devo colocar o erro eu acho
    //para compacar, eu tenho que fazer tudo no mesmo arquivo
    //para isso eu vou precisar usar um struct auxiliar
    //usar um ponteiro para arquivo auxiliar também????
    while(readDataRecord(fp, &achaRemovido) != 0){ //UFA e se eu usar o fread aqui?
        countRecords ++;
        printf("Entrou no primeiro while da função compact\n");
        /*printf("1 %d \n",achaRemovido.removido);
        printf("1 %d \n",achaRemovido.encadeamento);
        printf("1 %d \n",achaRemovido.idConecta);
        printf("1 %s \n",achaRemovido.siglaPais);
        printf("1 %d \n",achaRemovido.idPoPsConectado);
        //printf("%s \n",achaRemovido.unidadeMedida);
        printf("1 %d \n",achaRemovido.velocidade);
        printf("1 %s \n",achaRemovido.nomePoPs);
        printf("1 %s \n",achaRemovido.nomePais);*/
        if(achaRemovido.removido == '1'){ //UFA se for igual a 1 é porque o registro foi removido
            printf("Achei o removido\n");
            /*printf("%d \n",achaRemovido.removido);
            printf("%d \n",achaRemovido.encadeamento);
            printf("%d \n",achaRemovido.idConecta);
            printf("%s \n",achaRemovido.siglaPais);
            printf("%d \n",achaRemovido.idPoPsConectado);
            //printf("%s \n",achaRemovido.unidadeMedida);
            printf("%d \n",achaRemovido.velocidade);
            printf("%s \n",achaRemovido.nomePoPs);
            printf("%s \n",achaRemovido.nomePais);*/

            lugarRemovido = countRecords;
            printf("lugarRemovido: %d \n", lugarRemovido);
        } //UFA fim do if(achaRemovido.removido == '1')
        fseek(fp, (lugarRemovido * 64) + 64, SEEK_SET);
        fread(&auxiliaRemovido, sizeof(char), 1, fp); //UFA esse 1 pode dar merda...
        if(auxiliaRemovido == '1'){
            printf("dois registros seguidos removidos\n");
            //ai eu tenho que ver se o proximo também foi removido 
            //eu posso fazer um loop pra isso, porque ai caso o proximo
            //nao seja removido, ele vai ser armazenado na struct
        }
        else{
            printf("O proximo registro nao foi removido\n");
            writeDataRecord(fp, &achaRemovido);
            printf("%d \n",achaRemovido.removido);
            printf("%d \n",achaRemovido.encadeamento);
            printf("%d \n",achaRemovido.idConecta);
            printf("%s \n",achaRemovido.siglaPais);
            printf("%d \n",achaRemovido.idPoPsConectado);
            //printf("%s \n",achaRemovido.unidadeMedida);
            printf("%d \n",achaRemovido.velocidade);
            printf("%s \n",achaRemovido.nomePoPs);
            printf("%s \n",achaRemovido.nomePais);
        }
            //aqui eu tenho que fazer um loop para achar o proximo registro?
            //eu tenho que salvar aqui o lugar no arquivo em que o arquivo esta
            //usar o coiso de RRN????
            //while(readDataRecord(fp, &achaProximoRegistro) != 0){
                //printf("Entrou no primeiro while da função compact\n");
                //if(achaProximoRegistro.removido == '0'){
                    //printf("Achei o proximo\n");
                    //aqui eu posso usar pra sair do loop e add os dados da struct ontem esta removido
                    //break;
                //}
            //}
            //aqui eu tenho que pegar o lugar do registro removido e sobreescrever com a minha struct
            //fseek(fp, (lugarRemovido * 64), SEEK_SET);
            //writeDataRecord(fp, &achaProximoRegistro);
        //}

    }

}

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