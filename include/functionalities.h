#ifndef FUNCTIONALITIES_H
#define FUNCTIONALITIES_H

#include "structCode.h"
#include "fileRoutines.h"
#include "funcoesFornecidas.h"
#include "globalvalues.h"
#include "IOFile.h"

// this will be the main function for functionality 1 - that reads a csv file and stores it on a binary file
void functionality1();

// this will be the main function for functionality 2 - that reads an entire binary file and displays it in an organized manner
void functionality2();

// this will be the main function for functionality 3 - that searches for records that match in an inputted field with an inputted key
void functionality3();

//this will be the main function for functionality 4 - which removes one or more records according to what data (can be any data in the record) enter using keyboard
void functionality4();

// this will be the main function for functionality 5 - that inputs a data record from keyboard and inserts onto a binary file
void functionality5();

//this will be the main function for feature 6 - it strips all records marked as removed from the file, making the file smaller and less junk
void functionality6();

#endif
