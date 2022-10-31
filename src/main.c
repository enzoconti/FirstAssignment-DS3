/*
i.e. 11372883 - Jade Bortot de Paiva - 100%
i.e. 12547145 - Enzo serrano Conti - 100%
*/

#include "../include/structCode.h"
#include "../include/fileRoutines.h"
#include "../include/funcoesFornecidas.h"
#include "../include/functionalities.h"
#include "../include/globalvalues.h"
#include "../include/IOFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    int choice;
    scanf("%d",&choice);

    switch(choice){
        case 1:
            functionality1();
            break;
        case 2:
             functionality2();
            break;
        case 3:
             functionality3();
            break;
        case 4:
            functionality4();
            break;
        case 5:
            functionality5();
            break;
        case 6:
             functionality6();
            break;
    }

    return 0;
}