#include "../include/functionalities.h"

// this is the main funciton for functionality 1, which is based on CREATE TABLE from SQL
void functionality1(){
    HEADER h;
    h = newHeader();
    FILE* CSVfp, *binfp;
    char* csvFilepath, *binFilepath; // this will hold the filepaths inputted by keyboard to the corresponding files
    csvFilepath = inputStr();
    binFilepath = inputStr();

    // openning both filepaths
    CSVfp = fopen(csvFilepath, "r");        // read a non-binary file
    if(CSVfp == NULL) {printOpenError(); return ;}
    binfp = fopen(binFilepath, "wb"); // write onto a binary file
    if(binfp == NULL) {printOpenError(); return ;}

    h.status = '0'; // we set the header status as inconsistent now that the binary file is opened
    writeHeaderRecord(binfp,&h); //  we write the initial header, that has nothing but the flag for inconsistency

    // this larger function will input the CSV field by field, composing them into records to be written(updating the header while it does so)
    readCSV_writeBin(CSVfp, binfp, &h);
    fseek(binfp,0,SEEK_SET);
    writeHeaderRecord(binfp,&h);

    // closing the files
    fclose(CSVfp);
    fclose(binfp);

    // outputting onto terminal
    binarioNaTela(binFilepath);

    // freeing the allocated space
    free(csvFilepath);
    free(binFilepath);
}

//this is the main feature of functionality 2, here the binary file typed by the user is traversed so that all records are read and printed on the screen
void functionality2(){
    FILE*fp;
    DATARECORD dr;
    HEADER h;
    int hasFound=0;
    int countCluters=0;
    char*binFilepath; // this will hold the filepath inputted by keyboard to the corresponding file
    binFilepath = inputStr(); //get the file name
    
    fp = fopen(binFilepath, "rb"); // read onto a binary file
    if(fp == NULL) {printOpenError(); return ;} //if the file is empty, we return an error warning
    
    readHeader(fp, &h); //header reading
    if(h.status == '0') {printOpenError(); return ;} //if the status field present in the header is equal to 0, we return an error warning
    
    while(readDataRecord(fp,&dr) != 0){ //loop to loop through binary file records
        if(dr.removido == '0'){ //if the record is not marked as removed, I display it on the screen
            hasFound = 1;
            printRecord(dr);
        }
    }
    if(hasFound == 0){ // if there are no records
        printNoRecordError();
        countCluters = 1; // only the header has been readen
    }else{
        countCluters = h.nroPagDisco; //save the number of disk pages
    }
    printf("Numero de paginas de disco: %d\n\n", countCluters); // output formatting

    fclose(fp); 
    free(binFilepath);
}

void functionality3(){
    char* binFilepath;
    int fieldFlag,nRecords=0,nClusters=0;
    FILE* fp;
    HEADER h;   
    // firstly, the input is given for filepath and the file is opened
    binFilepath = inputStr();
    fp = fopen(binFilepath, "rb");
    if(fp == NULL) {printOpenError(); return ;} 

    // then, the input is given for the number of searches
    int nSearches;
    scanf("%d", &nSearches);

    readHeader(fp,&h);
    if(h.status == '0') {printOpenError(); return;}

    // this loop will get the name of the field and then the search key
    char searchedField[MAXDATAFIELDNAME];  // none fixed-lenght fields are bigger than the maximum of a variable field, so this is the maximum lenght of any value of any field
    for(int i=0;i<nSearches;i++){
        if(i != 0) fseek(fp,CLUSTERSIZE,SEEK_SET); // if it is not the first search we shall reset the file pointer to right after header
        
        printf("Busca %d\n",i+1); // output formatting

        // we simply input which field will be searched as a string and transform it onto a fieldFlag
        scanf("%s", searchedField);
        fieldFlag = getFlag_fromDataField(searchedField);
        
        // this larges function searches the key and prints it, returning the number of Records it has readen
        nRecords = searchFileAndPrint(fp, fieldFlag);
        nClusters = calculateNroPagDisco(nRecords);

        printf("Numero de paginas de disco: %d\n\n", nClusters); // output formatting
    }

    fclose(fp);
    free(binFilepath);

    return ;
}
// this is the main feature of functionality 4 - in it the user searches for a record, 
// in the same way as he does in functionality 3, but in this functionality the searched record is removed
void functionality4(){
    HEADER headerHeader; 

    char* binFilepath;
    int fieldFlag;
    int removeRecords = 0;
    FILE* fp;
    // firstly, the input is given for filepath and the file is opened
    binFilepath = inputStr(); //binary file that is typed by the user
    fp = fopen(binFilepath, "rb+"); //opens the file allowing changes to be made to it
    if(fp == NULL) {printOpenError(); return ;} //if the file does not exist
     
    readHeader(fp, &headerHeader); 
    if(headerHeader.status == '0') {printOpenError(); return ;}

    headerHeader.status = '0';
    fseek(fp,0,SEEK_SET);
    writeHeaderRecord(fp,&headerHeader); //we update the header because we are modifying the file

    // then, the input is given for the number of searches
    int nSearches;
    scanf("%d", &nSearches);

    // this loop will get the name of the field and then the search key
    char searchedField[MAXDATAFIELDNAME];  // this is the name(Str) of the searched field, which will be converted onto a flag
    for(int i=0;i<nSearches;i++){
        if(i != 0) fseek(fp,CLUSTERSIZE,SEEK_SET); // if it is not the first search we shall reset the file pointer to start
        
        // we simply input which field will be searched as a string and transform it onto a fieldFlag
        scanf("%s", searchedField);
        fieldFlag = getFlag_fromDataField(searchedField);
        
        removeRecords += searchFileAndRemove(fp, &headerHeader,fieldFlag);
    }

    headerHeader.status = '1';
    fseek(fp,0,SEEK_SET);

    writeHeaderRecord(fp,&headerHeader); // update the header

    fclose(fp);
    binarioNaTela(binFilepath);
    free(binFilepath);

    return ;
}

// this is the main function for functionality 5 - which inserts records
void functionality5(){
    DATARECORD inputdr;
    FILE* fp;
    HEADER hr;
    int n; // the number of inputted records to be added
    char* filepath;
    int RRN2badded, inputFlag;

    filepath = inputStr();

    fp = fopen(filepath,"r+b");// we need to read and then write a binary file WITHOUT deleting the old records(thats why r+b and not w+b)
    if(fp == NULL) {printOpenError(); return ;} 
    scanf("%d", &n);

    // we read the header and rewrite it to inconsistent
    readHeader(fp, &hr);
    if(hr.status == '0') {printOpenError(); return ;} // if it is already inconsistent there is an error
    
    hr.status = '0'; // we are modifying the header, so we put it as inconsistent and rewrite on the file
    // rewriting the header as inconsistent
    fseek(fp,0,SEEK_SET);
    writeHeaderRecord(fp,&hr);

    for(int i=0;i<n;i++){
        inputDataRecord(&inputdr);

        inputFlag = getRRN4Insertion(fp,&RRN2badded,&hr);
        
        insert(fp,RRN2badded,&inputdr,&hr,inputFlag); // this inserts inputdr
        
    }

    hr.status = '1';
    fseek(fp,0,SEEK_SET);
    writeHeaderRecord(fp,&hr);


    fclose(fp);
    binarioNaTela(filepath);
    free(filepath);
}

// functionality 6 compresses the binary file, it permanently removes all records marked as removed 
//(which are filled with $(garbage)), making records that are not removed back to sequential
void functionality6(){
    HEADER headerHeader; 
    char* binFilepath;
    FILE* fp;
    // firstly, the input is given for filepath and the file is opened
    binFilepath = inputStr();
    fp = fopen(binFilepath, "rb");
    if(fp == NULL) {
        printOpenError(); 
        return ;
    }

    readHeader(fp, &headerHeader); //read the header to modify it after compression
    if(headerHeader.status == '0') {printOpenError(); return ;}

    // update the header as inconsistent
    headerHeader.status = '0';
    fseek(fp,0,SEEK_SET);
    writeHeaderRecord(fp,&headerHeader);


    FILE*auxCompact;
    auxCompact = fopen("AUX.bin", "wb"); // we will only write on the new file

    compact(fp, auxCompact,&headerHeader); // we enter the function that will do the actual compression

    // headerHeader comes updated from compact
    fseek(auxCompact,0,SEEK_SET);
    writeHeaderRecord(auxCompact,&headerHeader);

    fclose(fp);
    fclose(auxCompact);

    // we remove the original file and rename th enew one to the original name
    // for the user, there has been no change on filepaths
    remove(binFilepath);
    rename("AUX.bin",binFilepath);

    binarioNaTela(binFilepath);
    free(binFilepath);

    return ;
}