#ifndef FILEROUTINES_H
#define FILEROUTINES_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "structCode.h"
#include "funcoesFornecidas.h"
#include "functionalities.h"
#include "globalvalues.h"



void readFile(FILE* fp);
int readDataRecord(FILE *fp, DATARECORD* outData);
//DATARECORD readDataRecord(FILE *fp/*, DATARECORD* outData*/);
int readDataField(FILE* fp, int fieldFlag, DATARECORD* outData);
void readCSV_writeBin(FILE *CSVfp, FILE *binfp, HEADER *head);
int readCSVRecord(FILE* CSVfp, DATARECORD* dr);
int readCSVField(FILE *CSVfp, DATARECORD*dr, int fieldFlag);
int isValid(FILE *fp,char c);
void writeHeaderRecord(FILE *fp, HEADER* hr);
void writeHeaderField(FILE *fp, HEADER* hr, int fieldFlag);
void writeDataRecord(FILE *fp, DATARECORD* dr);
int writeDataField(FILE *fp, DATARECORD* dr,int fieldFlag);
HEADER readHeader(FILE* fp);
void readHeaderField(FILE* fp, HEADER* outh, int fieldFlag);
//int readHeaderRecord(FILE *fp, HEADER* tempHeader);
//int readHeaderField(FILE* fp, int fieldFlag, HEADER* tempHeader);
void searchFileAndPrint(FILE* fp,char* searchedField, char* searchKey);
void searchIntOnFile(FILE* fp, int fieldFlag, int key);
void searchStrOnFile(FILE*fp, int fieldFlag, char* key);
int getFlag_fromDataField(char* searchedField);
/*
void RRNread(char* filepath, int RRN);
*/


#endif