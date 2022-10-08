#include "../include/structCode.h"


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