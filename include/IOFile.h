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

char* inputFilepath();
void printOpenError();
void printRecord(DATARECORD dr);
void printHeader(HEADER h);

#endif