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



void functionality3(){
    char* binFilepath;
    FILE* fp;
    // firstly, the input is given for filepath and the file is opened
    binFilepath = inputStr();
    fp = fopen(binFilepath, 'rb');

    // then, the input is given for the number of searches
    int nSearches;
    scanf("%d", &nSearches);

    // this loop will get the name of the field and then the search key
    char searchedField[MAXDATAFIELDNAME];
    char searchKey[MAX_VARSTRINGSIZE]; // none fixed-lenght fields are bigger than the maximum of a variable field, so this is the maximum lenght of any value of any field
    DATARECORD foundRecord;
    HEADER h;
    for(int i=0;i<nSearches;i++){
        // first the two inputs
        fgets(searchedField,15,stdin);
        scan_quote_string(searchKey);

        h = readHeader(fp);
        foundRecord = searchFile(fp,searchedField,searchKey);

        printSearchResult(foundRecord,i+1,h);
    }

    return ;
}