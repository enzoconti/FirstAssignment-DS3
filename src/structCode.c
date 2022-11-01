#include "../include/structCode.h"
// this is a smaller file, but needed for modularization of functions that deal only with struct fields

// this is a constructor for header - used in functionality1 to create table
HEADER newHeader(){
   HEADER h;
   h.nroPagDisco = 0;
   h.nroRegRem = 0;
   h.proxRRN = 0;
   h.qttCompacta = 0;
   h.status = '1';
   h.topoStack = -1; 

   return h;
}

// this function calculates how many clusters a giver number of records occupy
int calculateNroPagDisco(int countRecords){
   int nroPagDisco=0;
   nroPagDisco = (countRecords * DATARECORDSIZE) / CLUSTERSIZE + 1;
   if((countRecords * DATARECORDSIZE) % CLUSTERSIZE != 0) nroPagDisco++;

   return nroPagDisco;
}
