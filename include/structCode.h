#ifndef STRUCTCODE_H
#define STRUCTCODE_H


#include "globalvalues.h"


#define HEADERSIZE 21 // this is the fixed size of the header record
// although, as it will always occupy and individual cluster(and the cluster on this project was predefined as having 960 bytes), it will always be stores as 960 bytes on the binary file 
 
 // creating a struct to represent the information on the header record
typedef struct header{
    char status;        // this fixed-size field represents the status of that header - it gets inconsistent when we open it to RAM and consistent when we update it back on disk
    int topoStack;      // this fixed-size field represents the top of the stack containing the removed data records
    int proxRRN;        // this fixed-size field represents the value of the next RRN available for new data records
    int nroRegRem;      // this fixed-size field represents the number of removed records
    int nroPagDisco;    // this fixed-size field represents the number of clusters("paginas de disco" in PT-BR) occupied by the data records
    int qttCompacta;    // this fixed-size field represents the number of times that this file has been compressed (done on functionality 6 by this program)
}HEADER;


#define DATARECORDSIZE 64 // this is the fixed size of the data record
#define MAX_VARSTRINGSIZE (64 - 21) // this is the maximum size any of the variable strings can have(without '\0');
#define MAXDATAFIELDNAME 15 // this is the maximum name of a data field that will be used in funcitonality 3
// creating a struct to represent the information on the data records
typedef struct dataRecord{
    char removido;          // this fixed-size control field represents if the data record has been removed
    int encadeamento;       // this fixed-size control field stores the RRN of the next removed record
    int idConecta;          // this fixed-size field is the id of the point of presence(PoPs) - must be different form 0
    char siglaPais[3];      // this fixed-size field is the 2 character acronym of the country in which the PoP is registered(+1 byte for '\0' that will not be recorded on file)
    int idPoPsConectado;    // this fixed-size field is the id of the connected PoPs
    char unidadeMedida;     // this fixed-size field is the measure unit for the transmission speed (a M character represents Mbps, for example)
    int velocidade;              // this fixed-size field is the speed of transmission
    char nomePoPs[MAX_VARSTRINGSIZE];         // this variable-size field is the name of the PoPs - its delimitant '\0' will not be stored to a file
    char nomePais[MAX_VARSTRINGSIZE];         // this variable-size field is the full name of the country - its delimitant '\0' will not be stored to a file
}DATARECORD;

HEADER newHeader();

#endif