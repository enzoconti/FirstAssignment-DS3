#ifndef FILEROUTINES_H
#define FILEROUTINES_H
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "funcoesFornecidas.h"
#include "functionalities.h"
#include "globalvalues.h"
#include "structCode.h"


void readFile(FILE* fp);
int readDataRecord(FILE *fp, DATARECORD* outData);
int readDataField(FILE* fp, int fieldFlag, DATARECORD* outData);
void readCSV_writeBin(FILE *CSVfp, FILE *binfp, HEADER *head);
void putCSVontoAccordingField(FILE* fp, DATARECORD* tempData, char* fieldBuff, int fieldFlag);
int readCSVLine(FILE *fp, char** strline);
void writeHeaderRecord(FILE *fp, HEADER* hr);
void writeHeaderField(FILE *fp, HEADER* hr, int fieldFlag);
void writeDataRecord(FILE *fp, DATARECORD* dr);
int writeDataField(FILE *fp, DATARECORD* dr,int fieldFlag);
/*
void RRNread(char* filepath, int RRN);
void writeRecord(FILE *fp, PERSON*p);
void writeField(FILE *fp, PERSON*p,int fieldFlag);
*/


#endif