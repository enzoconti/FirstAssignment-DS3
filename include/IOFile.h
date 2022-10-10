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
char *zstrtok(char *str, const char *delim);
void printRecord(DATARECORD dr);


#endif