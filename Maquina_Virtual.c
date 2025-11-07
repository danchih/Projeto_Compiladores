#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pilha.h"

FILE *arquivo;
int s = 0;  // contador do topo da pilha
int i = 0;  // contador do programa

char Funcao_LDC(char caracter){
    // Implementacao da caracter = funcao LDC
     

    return caracter;
}

char Analisa_Nome_Funcao(char caracter){
    char id[50];
    int i = 0;

    id[i++] = caracter;
    caracter = fgetc(arquivo);
    while (caracter >=65 && caracter <=90){
        id[i++] = caracter;
        caracter = fgetc(arquivo);
    }
    id[i] = '\0';

    if (strcmp(id, "LDC")== 0){
        caracter = Funcao_LDC(caracter);
    } else if (strcmp(id, "LDV")== 0){
        caracter = Funcao_LDV(caracter);
    } else if (strcmp(id, "STR")== 0){
        caracter = Funcao_STR(caracter);
    } else if (strcmp(id, "ADD")== 0){
        caracter = Funcao_ADD(caracter);
    } else if (strcmp(id, "SUB")== 0){
        caracter = Funcao_SUB(caracter);
    } else if (strcmp(id, "MULT")== 0){
        caracter = Funcao_MULT(caracter);
    } else if (strcmp(id, "DIVI")== 0){
        caracter = Funcao_DIVI(caracter);
    } else if (strcmp(id, "INV")== 0){
        caracter = Funcao_INV(caracter);
    } else if (strcmp(id, "AND")== 0){
        caracter = Funcao_AND(caracter);
    } else if (strcmp(id, "OR")== 0){
        caracter = Funcao_OR(caracter);
    } else if (strcmp(id, "NEG")== 0){
        caracter = Funcao_NEG(caracter);
    } else if (strcmp(id, "CME")== 0){
        caracter = Funcao_CME(caracter);
    } else if (strcmp(id, "CMA")== 0){
        caracter = Funcao_CMA(caracter);
    } else if (strcmp(id, "CEQ")== 0){
        caracter = Funcao_CEQ(caracter);
    } else if (strcmp(id, "CDIF")== 0){
        caracter = Funcao_CDIF(caracter);
    } else if (strcmp(id, "CMEQ")== 0){
        caracter = Funcao_CMEQ(caracter);
    } else if (strcmp(id, "CMAQ")== 0){
        caracter = Funcao_CMAQ(caracter);
    } else if (strcmp(id, "JMP")== 0){
        caracter = Funcao_JMP(caracter);
    } else if (strcmp(id, "JMPF")== 0){
        caracter = Funcao_JMPF(caracter);
    } else if (strcmp(id, "CALL")== 0){
        caracter = Funcao_CALL(caracter);
    } else if (strcmp(id, "RD")== 0){
        caracter = Funcao_RD(caracter);
    } else if (strcmp(id, "PRN")== 0){
        caracter = Funcao_PRN(caracter);
    } else if (strcmp(id, "START")== 0){
        caracter = Funcao_START(caracter);
    } else if (strcmp(id, "HLT")== 0){
        caracter = Funcao_HLT(caracter);
    } else if (strcmp(id, "DALLOC")== 0){
        caracter = Funcao_DALLOC(caracter);
    } else if (strcmp(id, "ALLOC")== 0){
        caracter = Funcao_ALLOC(caracter);
    } else {
        caracter = Funcao_RETURN(caracter);
    }

    return caracter;
}

char Analisador_Lexico(char caracter){

        // verifica se o T.caracter lido e space, enter, tab ou fim_arquivo
        while((caracter == 10 || caracter == 9 || caracter == 0) && caracter != EOF){
            caracter = fgetc(arquivo);
        }
        
        if(caracter != EOF){
            caracter = Analisa_Nome_Funcao(caracter);
        }

    return caracter;
}

int main()
{
    char caracter;

    arquivo = fopen("teste.txt", "r");

    caracter = fgetc(arquivo);

    fclose(arquivo);

    return 0;
}
