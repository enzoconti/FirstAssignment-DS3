#include "../include/fileRoutines.h"

// this function will read the CSV from CSVfp line by line creating a temporary data record on RAM to be stored on the binary file binfp
void readCSV_writeBin(FILE *CSVfp, FILE *binfp, HEADER *head){
    char* tempLine, tempField;
    int countRecords=0; // this counter will count how many lines there are in the csv(= number of records in the bin file) to be used on the header
    DATARECORD tempData;
    tempLine = malloc(50 * sizeof(char));
    tempField = malloc(50 * sizeof(char));
    const int flagSequence[7] = {2,7,8,3,4,5,6}; // This constant array helds the sequence of the fields inputted on CSV
    // the sequece is defined as: idConecta, nomePoPs, nomePais, siglaPais, idPoPsConectado, unidadeMedida, velocidade
    // this fields have the following fieldFlags(defined on the sequence of declaration on struct and used on other functions such as readFile):
    // idConecta = 2, nomePoPs = 7, nomePais = 8, siglaPais = 3, idPoPsConectado = 4, unidadeMedida = 5, velocidade = 6


    readCSVLine(CSVfp, &tempLine); // this simply reads the first line of the csv that consists of the sequence of fields
                                   // as the sequence is constant in this project, this is not used by the program (it's just a default for csv files)

    // this first loop will get line by line(record by record) of the csv and write it onto the file (we need to do this with tempData bc if the file is too large it wouldn't fit in RAM, so we cannot input everything first)
    while(readCSVLine(CSVfp, &tempLine) != 0){
        // this second loop will put the line values onto a temporary data record to later be written onto a file
        for(int i=0;i<7;i++){
            tempField = strtok(tempLine, ",");

            // treating the case of double delimiters, meaning the field has null value
            if( strcmp(tempField, ",") == 0){ // this will indicate that the field has null value, so we must update the tempField to the accordingly null indicator
                switch (flagSequence[i])
                {
                case 2: // idConecta field(int)
                case 4: // idPoPsConectado field(int)
                case 6: // velocidade field(int)
                    strcpy(tempField, "-1");
                    break;
                
                case 5: // unidadeMedida field(char)
                    strcpy(tempField, "$");
                    break;
                case 3: // siglaPais field(static char array)
                    strcpy(tempField, "$$");
                case 7: // nomePoPs field(variable size string)
                case 8: // nomePais field(variable size string)
                        strcpy(tempField, ""); // the variable-sized strings receive null bc they will have no trash printed later on writeFile
                                               // this happens bc the trash shall be printed after the last delimiter for variable-sized fields
                        break;
                }
            }

            // with tempField updated to null case if needed or not, this function puts the tempField onto the tempData respective field
            putCSVontoAccordingField(CSVfp, &tempData,tempField, flagSequence[i]);
            countRecords++;
        }

        // these fields are not on the csv, so we hard-define them as not removed(removido=0) and null for the integer encadeamento field(encadeamento=-1)
        tempData.removido = 0;
        tempData.encadeamento = -1;

        // with the temporary data record set-up, we write it onto the binary file
        writeDataRecord(binfp, &tempData);
    }

    // now that we have the whole information about the data record we can update our header
    head->nroPagDisco = countRecords*DATARECORDSIZE / CLUSTERSIZE;
    head->proxRRN = countRecords + 1;
    head->status = '1';


    // freeing some temp memory that was allocated
    free(tempLine);
    free(tempField);

}

// this function will get a buffer provided by strtok() and put it onto the according field following the fieldFlag sequence
// this includes allocating for variable size fields, strcpy for the static array of siglaPais and atoi for integer fields
void putCSVontoAccordingField(FILE* fp, DATARECORD* tempData, char* fieldBuff, int fieldFlag){
    switch (fieldFlag) // this will not switch between all possible fieldFlags since fieldFlags 0(removido) and 1(encadeamento) are invalid for this functionality
    {
        case 2: // idConecta field (fixed size as an int)
            tempData->idConecta = atoi(fieldBuff);
            break;
        case 3: // siglaPais field (fixed size as static array of lenght 3)
            strcpy(tempData->siglaPais,fieldBuff);
            tempData->siglaPais[2] = '\0';       
            break;

        case 4: // idPoPsConectado field(fixed size as an int)
            tempData->idPoPsConectado = atoi(fieldBuff);
            break;

        case 5: // unidadeMedida field(fixed size as a char)
            tempData->unidadeMedida = *(fieldBuff);
            break;

        case 6: // velocidade field(fixed size as an int)
            tempData->velocidade = atoi(fieldBuff);
            break;

        case 7: // nomePoPs field(variable size string)
            tempData->nomePoPs = malloc( (DATARECORDSIZE-20) * sizeof(char)); 
            tempData->nomePoPs = fieldBuff;
            tempData->nomePoPs = realloc(tempData->nomePoPs, fieldBuff);
            break;
        case 8: // nomePais field(variable size string)
            tempData->nomePais = malloc( (DATARECORDSIZE-20) * sizeof(char)); 
            tempData->nomePais = fieldBuff;
            tempData->nomePais = realloc(tempData->nomePoPs, fieldBuff);
            break;
        default:
            break;
        }
}

int readCSVLine(FILE *fp, char** strline){
    int nullFlag=1,i=0;
    char buffChar;

    while(1){
        nullFlag = fscanf(fp, "%c", &buffChar);
        if(nullFlag == 0) return 0;

        (*strline)[i] = buffChar;
        i++;
    }

    return 1;
}

void readFile(FILE* fp){
    int countRecords=0;
    DATARECORD tempData;
    HEADER tempHeader;

    readHeader(fp, &tempHeader); // AINDA FALTA IMPLEMENTAR

    while( readDataRecord(fp, &tempData) != 0){ // this reads a Record from fp and puts its data into tempData
        printData(tempData);     // AINDA FALTA IMPLEMENTAR      
        countRecords++;
    }

    return ;

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
            outData->nomePoPs = malloc(MAX_VARSTRINGSIZE * sizeof(char));

            int i=0;
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

            outData->nomePoPs = realloc(outData->nomePoPs, ( strlen(outData->nomePoPs) + 1 ) * sizeof(char));

            break;
        
        case 8: // nomePais Field

            outData->nomePais = malloc(MAX_VARSTRINGSIZE * sizeof(char));

            int i=0;
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

            outData->nomePais = realloc(outData->nomePais, ( strlen(outData->nomePais) + 1 ) * sizeof(char));

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

void writeHeaderField(FILE *fp, HEADER* hr, int fieldFlag){
    switch(fieldFlag){
        case 0: // status field
            fwrite(hr->status, sizeof(char),1,fp);
            break;
        case 1: // topoStack field
            fwrite(hr->topoStack, sizeof(int),1,fp);
            break;
        case 2: // proxRRN field
            fwrite(hr->proxRRN, sizeof(int),1,fp);
            break;
        case 3: // nroRegRem field
            fwrite(hr->nroRegRem, sizeof(int),1,fp);
            break;
        case 4: // nroPagDisco field
            fwrite(hr->nroPagDisco, sizeof(int),1,fp);
            break;
        case 5: // qttCompacta field
            fwrite(hr->qttCompacta, sizeof(int),1,fp);
            break;
    }

    return ;
}

void writeDataRecord(FILE *fp, DATARECORD* dr){
    int sizeWritten=0;
    // simply writes the 9 different field options
    for(int i=0;i<9;i++){
        sizeWritten += writeDataField(fp,dr,i);
    }
    int trashsize = DATARECORDSIZE - sizeWritten;
    char *trash;
    trash = malloc(trashsize * sizeof(char));
    for(int i=0;i<trashsize;i++){
        trash[i] = '$';
    }

    fwrite(trash, trashsize * sizeof(char), 1, fp); // writes the remaining trash onto the file
}

int writeDataField(FILE *fp, DATARECORD* dr,int fieldFlag){
    int sizeWritten=0;
    switch(fieldFlag){
        case 0: // removido Field
            fwrite(dr->removido, sizeof(int),1, fp);
            sizeWritten = sizeof(int);
            break;

        case 1: // encadeamento Field
            fwrite(dr->encadeamento, sizeof(int),1, fp);
            sizeWritten = sizeof(int);
            break;

        case 2: // idConecta Field
            fwrite(dr->encadeamento, sizeof(int), 1, fp);
            sizeWritten = sizeof(int);
            break;

        case 3: // siglaPais Field
            fwrite(dr->siglaPais,sizeof(char),2,fp); // writes 2 chars bc we do not write '\0' 
            sizeWritten = 2* sizeof(char);
            break;

        case 4: // idPoPsConectdo Field
            fwrite(dr->idPoPsConectado, sizeof(int),1,fp);
            sizeWritten = sizeof(int);
            break;
        case 5: // unidade Medida Field
            fwrite(dr->unidadeMedida, sizeof(char), 1, fp);
            sizeWritten = sizeof(char);
            break;
        
        case 6: // velocidade Field
            fwrite(dr->velocidade, sizeof(int),1, fp);
            sizeWritten = sizeof(int);
            break;
        
        case 7: // nomePoPs Field
            int i=0;
            while(i < strlen(dr->nomePoPs)){
                fwrite(dr->nomePoPs[i],sizeof(char),1,fp);
                sizeWritten+=sizeof(char);
            }
            fwrite('|',sizeof(char),1,fp); // after the variable size field we write the delimiter
            sizeWritten+=sizeof(char); // +1 byte written for '|'
            break;
        
        case 8: // nomePais Field
            int i=0;
            while(i < strlen(dr->nomePais)){
                fwrite(dr->nomePais[i],sizeof(char),1,fp);
                sizeWritten+=sizeof(char);
            }
            fwrite('|',sizeof(char),1,fp); // after the variable size field we write the delimiter
            sizeWritten+=sizeof(char); // +1 byte written for '|'
            break;
    }

    return sizeWritten;
    
}
