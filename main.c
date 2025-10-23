#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *arquivo;
int linha = 1;

typedef struct{
    char lexema[50];
    char simbolo[50];
    char caracter;
}Token;

// Prototipos das funcoes ==================================================================================================
// Analisadores Lexicos
Token trata_digito(Token T);
Token trata_pontuacao(Token T);
Token trata_operador_aritmetico(Token T);
Token trata_operador_relacional(Token T);
Token trata_atribuicao(Token T);
Token trata_identificador_palavra(Token T);
Token Pega_Token(Token T);
Token Analisador_Lexico(Token T);
// Analisadores Sintaticos
void Analisador_sintatico(Token T);
Token Analisa_bloco(Token T);
Token Analisa_et_variavel(Token T);
Token Analisa_variaveis(Token T);
Token Analisa_Tipo(Token T);
Token Analisa_subrotinas(Token T);
Token Analisa_declaracao_procedimento(Token T);
Token Analisa_declaracao_funcao(Token T);
Token Analisa_comandos(Token T);
Token Analisa_comando_simples(Token T);
Token Analisa_atrib_chprocedimento(Token T);
Token Analisa_comando_atribuicao(Token T);
Token Analisa_chamada_procedimento(Token T);
Token Analisa_se(Token T);
Token Analisa_enquanto(Token T);
Token Analisa_leia(Token T);
Token Analisa_escreva(Token T);
Token Analisa_expressao(Token T);
Token Analisa_expressao_simples(Token T);
Token Analisa_termo(Token T);
Token Analisa_fator(Token T);
Token Analisa_chamada_funcao(Token T);


// Funcao Principal =========================================================================================================

int main()
{
    arquivo = fopen("teste.txt", "r");
    Token T;

    T.caracter = fgetc(arquivo);

    Analisador_sintatico(T);

    fclose(arquivo);

    return 0;
}

// Funcoes Desenvolvidas ====================================================================================================
// Funcoes Lexicas ==========================================================================================================

Token trata_digito(Token T){
    char num[50];
    int i = 0;
    strcpy(T.simbolo, "snumero");

    num[i++] = T.caracter;
    T.caracter = fgetc(arquivo);

    // Verifica se T.caracter esta entre 0 a 9
    while (T.caracter >= 48 && T.caracter <=57) {
        num[i++] = T.caracter;
        T.caracter = fgetc(arquivo);
    }
    num[i] = '\0';
    strcpy(T.lexema, num);
    return T;
}

Token trata_pontuacao(Token T){

    if (T.caracter == 59){
        strcpy(T.simbolo, "sponto_virgula");
    } else if (T.caracter == 44){
        strcpy(T.simbolo, "svirgula");
    } else if (T.caracter == 40){
        strcpy(T.simbolo, "sabre_parenteses");
    } else if (T.caracter == 41){
        strcpy(T.simbolo, "sfecha_parenteses");
    } else if (T.caracter == 46){
        strcpy(T.simbolo, "sponto");
    }
    T.lexema[0] = T.caracter;
    T.lexema[1] = '\0';
    
    T.caracter = fgetc(arquivo);
    return T;
}

Token trata_operador_aritmetico(Token T){

    // Verifica se caracter e +
    if(T.caracter == 43){
        strcpy(T.simbolo, "smais");
    // Verifica se caracter e -
    }else if(T.caracter == 45){
        strcpy(T.simbolo, "smenos");
    // Verifica se caracter e *
    }else{
        strcpy(T.simbolo, "smult");
    }
    T.lexema[0] = T.caracter;
    T.lexema[1] = '\0';
    
    T.caracter = fgetc(arquivo);
    return T;
}

Token trata_operador_relacional(Token T){
    const char *simbolo;
    char dois_caracter[3];
    int i = 0;

    dois_caracter[i++] = T.caracter;

    // Verifica se caracter e <
    if (T.caracter == 60){
        T.caracter = fgetc(arquivo);
        // Verifica se caracter e <=
        if (T.caracter == 61){
            dois_caracter[i++] = T.caracter;
            simbolo = "smenorig";
            T.caracter = fgetc(arquivo);
        } else {
            simbolo = "smenor";
        }

    // Verifica se caracter e >
    } else if (T.caracter == 62){
        T.caracter = fgetc(arquivo);
        // Verifica se caracter e >=
        if (T.caracter == 61){
            dois_caracter[i++] = T.caracter;
            simbolo = "smaiorig";
            T.caracter = fgetc(arquivo);
        } else {
            simbolo = "smaior";
        }

    // Verifica se caracter e !
    } else if (T.caracter == 33){
        T.caracter = fgetc(arquivo);
        // Verifica se caracter e igual a !=
        if (T.caracter == 61){
            dois_caracter[i++] = T.caracter;
            simbolo = "sdif";
            T.caracter = fgetc(arquivo);
        } else {
            printf("Erro: Ausencia do = para o lexema != \n");
        }
    // Verifica se caracter e =
    } else {
        simbolo = "sig";
        T.caracter = fgetc(arquivo);
    }
    dois_caracter[i] = '\0';
    strcpy(T.lexema, dois_caracter);
    strcpy(T.simbolo, simbolo);

    return T;
}

Token trata_atribuicao(Token T){
    char atribuicao[3];
    int i = 0;

    atribuicao[i++] = T.caracter;
    T.caracter = fgetc(arquivo);
    if(T.caracter == 61){
        atribuicao[i++] = T.caracter;
        strcpy(T.simbolo, "satribuicao");
        T.caracter = fgetc(arquivo);
    } else {
        strcpy(T.simbolo, "sdoispontos");
    }
    atribuicao[i] = '\0';
    strcpy(T.lexema, atribuicao);

    return T;
}

Token trata_identificador_palavra(Token T){
    char id[50];
    int i = 0;

    id[i++] = T.caracter;
    T.caracter = fgetc(arquivo);
    while ((T.caracter >= 48 && T.caracter <=57) || ((T.caracter >= 97 && T.caracter <=122) || (T.caracter >=65 && T.caracter <=90)) || (T.caracter == 95)){
        id[i++] = T.caracter;
        T.caracter = fgetc(arquivo);
    }
    id[i] = '\0';

    if (strcmp(id, "se")== 0){
        strcpy(T.simbolo, "sse");
    } else if (strcmp(id, "programa")== 0){
        strcpy(T.simbolo, "sprograma");
    } else if (strcmp(id, "entao")== 0){
        strcpy(T.simbolo, "sentao");
    } else if (strcmp(id, "senao")== 0){
        strcpy(T.simbolo, "ssenao");
    } else if (strcmp(id, "enquanto")== 0){
        strcpy(T.simbolo, "senquanto");
    } else if (strcmp(id, "faca")== 0){
        strcpy(T.simbolo, "sfaca");
    } else if (strcmp(id, "inicio")== 0){
        strcpy(T.simbolo, "sinicio");
    } else if (strcmp(id, "fim")== 0){
        strcpy(T.simbolo, "sfim");
    } else if (strcmp(id, "escreva")== 0){
        strcpy(T.simbolo, "sescreva");
    } else if (strcmp(id, "leia")== 0){
        strcpy(T.simbolo, "sleia");
    } else if (strcmp(id, "var")== 0){
        strcpy(T.simbolo, "svar");
    } else if (strcmp(id, "inteiro")== 0){
        strcpy(T.simbolo, "sinteiro");
    } else if (strcmp(id, "booleano")== 0){
        strcpy(T.simbolo, "sbooleano");
    } else if (strcmp(id, "verdadeiro")== 0){
        strcpy(T.simbolo, "sverdadeiro");
    } else if (strcmp(id, "falso")== 0){
        strcpy(T.simbolo, "sfalso");
    } else if (strcmp(id, "procedimento")== 0){
        strcpy(T.simbolo, "sprocedimento");
    } else if (strcmp(id, "funcao")== 0){
        strcpy(T.simbolo, "sfuncao");
    } else if (strcmp(id, "div")== 0){
        strcpy(T.simbolo, "sdiv");
    } else if (strcmp(id, "e")== 0){
        strcpy(T.simbolo, "se");
    } else if (strcmp(id, "ou")== 0){
        strcpy(T.simbolo, "sou");
    } else if (strcmp(id, "nao")== 0){
        strcpy(T.simbolo, "snao");
    } else {
        strcpy(T.simbolo, "sidentificador");
    }
    strcpy(T.lexema, id);
    
    //T.caracter = fgetc(arquivo);
    return T;
}

Token Pega_Token(Token T){
    if(T.caracter >= 48 && T.caracter <=57){
        T = trata_digito(T);
    } else if ((T.caracter >= 97 && T.caracter <=122) || (T.caracter >=65 && T.caracter <=90)){
        T = trata_identificador_palavra(T);
    } else if (T.caracter == 58){
        T = trata_atribuicao(T);
    } else if ((T.caracter == 42) || (T.caracter == 43) || (T.caracter == 45)){
        T = trata_operador_aritmetico(T);
    } else if ((T.caracter >= 60 && T.caracter <=62) || (T.caracter == 33)){
        T = trata_operador_relacional(T);
    } else if ((T.caracter == 59) || (T.caracter == 44) || (T.caracter == 41) || (T.caracter == 46)|| (T.caracter == 40)){
        T = trata_pontuacao(T);
    } else {
        printf("Erro: caracter nao encontrado\n");
        T.caracter = fgetc(arquivo);

    }
    return T;
}

Token Analisador_Lexico(Token T){

        // verifica se o T.caracter lido e { , space, enter ou fim_arquivo
        while((T.caracter == 123 || T.caracter == 32 || T.caracter == 10) && T.caracter != EOF){
            if(T.caracter == 123){
                while(T.caracter != 125 && T.caracter != EOF){
                    T.caracter = fgetc(arquivo);
                }
            }
            while((T.caracter == 32 || T.caracter == 10 || T.caracter == 9 || T.caracter == 0 || T.caracter == 125) && T.caracter != EOF){
                if(T.caracter == 10){
                    linha += 1;
                }
                T.caracter = fgetc(arquivo);
            }
        }
        if(T.caracter != EOF){
            T = Pega_Token(T);
        }

    return T;
}

// Funcoes Sintaticas =======================================================================================================

void Analisador_sintatico(Token T){
    T = Analisador_Lexico(T);
    if(strcmp(T.simbolo, "sprograma")== 0){
        T = Analisador_Lexico(T);
        if(strcmp(T.simbolo, "sidentificador")== 0){
            T = Analisador_Lexico(T);
            if(strcmp(T.simbolo, "sponto_virgula")== 0){
                T = Analisa_bloco(T);
                if(strcmp(T.simbolo, "sponto")== 0){
                    T = Analisador_Lexico(T);
                    if(T.caracter == EOF){
                        printf("Termino de programa\n");
                    } else {
                        printf("[%d] Erro ao terminar programa --> (lexema recebido: '%s')\n", linha, T.lexema);
                    }
                } else {
                    printf("[%d] Erro: falta de . apos o termino do bloco --> (lexema recebido: '%s')\n", linha, T.lexema);
                }
            } else {
                printf("[%d] Erro: falta de ; ao iniciar o programa --> (lexema recebido: '%s')\n", linha, T.lexema);
            }
        } else {
            printf("[%d] Erro ao identificar programa --> (lexema recebido: '%s')\n", linha, T.lexema);
        }
    } else {
        printf("[%d] Erro ao iniciar programa --> (lexema recebido: '%s')\n", linha, T.lexema);
    }
}

Token Analisa_bloco(Token T){
    T = Analisador_Lexico(T);

    while ((strcmp(T.simbolo, "svar")!= 0) &&
           (strcmp(T.simbolo, "sprocedimento")!= 0) &&
           (strcmp(T.simbolo, "sfuncao")!= 0) &&
           (strcmp(T.simbolo, "sinicio") != 0) &&
           (T.caracter != EOF)) {

        printf("[%d] Aviso: simbolo inesperado '%s' -> ignorado\n", linha, T.lexema);

        T = Analisador_Lexico(T);
    }
    if (T.caracter == EOF) {
        printf("[%d] Erro: fim de arquivo inesperado ---> Erro ao iniciar bloco --> (lexema recebido: '%s')\n", linha, T.lexema);
        return T;
    }

    if (strcmp(T.simbolo, "svar") == 0){
        T = Analisa_et_variavel(T);
    }
    if ((strcmp(T.simbolo, "sprocedimento") == 0) || (strcmp(T.simbolo, "sfuncao") == 0)){
        T = Analisa_subrotinas(T);
    }
    if (strcmp(T.simbolo, "sinicio") == 0){
        T = Analisa_comandos(T);
    }
    return T;
}


Token Analisa_et_variavel(Token T){
    if(strcmp(T.simbolo, "svar")== 0){
        T = Analisador_Lexico(T);
        if(strcmp(T.simbolo, "sidentificador")== 0){
            while(strcmp(T.simbolo, "sidentificador")== 0){
                T = Analisa_variaveis(T);
                if(strcmp(T.simbolo, "sponto_virgula")== 0){
                    T = Analisador_Lexico(T);
                } else {
                    printf("[%d] Erro: falta ; apos a declaracao das variavel --> (lexema recebido: '%s')\n", T.lexema);
                }
            }
        } else {
            printf("[%d] Erro ao declarar variavel --> (lexema recebido: '%s')\n", linha, T.lexema);
        }
    }
    return T;
}

Token Analisa_variaveis(Token T){
    do{
        if(strcmp(T.simbolo, "sidentificador") == 0){
            T = Analisador_Lexico(T);
            if((strcmp(T.simbolo, "svirgula")== 0) || (strcmp(T.simbolo, "sdoispontos")== 0)){
                if(strcmp(T.simbolo, "svirgula") == 0){
                    T = Analisador_Lexico(T);
                    if(strcmp(T.simbolo, "sdoispontos") == 0)
                        printf("[%d] Erro: falta de identificador apos virgula --> (lexema recebido: '%s')\n", linha, T.lexema);
                }
            }
        } else {
            printf("[%d] Erro: declaracao de variavel incorreta --> (lexema recebido: '%s')\n", linha, T.lexema);
        }
    }while(strcmp(T.simbolo, "sdoispontos") != 0 && strcmp(T.simbolo, "sponto_virgula") != 0 && T.caracter != EOF);

    if(strcmp(T.simbolo, "sdoispontos") != 0){
        printf("[%d] Erro: falta ':' na declaracao de variavel --> (lexema recebido: '%s')\n", linha, T.lexema);
    }
    T = Analisador_Lexico(T);
    T = Analisa_Tipo(T);
    return T;
}

Token Analisa_Tipo(Token T){
    if((strcmp(T.simbolo, "sinteiro") != 0) && (strcmp(T.simbolo, "sbooleano") != 0)){
        printf("[%d] Erro: tipo de variavel incorreto --> (lexema recebido: '%s')\n", linha, T.lexema);
    }
    T = Analisador_Lexico(T);
    return T;
}

Token Analisa_subrotinas(Token T){
    while(strcmp(T.simbolo, "sprocedimento")== 0 || strcmp(T.simbolo, "sfuncao")== 0) {
       if(strcmp(T.simbolo, "sprocedimento")== 0){
        T = Analisa_declaracao_procedimento(T);
       }
       if (strcmp(T.simbolo, "sfuncao")== 0){
        T = Analisa_declaracao_funcao(T);
       }
       if(strcmp(T.simbolo, "sponto_virgula")== 0){
            T = Analisador_Lexico(T);
       } else {
            printf("[%d] Erro: falta ; apos a declaracao da subrotina --> (lexema recebido: '%s')\n", linha, T.lexema);
       }
    }
    return T;
}

Token Analisa_declaracao_procedimento(Token T){
    T = Analisador_Lexico(T);
    if(strcmp(T.simbolo, "sidentificador")== 0){
        T = Analisador_Lexico(T);
        if(strcmp(T.simbolo, "sponto_virgula")== 0){
            T = Analisa_bloco(T);
        } else {
            printf("[%d] Erro: falta ; na declaracao do procedimento --> (lexema recebido: '%s')\n", linha, T.lexema);
        }
    } else {
        printf("[%d] Erro ao identificar a declaracao do procedimento\n", linha, T.lexema);
    }

    return T;
}

Token Analisa_declaracao_funcao(Token T){
    T = Analisador_Lexico(T);
    if(strcmp(T.simbolo, "sidentificador")== 0){
        T = Analisador_Lexico(T);
        if(strcmp(T.simbolo, "sdoispontos")== 0){
            T = Analisador_Lexico(T);
            if((strcmp(T.simbolo, "sinteiro")== 0) || (strcmp(T.simbolo, "sbooleano")== 0)){
                T = Analisador_Lexico(T);
                if(strcmp(T.simbolo, "sponto_virgula")== 0){
                    T = Analisa_bloco(T);
                }
                else{
                    printf("[%d] Erro: falta ; na declaracao da funcao --> (lexema recebido: '%s')\n", linha, T.lexema);
                }
            } else {
                printf("[%d] Erro: Tipo nao correspondente (inteiro/booleano) na declaracao da funcao --> (lexema recebido: '%s')\n", linha, T.lexema);
            }
        } else {
            printf("[%d] Erro: ausencia de : na declaracao da funcao --> (lexema recebido: '%s')\n", linha, T.lexema);
        }
    } else {
        printf("[%d] Erro ao identificar a declaracao da funcao --> (lexema recebido: '%s')\n", linha, T.lexema);
    }

    return T;
}

Token Analisa_comandos(Token T) {
    if (strcmp(T.simbolo, "sinicio") == 0) {
        T = Analisador_Lexico(T);
        T = Analisa_comando_simples(T);

        while (strcmp(T.simbolo, "sfim") != 0 && T.caracter != EOF) {
            if (strcmp(T.simbolo, "sponto_virgula") == 0) {
                T = Analisador_Lexico(T);
                if (strcmp(T.simbolo, "sfim") != 0) {
                    T = Analisa_comando_simples(T);
                }
            } else {
                printf("[%d] Erro: esperado ';' ao final da analise do comando --> (lexema recebido: '%s')\n", linha, T.lexema);
                T = Analisador_Lexico(T);
            }
        }

        if (strcmp(T.simbolo, "sfim") == 0) {
            T = Analisador_Lexico(T);
        } else {
            printf("[%d] Erro: esperado 'fim' ao finalizar comando --> (lexema recebido: '%s')\n", linha, T.lexema);
        }
    } else {
        printf("[%d] Erro: esperado 'inicio' ao inicializar comando --> (lexema recebido: '%s')\n", linha, T.lexema);
    }

    return T;
}

Token Analisa_comando_simples(Token T){
    if(strcmp(T.simbolo, "sidentificador")==0){
        T = Analisa_atrib_chprocedimento(T);
    } else if(strcmp(T.simbolo, "sse")==0){
        T = Analisa_se(T);
    } else if(strcmp(T.simbolo, "senquanto")==0){
        T = Analisa_enquanto(T);
    } else if(strcmp(T.simbolo, "sleia")==0){
        T = Analisa_leia(T);
    } else if(strcmp(T.simbolo, "sescreva")==0){
        T = Analisa_escreva(T);
    } else if(strcmp(T.simbolo, "sinicio")==0){
        T = Analisa_comandos(T);
    } else{
        printf("[%d] Erro: comando invalido  --> (lexema recebido: '%s')\n", linha, T.lexema);
    }
    return T;
}

Token Analisa_atrib_chprocedimento(Token T){
    Token T1 = Analisador_Lexico(T);
    if(strcmp(T1.simbolo, "satribuicao")== 0){
        T = Analisa_comando_atribuicao(T1);
    } else {
        T = Analisa_chamada_procedimento(T);
    }
    return T;
}

Token Analisa_comando_atribuicao(Token T){
    T = Analisador_Lexico(T);
    T = Analisa_expressao(T);
    return T;
}

Token Analisa_chamada_procedimento(Token T){
    if(strcmp(T.simbolo, "sidentificador")== 0){
       T = Analisador_Lexico(T);
    } else {
        printf("[%d] Erro ao identificar chamada de procedimento --> (lexema recebido: '%s')\n", linha, T.lexema);
    }
    return T;
}

Token Analisa_se(Token T){
    T = Analisador_Lexico(T);
    T = Analisa_expressao(T);
    if(strcmp(T.simbolo, "sentao")== 0){
        T = Analisador_Lexico(T);
        T = Analisa_comando_simples(T);
        if(strcmp(T.simbolo, "ssenao")== 0){
            T = Analisador_Lexico(T);
            T = Analisa_comando_simples(T);
        }
    } else {
        printf("[%d] Erro sintatico do comando SE (ausencia do ENTAO) --> (lexema recebido: '%s')\n", linha, T.lexema);
    }
    return T;
}

Token Analisa_enquanto(Token T){
    T = Analisador_Lexico(T);
    T = Analisa_expressao(T);
    if(strcmp(T.simbolo, "sfaca")== 0){
        T = Analisador_Lexico(T);
        T = Analisa_comando_simples(T);
    } else {
        printf("[%d] Erro sintatico do comando ENQUANTO (ausencia do FACA) --> (lexema recebido: '%s')\n", linha, T.lexema);
    }
    return T;
}

Token Analisa_leia(Token T){
    T = Analisador_Lexico(T);
    if(strcmp(T.simbolo, "sabre_parenteses") == 0){
        T = Analisador_Lexico(T);
        if (strcmp(T.simbolo, "sidentificador") == 0){
            T = Analisador_Lexico(T);
            if (strcmp(T.simbolo, "sfecha_parenteses")==0){
                T = Analisador_Lexico(T);
            } else {
                printf("[%d] Erro: ausencia de ')' no final do comando LEIA --> (lexema recebido: '%s')\n", linha, T.lexema);
            }
        } else{
            printf("[%d] Erro ao fazer a leitura (necessario um identificador) --> (lexema recebido: '%s')\n", linha, T.lexema);
        }
    } else {
        printf("[%d] Erro: ausencia de '(' no inicio do comando LEIA --> (lexema recebido: '%s')\n", linha, T.lexema);
    }
    return T;
}

Token Analisa_escreva(Token T){
    T = Analisador_Lexico(T);
    if(strcmp(T.simbolo, "sabre_parenteses")== 0){
        T = Analisador_Lexico(T);
        if(strcmp(T.simbolo, "sidentificador")== 0){
            T = Analisador_Lexico(T);
            if(strcmp(T.simbolo, "sfecha_parenteses")== 0){
               T = Analisador_Lexico(T);
            } else {
                printf("[%d] Erro: ausencia de ')' no final do comando ESCREVA --> (lexema recebido: '%s')\n", linha, T.lexema);
            }
        } else {
            printf("[%d] Erro ao fazer a escrita (necessario um identificador) --> (lexema recebido: '%s')\n", linha, T.lexema);
        }
    } else {
        printf("[%d] Erro: ausencia de '(' no inicio do comando ESCREVA --> (lexema recebido: '%s')\n", linha, T.lexema);
    }
    return T;
}

Token Analisa_expressao(Token T){
    T = Analisa_expressao_simples(T);
    if((strcmp(T.simbolo, "smaior")== 0) ||
       (strcmp(T.simbolo, "smaiorig")== 0) ||
       (strcmp(T.simbolo, "smenor")== 0) ||
       (strcmp(T.simbolo, "smenorig")== 0) ||
       (strcmp(T.simbolo, "sig")== 0) ||
       (strcmp(T.simbolo, "sdif")== 0)){

        T = Analisador_Lexico(T);
        T = Analisa_expressao_simples(T);
    }

    return T;
}

Token Analisa_expressao_simples(Token T){
    if((strcmp(T.simbolo, "smais")== 0) || (strcmp(T.simbolo, "smenos")== 0)){
        T = Analisador_Lexico(T);
    }
    T = Analisa_termo(T);
    while((strcmp(T.simbolo, "smais")== 0) || (strcmp(T.simbolo, "smenos")== 0) || (strcmp(T.simbolo, "sou")== 0)){
        T = Analisador_Lexico(T);
        T = Analisa_termo(T);
    }
    return T;
}

Token Analisa_termo(Token T){
    T = Analisa_fator(T);
    while((strcmp(T.simbolo, "smult")== 0) || (strcmp(T.simbolo, "sdiv")== 0) || (strcmp(T.simbolo, "se")== 0)){
        T = Analisador_Lexico(T);
        T = Analisa_fator(T);
    }
    return T;
}

Token Analisa_fator(Token T){
    if(strcmp(T.simbolo, "sidentificador")== 0){
        T = Analisa_chamada_funcao(T);
    } else if(strcmp(T.simbolo, "snumero")== 0){
        T = Analisador_Lexico(T);
    } else if(strcmp(T.simbolo, "snao")== 0){
        T = Analisador_Lexico(T);
        T = Analisa_fator(T);
    } else if(strcmp(T.simbolo, "sabre_parenteses")== 0){
        T = Analisador_Lexico(T);
        T = Analisa_expressao(T);
        if(strcmp(T.simbolo, "sfecha_parenteses")== 0){
            T = Analisador_Lexico(T);
        } else {
            printf("[%d] Erro: ausencia de ) na analise do fator apos sua abertura --> (lexema recebido: '%s')\n", linha, T.lexema);
        }
    } else if((strcmp(T.lexema, "verdadeiro")== 0) || (strcmp(T.lexema, "falso")== 0)){
        T = Analisador_Lexico(T);
    } else {
        printf("[%d] Erro ao fazer a analise do fator --> (lexema recebido: '%s')\n", linha, T.lexema);
    }
    return T;
}

Token Analisa_chamada_funcao(Token T){
    if(strcmp(T.simbolo, "sidentificador")== 0){
       T = Analisador_Lexico(T);
    } else {
        printf("[%d] Erro ao identificar chamada de funcao/variavel --> (lexema recebido: '%s')\n", linha, T.lexema);
    }
    return T;
}

