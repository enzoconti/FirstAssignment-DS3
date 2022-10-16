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

void functionality2(){

    //printf("inside functionality 2\n");
    HEADER h;
    DATARECORD d;
    //h = newHeader();

    //printf("succesfully created new empty header\n");
    FILE* /*CSVfp, **/binfp;
    char/** csvFilepath,*/ *binFilepath; // this will hold the filepaths inputted by keyboard to the corresponding files
    
    binFilepath = inputStr(); //pega o nome do arquivo

    binfp = fopen(binFilepath, "rb"); // write onto a binary file
    if(binfp == NULL) {printOpenError(); return ;} //CONFERIR SE EH ESSE MESMO ESSE ERRO QUE TEM QUE EXIBIR

    h = readHeader(binfp);
    int tamArq = 960;

    //fseek(binfp, 0, SEEK_END);
    //int tamTotalArq = ftell(binfp);
    //printf("Tamanho ftell(binfp): %d \n", tamTotalArq);
    //int nPaginasDisco = tamTotalArq/960;
    //printf("nPaginasDisco: %d \n", nPaginasDisco);
    //fseek(binfp, tamArq, SEEK_SET); 
    
    fseek(binfp, tamArq, SEEK_SET);
    //for(int i = 0; i < nPaginasDisco; i++){ //esse loop tem que ser arrumado para percorrer o arquivo todo
    while(readDataRecord(binfp, &d)){

        //printf("While ta rodando \n");
        //printf("---Reotrno da função: %d\n",readDataRecord(binfp, &d));
        fseek(binfp, tamArq, SEEK_SET);
        if(d.removido == '0'){
                    
            //int a = readDataRecord(binfp, &d); 
            //printf("\nDados \n");
            //printf("Removido: %c\n", d.removido);
            //printf("Encadeamento: %d\n", d.encadeamento);
            //
            if(d.idConecta != -1) printf("Identificador de ponto: %d\n", d.idConecta); //identificador de ponto
            if(d.nomePoPs[0] != '\0') printf("Nome do ponto: %s\n",d.nomePoPs); //nome do ponto
            //printf("entre d.nomePoPs e d.nomePais %d\n", d.nomePoPs[0]);
            if(d.nomePais[0] != '\0') printf("Pais de localizacao: %s\n",d.nomePais); //pais de localizacao
            if(d.siglaPais[0] != '$') printf("Sigla do pais: %s\n", d.siglaPais); //sigla do pais
            if(d.idPoPsConectado != -1) printf("Identificador do ponto conectado: %d\n", d.idPoPsConectado); //identificador do ponto conectado
            //printf("Unidade de medida: %c\n", d.unidadeMedida);
            if((d.velocidade != -1) && (d.unidadeMedida != '$')) printf("Velocidade de transmissao: %d %cbps\n", d.velocidade, d.unidadeMedida); //velocidade de transmicao 
            printf("\n");
            
            //unidadeMedida tem que ser um char
        }
        tamArq = tamArq + 64;
    }
    //os printf para exibir o header
    /*printf("Status: %c\n", h.status); 
    printf("Topo: %d\n", h.topoStack); 
    printf("proxRRN: %d\n", h.proxRRN); 
    printf("nroRegRem: %d\n", h.nroRegRem); 
    printf("nroPagDisco: %d\n", h.nroPagDisco); 
    printf("qttCompacta: %d\n", h.qttCompacta);*/  
    
    printf("Numero de paginas de disco: %d\n", h.nroPagDisco);
   
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