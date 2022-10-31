#include "../include/functionalities.h"

// this is the main funciton for functionality 1, which is based on CREATE TABLE from SQL
void functionality1(){
    //printf("inside functionality 1\n");
    HEADER h;
    h = newHeader();
    //printf("succesfully created new empty header\n");
    FILE* CSVfp, *binfp;
    char* csvFilepath, *binFilepath; // this will hold the filepaths inputted by keyboard to the corresponding files
    //printf("inputting filepaths\n");
    csvFilepath = inputStr();
    //printf("got the csv filepath as:%s\n", csvFilepath);

    binFilepath = inputStr();
    //printf("got the bin filepath as:%s\n", binFilepath);

    // openning both filepaths
    CSVfp = fopen(csvFilepath, "r");        // read a non-binary file
    if(CSVfp == NULL) {printOpenError(); return ;}
    binfp = fopen(binFilepath, "wb"); // write onto a binary file
    if(binfp == NULL) {printOpenError(); return ;}
    //printf("both files opened ok\n");

    h.status = '0'; // we set the header status as inconsistent now that the binary file is opened
    //printf("writing initial header\n");
    writeHeaderRecord(binfp,&h); //  we write the initial header, that has nothing but the flag for inconsistency
    //printf("initial header written ok\n");

    //printf("calling readCSV_writeBin now\n");
    // this larger function will input the CSV line, decompose it into the right struct fields and write the data records (updating the header while it does so)
    readCSV_writeBin(CSVfp, binfp, &h);
    //printf("readCSV_writeBin has gone out ok\n");

    // fseek to the beginning of the file to write the updated header
    //printf("going to fseek to write the header now\n");
    fseek(binfp,0,SEEK_SET);
    writeHeaderRecord(binfp,&h);
    //printf("written header ok, closing files\n");

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
    //printHeader(h);
    //int i=0;
    if(h.status == '0') {printOpenError(); return ;} //if the status field present in the header is equal to 0, we return an error warning
    while(readDataRecord(fp,&dr) != 0){ //loop to loop through binary file records
        //printf("***********\nRRN=%d\n",i++);
        //printf("has readen the first dataRecord as:\n");
        //printRecord(dr);
        if(dr.removido == '0'){ //if the record is not marked as removed, I display it on the screen
            hasFound = 1;
            printRecord(dr);
        }
        //printf("**********\n");
    }
    if(hasFound == 0){ // if there are no records
        printNoRecordError();
        countCluters = 1; // only the header has been readen
    }else{
        countCluters = h.nroPagDisco; //save the number of disk pages
    }
    printf("Numero de paginas de disco: %d\n\n", countCluters);

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
    //printf("got filepath as %s\n", binFilepath);
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
        printf("Busca %d\n",i+1);
        // we simply input which field will be searched as a string and transform it onto a fieldFlag
        scanf("%s", searchedField);
        fieldFlag = getFlag_fromDataField(searchedField);
        
        nRecords = searchFileAndPrint(fp, fieldFlag);
        nClusters = calculateNroPagDisco(nRecords);
        printf("Numero de paginas de disco: %d\n\n", nClusters);
    }

    fclose(fp);
    free(binFilepath);

    return ;
}
//this is the main feature of functionality 4 - in it the user searches for a record, 
//in the same way as he does in functionality 3, but in this functionality the searched record is removed
void functionality4(){
    HEADER headerHeader; 

    char* binFilepath;
    int fieldFlag;
    int removeRecords = 0;
    FILE* fp;
    // firstly, the input is given for filepath and the file is opened
    binFilepath = inputStr(); //binary file that is typed by the user
    //printf("got filepath as %s\n", binFilepath);
    fp = fopen(binFilepath, "rb+"); //opens the file allowing changes to be made to it
    if(fp == NULL) {printOpenError(); return ;} //if the file does not exist
     
    readHeader(fp, &headerHeader); 
    if(headerHeader.status == '0') {printOpenError(); return ;}

    headerHeader.status = '0';
    fseek(fp,0,SEEK_SET);
    writeHeaderRecord(fp,&headerHeader); //I update the header because I'm modifying the file

    // then, the input is given for the number of searches
    int nSearches;
    scanf("%d", &nSearches);

    // this loop will get the name of the field and then the search key
    char searchedField[MAXDATAFIELDNAME];  // none fixed-lenght fields are bigger than the maximum of a variable field, so this is the maximum lenght of any value of any field
    for(int i=0;i<nSearches;i++){
        if(i != 0) fseek(fp,CLUSTERSIZE,SEEK_SET); // if it is not the first search we shall reset the file pointer to start
        // we simply input which field will be searched as a string and transform it onto a fieldFlag
        scanf("%s", searchedField);
        fieldFlag = getFlag_fromDataField(searchedField);
        
        removeRecords += searchFileAndRemove(fp, &headerHeader,fieldFlag);
    }

    //printf("removeRecords %d\n", removeRecords);
    /*fseek(fp, 0, SEEK_SET);
    readHeader(fp, &headerHeader);
    printf("headerHeader.topoStack %d\n", headerHeader.topoStack);
    printf("headerHeader.proxRRN %d\n", headerHeader.proxRRN);
    printf("headerHeader.nroRegRem %d\n", headerHeader.nroRegRem);*/

    headerHeader.status = '1';
    fseek(fp,0,SEEK_SET);
    //printf("final header:\n");
    //printHeader(headerHeader);
    writeHeaderRecord(fp,&headerHeader); //// update the header because I have modified the records part of the file

    fclose(fp);
    binarioNaTela(binFilepath);
    free(binFilepath);

    return ;
}

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
        //printRecord(inputdr);

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

//feature 6 compresses the binary file, it permanently removes all records marked as removed 
//(which are filled with $(garbage)), making records that are not removed back to sequential
void functionality6(){
    HEADER headerHeader; 
    char* binFilepath;
    //int fieldFlag;
    //int removeRecords = 0; //UFA talvez eu não use
    FILE* fp;
    // firstly, the input is given for filepath and the file is opened
    binFilepath = inputStr();
    //printf("got filepath as %s\n", binFilepath);
    fp = fopen(binFilepath, "rb");
    if(fp == NULL) {
        printOpenError(); 
        return ;
    }

    readHeader(fp, &headerHeader); //read the header to modify it after compression
    if(headerHeader.status == '0') {printOpenError(); return ;}

    headerHeader.status = '0';
    fseek(fp,0,SEEK_SET);
    writeHeaderRecord(fp,&headerHeader);


    FILE*auxCompact;
    auxCompact = fopen("AUX.bin", "wb"); // we will only write on the new file

    compact(fp, auxCompact,&headerHeader); //I enter the function that will do the actual compression

    fclose(fp);
    fclose(auxCompact);

    remove(binFilepath);
    rename("AUX.bin",binFilepath);

    binarioNaTela(binFilepath);
    free(binFilepath);

    return ;
}