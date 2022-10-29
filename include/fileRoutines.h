#ifndef FILEROUTINES_H
#define FILEROUTINES_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
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
void readHeader(FILE* fp, HEADER*outh);
void readHeaderField(FILE* fp, HEADER* outh, int fieldFlag);
int searchFileAndPrint(FILE* fp,int fieldFlag,int func);
int searchIntOnFile(FILE* fp, int fieldFlag, int key);
int searchStrOnFile(FILE*fp, int fieldFlag, char* key);
int getFlag_fromDataField(char* searchedField);
void insert(FILE* fp, int endRRN, DATARECORD* inputDr,HEADER *h, int inputFlag);
char* removeSpaces(char* originalStr);
int getRRN4Insertion(FILE* fp, int*RRN,HEADER* h);
/*
void RRNread(char* filepath, int RRN);
*/

int removeIntOnFile(FILE* fp, int fieldFlag, int key);
int removeStrOnFile(FILE*fp, int fieldFlag, char* key);
<<<<<<< HEAD
void removeRegister(FILE *fp, int count);
void updateHeader(FILE *fp, int count);
void compact(FILE *fp, int numRemovidos);
int/*void*/ quantidadeRegistros(FILE *fp);
=======
void removeRegister(FILE *fp);
void compact(FILE *OriginalFp,FILE* auxCompact,HEADER* currentHeader);
// int/*void*/ quantidadeRegistros(FILE *fp);
>>>>>>> 922392d17b08b169700114f8d5580c32ff9cba5f

#endif