#ifndef IOFILE_H
#define IOFILE_h

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "fileRoutines.h"
#include "funcoesFornecidas.h"
#include "functionalities.h"
#include "globalvalues.h"
#include "structCode.h"

char* inputStr();
void printOpenError();
void printRecord(DATARECORD dr);
void printHeader(HEADER h);
void printNoRecordError();
void inputDataField(DATARECORD* dr, int fieldFlag);
void inputDataRecord(DATARECORD* dr);
#endif