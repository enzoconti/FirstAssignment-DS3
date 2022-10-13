#include "../include/functionalities.h"

// this is the main funciton for functionality 1, which is based on CREATE TABLE from SQL
void functionality1(){
    printf("inside functionality 1\n");
    HEADER h;
    h = newHeader();
    printf("succesfully created new empty header\n");
    FILE* CSVfp, *binfp;
    char* csvFilepath, *binFilepath; // this will hold the filepaths inputted by keyboard to the corresponding files
    printf("inputting filepaths\n");
    csvFilepath = inputStr();
    printf("got the csv filepath as:%s\n", csvFilepath);

    binFilepath = inputStr();
    printf("got the bin filepath as:%s\n", binFilepath);

    // openning both filepaths
    CSVfp = fopen(csvFilepath, "r");        // read a non-binary file
    if(CSVfp == NULL) {printOpenError(); return ;}
    binfp = fopen(binFilepath, "wb"); // write onto a binary file
    if(binfp == NULL) {printOpenError(); return ;}
    printf("both files opened ok\n");

    h.status = '0'; // we set the header status as inconsistent now that the binary file is opened
    printf("writing initial header\n");
    writeHeaderRecord(binfp,&h); //  we write the initial header, that has nothing but the flag for inconsistency
    printf("initial header written ok\n");

    printf("calling readCSV_writeBin now\n");
    // this larger function will input the CSV line, decompose it into the right struct fields and write the data records (updating the header while it does so)
    readCSV_writeBin(CSVfp, binfp, &h);
    printf("readCSV_writeBin has gone out ok\n");

    // fseek to the beginning of the file to write the updated header
    printf("going to fseek to write the header now\n");
    fseek(binfp,0,SEEK_SET);
    writeHeaderRecord(binfp,&h);
    printf("written header ok, closing files\n");

    // closing the files
    fclose(CSVfp); // WHY THIS FCLOSE DOES NOT WORK????????????????????????
    fclose(binfp);

    // outputting onto terminal
    binarioNaTela(binFilepath);

    // freeing the allocated space
    free(csvFilepath);
    free(binFilepath);
}

void functionality2(){ //entrar com a struct???
    
    //vou printar a lista de struct aqui aaaaaa

    //acho que tenho que falar qual arquivo vai aqui -.-

    //calcular pagina de disco
    
    //fazer loop para percorrer todos coisos

    printf("inside functionality 2\n");
    HEADER h;
    DATARECORD d;
    //h = newHeader();

    printf("succesfully created new empty header\n");
    FILE* /*CSVfp, **/binfp;
    char/** csvFilepath,*/ *binFilepath; // this will hold the filepaths inputted by keyboard to the corresponding files
    
    binFilepath = inputStr();

    // openning both filepaths
    //CSVfp = fopen(csvFilepath, "r");        // read a non-binary file
    //if(CSVfp == NULL) {printOpenError(); return ;}
    binfp = fopen(binFilepath, "rb"); // write onto a binary file
    if(binfp == NULL) {printOpenError(); return ;} //CONFERIR SE EH ESSE MESMO ESSE ERRO QUE TEM QUE EXIBIR

    h = readHeader(binfp);
    readDataRecord(binfp, &d); //FILE *fp, DATARECORD* outData
    printf("Identificador de ponto: %d\n", d.idConecta);
    printf("Nome do ponto: %s\n",d.nomePoPs);
    printf("Pais de localizacao: %s\n",d.nomePais);
    printf("Sigla do pais %s\n", d.siglaPais);
    printf("Identificador do ponto conectado: %d\n", d.idPoPsConectado);
    printf("Velocidade de transmissao: %d %d\n", d.velocidade, d.unidadeMedida);


    printf("Numero de paginas de disco: %c\n", h.nroPagDisco);
}

void functionality3(){
    char* binFilepath;
    FILE* fp;
    // firstly, the input is given for filepath and the file is opened
    binFilepath = inputStr();
    printf("got filepath as %s\n", binFilepath);
    fp = fopen(binFilepath, "rb");

    // then, the input is given for the number of searches
    int nSearches;
    scanf("%d", &nSearches);

    // this loop will get the name of the field and then the search key
    char searchedField[MAXDATAFIELDNAME];
    char searchKey[MAX_VARSTRINGSIZE]; // none fixed-lenght fields are bigger than the maximum of a variable field, so this is the maximum lenght of any value of any field
    for(int i=0;i<nSearches;i++){
        // first the two inputs
        scanf("%s", searchedField);
        scan_quote_string(searchKey);

        printf("calling searchFileAndPrint with searchedField=%s and searchKey=%s\n", searchedField,searchKey);
        searchFileAndPrint(fp,searchedField,searchKey);
    }

    fclose(fp);
    free(binFilepath);

    return ;
}