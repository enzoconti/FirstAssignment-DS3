#include "../include/functionalities.h"

// this is the main funciton for functionality 1, which is based on CREATE TABLE from SQL
void functionality1(){
    HEADER h;
    h = newHeader();
    FILE* CSVfp, binfp;
    char* csvFilepath, binFilepath; // this will hold the filepaths inputted by keyboard to the corresponding files
    inputFilePath(csvFilepath);
    inputFilePath(binFilepath);
    
    // openning both filepaths
    CSVfp = fopen(csvFilepath, "r");        // read a non-binary file
    if(CSVfp == NULL) printfOpenError();
    binFilepath = fopen(binFilepath, "wb"); // write onto a binary file
    if(binfp == NULL) printfOpenError();

    h.status = '0'; // we set the header status as inconsistent now that the binary file is opened
    writeHeaderRecord(binfp,&h); //  we write the initial header, that has nothing but the flag for inconsistency

    // this larger function will input the CSV line, decompose it into the right struct fields and write the data records (updating the header while it does so)
    readCSV_writeBin(CSVfp, binfp, &h);

    // fseek to the beginning of the file to write the updated header
    fseek(binfp,0,SEEK_SET);
    writeHeaderRecord(binfp,head);

    // closing the files
    fclose(CSVfp);
    fclose(binfp);

    // outputting onto terminal
    binarioNaTela(binfp);

    // freeing the allocated space
    free(csvFilepath);
    free(binFilepath);
}