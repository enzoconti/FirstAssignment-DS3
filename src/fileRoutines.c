#include "../include/fileRoutines.h"

// this function will read the CSV from CSVfp line by line creating a temporary data record on RAM to be stored on the binary file binfp
// it is utilized on functionality 1
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

// this function reads a record from the CSV, being utilized on functionality 1
// it returns 1 if the read was sucessfull, and 0 if not - being used on the stop condition of readCSV_writeBin loop
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

// this function reads one field of the CSV (and fieldFlag carries the information of which field it is)
// it returns 1 if the read is sucessfull and 0 if not
int readCSVField(FILE *CSVfp, DATARECORD*dr, int fieldFlag){

    char buffChar, nullFlag=1, buffStr[MAX_VARSTRINGSIZE];

    // first, this loop reads the file until an invalid character and puts it onto a string
    int i=0;
    while(1){
        nullFlag = fread(&buffChar,sizeof(char),1,CSVfp);
        if(nullFlag == 0) return 0;

        if(isValid(CSVfp,buffChar) == 0){ // this sub-function checks for an invalid character to end the reading of a field
            break;
        }

        buffStr[i] = buffChar;
        i++;
    }
    buffStr[i] = '\0';

    // there is some special cases in which the last or first characters are whitespaces
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

// this function checks if a character readen by the csv is valid
// ',' is invalid bc it is the delimitant between two fields
// '\n' is invalid bc it is the delimitant between two records
// there is another special case that is '\r' followed by '\n', in this case we return 0
// but if the char is '\r' and the next is not '\n', so we need to fseek back bc the next is a field character
int isValid(FILE *fp,char c){ 
    char nextChar;
    if(c == '\n' || c == ','){
        return 0;
    }

    else if(c == '\r'){
        nextChar = fgetc(fp);
        if(nextChar != '\n'){
            fseek(fp,-1,SEEK_CUR); // nextChar is a field character and we've gotten it by mistake, so fseek back to get it again when we read a field
            return 0;
        }else if(nextChar == '\n'){
            return 0;
        }
    }

    return 1;
}

// this will read a header and load it into RAM
// it also sets the fp ready to later data record reading by fseeking it until the end of the cluster
void readHeader(FILE* fp, HEADER* outHeader){
    for(int i=0;i<6;i++){
        readHeaderField(fp,outHeader,i); // basically reading each one of 6 fields
    }

    fseek(fp,CLUSTERSIZE - HEADERSIZE, SEEK_CUR); // this jumps the trash so that the readRecord that may follow can start by the data records

    return ;
}

// this reads one field of the header depending on fieldFlag
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


    while(countFieldsSize < DATARECORDSIZE){ // the stop case is until the end of the datarecord size, but bc there are variable size fields this is not always reached - which is the reason that readDataField must have a 0 return case to indicate the file has ended
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

        fieldFlag = fieldFlag + 1; // this makes the fieldFlag increase from 0 to 8
        if(fieldFlag == 9){ // this means all the 9 fields have been readen, but not necessairly we have reached the size of record(bc it can be trash at the end)
            trashsize = DATARECORDSIZE - countFieldsSize;
            break;
        }
    }
    fseek(fp,trashsize,SEEK_CUR); // this makes the function jump the trash that may be on the end, making it possible to read the next record on a eventual next call

    return 1; // if the Record has ended and the file still not, we return 1 to indicate the program can read another record
    //se o registro terminou e o arquivo ainda não, retornamos 1 para indicar ao readFile que ele pode ler outro registro
}

// this function will read one of the 9 possible data fields according to the fieldFlag and puts it onto a field of outData
int readDataField(FILE* fp, int fieldFlag, DATARECORD* outData){
    
    int outSizeCounter = 0; // this accumulates how much has been readen for this specific field
    int nullFlag = 1; // this flag will indicate when fread fails(meaning the file has ended)
    int i = 0;
    char buffChar = 'S';
    //printf("\nValos do fieldFlag %d\n\n", fieldFlag);
    switch(fieldFlag){

        case 0: // removido Field
            nullFlag = fread(&(outData->removido),sizeof(char),1,fp);
            outSizeCounter = sizeof(char);
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
            
                if(i >= MAX_VARSTRINGSIZE){ // if it reaches the max size and has not found a delimiter it truncates the value (it should not be needed by the test cases, but some *stack smashing* erros ocurred without it)
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
        sizeWritten += writeDataField(fp,dr,i); // writeDataField returns the size that was written
    }
    int trashsize = DATARECORDSIZE - sizeWritten; // the rest of fixed-size record is filled with trash
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


// this function is used by functionality 3 to search a field on a file and print the record that was found
int searchFileAndPrint(FILE* fp,int fieldFlag){
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
    if(isKeyInt == 1){
        //printf("calling searchIntOnFile\n");
         nRecords = searchIntOnFile(fp, fieldFlag, IntegerKey);
    }else{
        //printf("calling searchStrOnFile\n");
        nRecords = searchStrOnFile(fp, fieldFlag, StrKey);
    }
    return nRecords;
}

// this function is used by functionality 4 and is similar to the one above - but it must carry a header as argument bc it is updated(in RAM) while we remove records
int searchFileAndRemove(FILE* fp,HEADER* h,int fieldFlag){
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
    if(isKeyInt == 1){
        //printf("calling searchIntOnFile\n");
         nRecords = removeIntOnFile(fp,h,fieldFlag, IntegerKey);
    }else{
        //printf("calling searchStrOnFile\n");
        nRecords = removeStrOnFile(fp, h,fieldFlag, StrKey);
    }
    
    return nRecords;
}


// this function searches the file for an int key, by reading every record and  printing it if the desired key-field equals the inputted search key of that field
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

    if(hasFound == 0) printNoRecordError(); // if there is no record that correponds to the one searched, we let the user know that there is no record
    return countRecords;
}

// this function searches the file for a string/char key, by reading every record and  printing it if the desired key-field equals the inputted search key of that field
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
        switch(fieldFlag){ // there are 4 char/char* data fields, siglaPais(fieldFlag=3), unidadeMedida(fieldFlag=5), nomePoPs(fieldFlag=7), nomePais(fieldFlag=8)
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

    if(hasFound == 0) printNoRecordError(); // if there is no record that correponds to the one searched, we let the user know that there is no record

    return countRecords;
}

// this function inputs the name of a field in a string and returns a flag corresponding to that field
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

// this function is used in functionality 5 - it gets the RRN that sould be inserted
// it returns a flag that indicates if that RRN is from end of file or from removed stack, so that the program knows if it must update header or not
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

// this function inserts a data record inputDr onto the RRN addRRN and, if necessary, updated the header h
void insert(FILE* fp, int addRRN, DATARECORD* inputDr,HEADER *h, int inputFlag){
    int byteoffset = addRRN * DATARECORDSIZE + CLUSTERSIZE; // we calculate the byteoffset from RRN by multiplying it to datarecordsize and skipping the first cluster that correponds to header
    fseek(fp,byteoffset,SEEK_SET);
    DATARECORD removedDataRecord;

    if(inputFlag == 1){ // this will update the header topoStack field by popping the RRN2 from the stack, in the case that we insert on a removed spot and not on end
        // reads to update header
        readDataRecord(fp, &removedDataRecord);
        h->topoStack = removedDataRecord.encadeamento; // updates the stack
        h->nroRegRem--;                                // descrease the number of removed records
        fseek(fp,byteoffset,SEEK_SET);                 // back to the record to rewrite it
    }else{
        h->proxRRN++; // if we add to the end we need to count this RRN on proxRRN
    }

    // we write the data record knowing that fp is already on the right position
    writeDataRecord(fp,inputDr);
}

// this function removes the fields identified as of type int
int removeIntOnFile(FILE* fp, HEADER* h,int fieldFlag, int key){
    DATARECORD dr; //struct destined to receive the record to be removed
    int countRecords=0; //is a variable that counts how many times the loop ran

    //printf("inside searchIntOnFIle and looking for key=%d for fieldFlag=%d\n", key, fieldFlag);

    //int i=0;
    while(readDataRecord(fp, &dr) != 0){ //this loop is reading the data from the file and passing it to the struct as long as there is data in the file
        countRecords++;
        //printf("inside the loop of removeIntOnFile for %dth time\n",i++);
        //printRecord(dr);
        switch(fieldFlag){ // there are 3 integer data fields, idConecta(2), idPoPsConectado(4) and velocidade(6)
            case 2: // idConecta field
                //printf("inside case 2 of searchIntOnFile\nlooking for key=%d and got this record with dr.idConecta=%d\n'n",key,dr.idConecta);
                if(dr.idConecta == key){
                    printf("Im removing based on idConecta and dr.idConecta=%d and key=%d\n",dr.idConecta,key);
                    //printRecord(dr);
                    removeRecord(fp, h,countRecords-1); // this sub-function removes the record and updated the header (we need RRN = countRecords-1 to update the stack)
                }
                break;
            case 4: // idPoPsConectado field
                if(dr.idPoPsConectado == key){
                    printf("Im removing based on idPoPsConectado and dr.idPoPsConectado=%d and key=%d\n",dr.idPoPsConectado,key);
                    //printRecord(dr);
                    removeRecord(fp, h,countRecords-1);
                }
                break;
            case 6: // velocidade field
                if(dr.velocidade == key){
                    printf("Im removing based on velocidade and dr.velocidade=%d and key=%d\n",dr.velocidade,key);
                    //printRecord(dr);
                    removeRecord(fp, h,countRecords-1);
                }
            break;
        }
    }

    return countRecords; // this returns how many records have been readen
}


//this function removes the fields identified as of type char (string)
int removeStrOnFile(FILE*fp, HEADER* h,int fieldFlag, char* key){
    DATARECORD dr;  //struct destined to receive the record to be removed
    int countRecords=0; //is a variable that counts how many times the loop ran

    //UFA
    //printf("header has been readen as:\n");
    //printHeader(h);
    //printf("inside searchStrOnFile with key=%s and fieldFlag=%d\nftell is currently on %ld",key,fieldFlag, ftell(fp));
    //writeDataRecord(fp, &dr);
    //int i=0;
    while(readDataRecord(fp, &dr) != 0){ //this loop is reading the data from the file and passing it to the struct as long as there is data in the file
        countRecords++;
        //printf("inside loop of removeStrOnFile for %dth time",i++);
        //printf("th data record has been readen as:\n");
        //printRecord(dr);
        switch(fieldFlag){ // there are 4 char/char* data fields, siglaPais(3), unidadeMedida(5), nomePoPs(7), nomePais(8)
            case 3: // siglaPais field
                if(strcmp(dr.siglaPais,key) == 0){
                    printf("Im removing based on siglaPais and dr.siglaPais=%s and key=%s\n", dr.siglaPais,key);
                    //printRecord(dr);
                    removeRecord(fp, h,countRecords-1);
                }
                break;
            case 5: // unidadeMedida field
                if(dr.unidadeMedida == key[0]){
                    printf("Im removing based on unidadeMedida and dr.unidadeMedida=%c and key=%s\n", dr.unidadeMedida,key);
                    //printRecord(dr);
                    removeRecord(fp, h,countRecords-1);
                }
                break;
            case 7: // nomePoPs field
                if(strcmp(dr.nomePoPs,key) == 0){
                    printf("Im removing based on nomePoPs and dr.nomePoPs=%s and key=%s\n", dr.nomePoPs,key);
                    //printRecord(dr);
                    removeRecord(fp, h,countRecords-1);
                }
                break;
            case 8: // nomePais field
                if(strcmp(dr.nomePais,key) == 0){
                    printf("Im removing based on nomePais and dr.nomePais=%s and key=%s\n", dr.nomePais,key);
                    //printRecord(dr);
                    removeRecord(fp, h,countRecords-1);
                }   
                break;
        }
    }

    return countRecords;
}

//this function actually removes the registry after it is found
void removeRecord(FILE *fp,HEADER* h, int RRN){
    printf("*****Entrei na função removeRecord******\n");
    printf("Header atual:\n");
    printHeader(*h);
    printf("******RRN a ser removido eh %d******\n",RRN);
    //HEADER h;
    //DATARECORD d;
    int byteoffset = RRN * DATARECORDSIZE + CLUSTERSIZE;
    fseek(fp,byteoffset,SEEK_SET);
    
    char newStatus = '1';
    // pushing onto the removed stack
    int encadeamento = h->topoStack;

    // updating header
    h->topoStack = RRN;
    h->nroRegRem++;

    // only 5 bytes written, the rest is trash (+1 for \0)
    char trash[DATARECORDSIZE - 5 + 1] = "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$";

    // we rewrite this record
    fwrite(&newStatus,sizeof(char),1,fp);
    fwrite(&encadeamento,sizeof(int),1,fp);
    fwrite(trash,(DATARECORDSIZE - 5) *sizeof(char),1,fp);
}


//function that will compress the file
void compact(FILE *OriginalFp,FILE* auxCompact,HEADER* currentHeader){ 
    DATARECORD tempData;
    HEADER newH;

    int countRecords=0;

    // we skip the header of auxCompact bc we dont have all information about it
    fseek(auxCompact,CLUSTERSIZE,SEEK_SET); 
    while(readDataRecord(OriginalFp,&tempData) != 0){ //traversing the file to find the records marked as removed
        if(tempData.removido == '0'){ // if it is not removed we rewrite on auxCompact
            countRecords++;
            writeDataRecord(auxCompact,&tempData);
        }
    }
    //rewriting the header according to how the file was after compression
    newH.status = '1';
    newH.topoStack = -1;
    newH.nroRegRem = 0;
    newH.nroPagDisco = calculateNroPagDisco(countRecords);
    newH.proxRRN = countRecords;
    newH.qttCompacta = currentHeader->qttCompacta+1;

    fseek(auxCompact,0,SEEK_SET);
    writeHeaderRecord(auxCompact,&newH); //writing the undeleted files in the helper file, which will be the "correct" file in the end

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