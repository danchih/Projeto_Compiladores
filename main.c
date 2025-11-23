#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pilha.h"

/* 
Codigo Fonte do Compilador. Contem as etapas:
- Analisador Lexico
- Analisador Sintatico
- Analisador Semantico
- Geracao de Codigo
*/

FILE *arquivo;
FILE *arquivo_obj;
char *nome_arquivo;

int linha = 1;

Pilha* pilha = NULL;
Pilha* pilha_op = NULL;
Pilha* pilha_counter = NULL;
char posfixa[250][100];
int conta_string = 0;

char rotulo_str[5];
char c = 'X';
int end = 1;
char end_str[20];
char counter_str[20];
int rotulo = 1;
int counter = 0;

typedef struct{
    char lexema[50];
    char simbolo[50];
    char caracter;
}Token;

// Prototipos das funcoes ==================================================================================================
char *mudar_extensao_para_obj(const char *caminho_original);

// Analisadores Lexicos ====================================================================================================
Token trata_digito(Token T);                    // Faz a analise/geracao dos Tokens numericos
Token trata_pontuacao(Token T);                 // Faz a analise/geracao dos Tokens de pontuacao
Token trata_operador_aritmetico(Token T);       // Faz a analise/geracao dos Tokens de operador aritmetico
Token trata_operador_relacional(Token T);       // Faz a analise/geracao dos Tokens de operador relacional
Token trata_atribuicao(Token T);                // Faz a analise/geracao dos Tokens de atribuicao
Token trata_identificador_palavra(Token T);     // Faz a analise/geracao dos Tokens de identificadores
Token Pega_Token(Token T);                      // Funcao que verifica qual tipo de Token foi encontrado
Token Analisador_Lexico(Token T);               // Funcao que desconsidera tudo que é espaco, tab, enter, comentario {}


// Analisadores Sintaticos, Semantico e Geracao de Codigo  ==================================================================
/* Verifica se a estrutura do codigo esta correta, se os identificadores foram declarados corretamente e faz a geracao de codigo */
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
Token Analisa_comando_atribuicao(Token T, Token T_var);
Token Analisa_chamada_procedimento(Token T1, Token T);
Token Analisa_se(Token T);
Token Analisa_enquanto(Token T);
Token Analisa_leia(Token T);
Token Analisa_escreva(Token T);
Token Analisa_expressao(Token T);
Token Analisa_expressao_simples(Token T);
Token Analisa_termo(Token T);
Token Analisa_fator(Token T);
Token Analisa_chamada_funcao(Token T);

// Funcoes de Conversao para Posfixa
void Converte_posfixa(Token T);
int Precedencia_topo(Pilha *p);
int Precedencia_atual(Token T);
char *Verifica_Tipo_Posfixa();
char *Pesquisa_Tipo(char* simbolo);

// Funcao Gera Codigo
void Gera_Codigo(char* rot, char* comando, char* tipo1, char* tipo2);

// Funcoes de Pesquisa na Tabela de Simbolos ==============================================================================
int Pesquisa_duplicvar_tabela(char* nome_variavel);         // Funcao para pesquisar se a variavel ja foi declarada ate a primeira marca
void Pesquisa_tipo_tabela(char* simbolo);                   // Funcao para pequisar o tipo do identificador na tabela
int Pesquisa_declvar_tabela(char* nome_variavel);           // Funcao para pesquisar em toda a tabela se a variavel ja foi declarada
int Pesquisa_primeira_marca_tabela();                       // Pequisa em qual funcao se encontra para pegar o tipo
int Pesquisa_declfuncao_existe_tabela(char* nome_variavel); // Pesquisa se a funcao ja foi declarada (faz a busca na tabela inteira)
int Pesquisa_declproc_tabela(char* nome_procedimento);      // Verifica se o procedimento ja foi declarado ate a primeira marca
int Pesquisa_declfunc_tabela(char* nome_funcao);            // Verifica se a funcao ja foi declarada ate a primeira marca
int Pesquisa_champroc_tabela(char* nome_procedimento);      // Verifica se a procedimento ja foi declarada (faz a busca na tabela inteira)
int Pesquisa_chamfuncao_tabela(char* nome_funcao);          // Verifica se a funcao ja foi declarada (faz a busca na tabela inteira)
void Desempilha_volta_nivel(Pilha* p);

// Dicionario da pilha ======================================================================================================s
// Ordem da Tabela:  lexema - escopo - tipo - memoria 

// escopo = 1: procedimento / funcao / programa
// escopo = 0: nada

// tipo = 0: nao declarado
// tipo = 1: nome do programa
// tipo = 2: var inteiro
// tipo = 3: var booleano
// tipo = 4: procedimento
// tipo = 5: funcao inteiro
// tipo = 6: funcao booleano

// Funcao Principal =========================================================================================================

int main(int argc, char *argv[])
{
    setvbuf(stdout, NULL, _IONBF, 0); // <<< Desativa buffer de stdout
    setvbuf(stderr, NULL, _IONBF, 0); // desativa buffer de erro também

    if (argc < 2) {
        printf("Uso: %s <arquivo_fonte>\n", argv[0]);
        return 1;
    }

    arquivo = fopen(argv[1], "r");
    if (arquivo == NULL) {
        printf("Erro: não foi possível abrir o arquivo '%s'\n", argv[1]);
        return 1;
    }

    nome_arquivo = mudar_extensao_para_obj(argv[1]);

    // arquivo = fopen("teste.txt", "r");
    // nome_arquivo = "teste.obj";
    arquivo_obj = fopen(nome_arquivo, "w");
    if (arquivo_obj == NULL) {
        perror("Erro ao criar arquivo OBJ");
    }
    fclose(arquivo_obj);

    Token T;
    pilha = CriaPilha();
    pilha_op = CriaPilha();
    pilha_counter = CriaPilha();

    T.caracter = fgetc(arquivo);

    Analisador_sintatico(T);

    fclose(arquivo);

    return 0;
}

char *mudar_extensao_para_obj(const char *caminho_original) {
    char *ponto;
    char *caminho_temp = strdup(caminho_original); 

    if (caminho_temp == NULL) {
        perror("Erro ao alocar memória para caminho_temp");
        return NULL;
    }

    // Encontrar a última ocorrência de '.'
    ponto = strrchr(caminho_temp, '.');

    if (ponto) {
        *ponto = '\0';
    }

    size_t novo_tamanho = strlen(caminho_temp) + strlen(".obj") + 1;

    char *novo_caminho = (char*)malloc(novo_tamanho);
    if (novo_caminho == NULL) {
        perror("Erro ao alocar memória para novo_caminho");
        free(caminho_temp); 
        return NULL;
    }

    sprintf(novo_caminho, "%s%s", caminho_temp, ".obj");

    free(caminho_temp);

    return novo_caminho;
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
            printf("[%d] Erro: Ausencia do = para o lexema != \n", linha);
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
        printf("[%d] Erro: caracter nao encontrado\n", linha);
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
            push(pilha, T.lexema, 1, 1, -1);
            T = Analisador_Lexico(T);
            if(strcmp(T.simbolo, "sponto_virgula")== 0){
                Gera_Codigo(" ", "START", " ", " ");
                Gera_Codigo(" ", "ALLOC", "0", "1");
                T = Analisa_bloco(T);
                if(strcmp(T.simbolo, "sponto")== 0){
                    T = Analisador_Lexico(T);
                    if(T.caracter == EOF){
                        Gera_Codigo(" ", "DALLOC", "0", "1");
                        Gera_Codigo(" ", "HLT", " ", " ");
                        printf("Termino de programa\n");
                    } else {
                        printf("[%d] Erro ao terminar programa\n", linha);
                    }
                } else {
                    printf("[%d] Erro: falta de . apos o termino do bloco \n", linha);
                }
            } else {
                printf("[%d] Erro: falta de ; ao iniciar o programa \n", linha);
            }
        } else {
            printf("[%d] Erro ao identificar programa \n", linha);
        }
    } else {
        printf("[%d] Erro ao iniciar programa \n", linha);
    }
}

Token Analisa_bloco(Token T){
    T = Analisador_Lexico(T);

    while ((strcmp(T.simbolo, "svar")!= 0) &&
           (strcmp(T.simbolo, "sprocedimento")!= 0) &&
           (strcmp(T.simbolo, "sfuncao")!= 0) &&
           (strcmp(T.simbolo, "sinicio") != 0) &&
           (T.caracter != EOF)) {

        printf("[%d] Aviso: simbolo inesperado '%s' (ignorado) \n", linha, T.lexema);

        T = Analisador_Lexico(T);
    }
    if (T.caracter == EOF) {
        printf("[%d] Erro: fim de arquivo inesperado (Erro ao iniciar bloco) \n", linha);
        return T;
    }

    if (strcmp(T.simbolo, "svar") == 0){
        T = Analisa_et_variavel(T);
    } else {
        push(pilha_counter, "0", 0, 0, 0);
    }

    if ((strcmp(T.simbolo, "sprocedimento") == 0) || (strcmp(T.simbolo, "sfuncao") == 0)){
        T = Analisa_subrotinas(T);
    }

    T = Analisa_comandos(T);
    char *dalloc_cmp = pop_op(pilha_counter);
    if (dalloc_cmp == NULL) dalloc_cmp = "0";

    int tipo_aux = Pesquisa_primeira_marca_tabela();
    if (tipo_aux == 5 || tipo_aux == 6){
        Gera_Codigo(" ", "STR", "0", " ");
    }
    
    if (strcmp(dalloc_cmp, "0") != 0 && dalloc_cmp[0] != '\0') {
        end = end - atoi(dalloc_cmp);
        sprintf(end_str, "%d", end);
        Gera_Codigo(" ", "DALLOC", end_str, dalloc_cmp);
    
    } 
    
    return T;
}


Token Analisa_et_variavel(Token T){
    int end_var;

    end_var = end;
    if(strcmp(T.simbolo, "svar")== 0){
        T = Analisador_Lexico(T);
        if(strcmp(T.simbolo, "sidentificador")== 0){
            while(strcmp(T.simbolo, "sidentificador")== 0){
                T = Analisa_variaveis(T);
                if(strcmp(T.simbolo, "sponto_virgula")== 0){
                    T = Analisador_Lexico(T);
                } else {
                    printf("[%d] Erro: falta ; apos a declaracao das variavel \n", linha);
                }
            }
        } else {
            printf("[%d] Erro ao declarar variavel \n", linha);
        }

        // alterado
        sprintf(end_str, "%d", end_var);
        sprintf(counter_str, "%d", counter);
        Gera_Codigo(" ", "ALLOC", end_str, counter_str);
        push(pilha_counter, counter_str, 0, 0, 0);
        counter = 0;
    }
    return T;
}

Token Analisa_variaveis(Token T){
    do{
        if(strcmp(T.simbolo, "sidentificador") == 0){
            if (Pesquisa_duplicvar_tabela(T.lexema) == 0){
                push(pilha, T.lexema, 0, 0, end);
                end ++;
                counter++;
                T = Analisador_Lexico(T);
                if((strcmp(T.simbolo, "svirgula")== 0) || (strcmp(T.simbolo, "sdoispontos")== 0)){
                    if(strcmp(T.simbolo, "svirgula") == 0){
                        T = Analisador_Lexico(T);
                        if(strcmp(T.simbolo, "sdoispontos") == 0)
                            printf("[%d] Erro: falta de identificador apos virgula\n", linha);
                    }
                } else{
                    printf("[%d] Erro: falta de pontuacao na declaracao de variavel\n", linha);
                }
            }else{
                printf("[%d] Erro: variavel '%s' ja declarada anteriormente \n", linha, T.lexema);
                T = Analisador_Lexico(T);
            }
        } else {
            printf("[%d] Erro: declaracao de variavel incorreta \n", linha);
        }
    }while(strcmp(T.simbolo, "sdoispontos") != 0 && strcmp(T.simbolo, "sponto_virgula") != 0 && T.caracter != EOF);

    if(strcmp(T.simbolo, "sdoispontos") != 0){
        printf("[%d] Erro: falta ':' na declaracao de variavel\n", linha);
    }

    T = Analisador_Lexico(T);
    T = Analisa_Tipo(T);

    return T;
}

Token Analisa_Tipo(Token T){
    if((strcmp(T.simbolo, "sinteiro") != 0) && (strcmp(T.simbolo, "sbooleano") != 0)){
        printf("[%d] Erro: tipo de variavel invalido \n", linha);
    }else{
        Pesquisa_tipo_tabela(T.simbolo);
    }
    T = Analisador_Lexico(T);
    return T;
}

Token Analisa_subrotinas(Token T){
    int auxrot;

    auxrot = rotulo;
    sprintf(rotulo_str, "%c%d", c, auxrot);
    Gera_Codigo(" ", "JMP", rotulo_str, " ");
    rotulo++;

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
            printf("[%d] Erro: falta ; apos a declaracao da subrotina \n", linha);
       }
    }

    sprintf(rotulo_str, "%c%d", c, auxrot);
    Gera_Codigo(rotulo_str, "NULL", " ", " ");

    return T;
}

Token Analisa_declaracao_procedimento(Token T){
    T = Analisador_Lexico(T);
    if(strcmp(T.simbolo, "sidentificador")== 0){
        if (Pesquisa_declproc_tabela(T.lexema) == 0){
            push(pilha, T.lexema, 1, 4, rotulo);

            sprintf(rotulo_str, "%c%d", c, rotulo);
            Gera_Codigo(rotulo_str, "NULL", " ", " ");
            rotulo++;
            T = Analisador_Lexico(T);
            if(strcmp(T.simbolo, "sponto_virgula")== 0){
                T = Analisa_bloco(T);
            } else {
                printf("[%d] Erro: falta ; na declaracao do procedimento \n", linha);
            }
        } else {
            printf("[%d] Erro: procedimento '%s' ja declarado anteriormente\n", linha, T.lexema);
        }
    } else {
        printf("[%d] Erro ao identificar a declaracao do procedimento \n", linha);
    }

    Gera_Codigo(" ", "RETURN", " ", " ");
    Desempilha_volta_nivel(pilha);

    return T;
}

Token Analisa_declaracao_funcao(Token T){
    T = Analisador_Lexico(T);
    if(strcmp(T.simbolo, "sidentificador")== 0){
        if(Pesquisa_declfunc_tabela(T.lexema) == 0){
            push(pilha, T.lexema, 1, 0, rotulo);
            sprintf(rotulo_str, "%c%d", c, rotulo);
            Gera_Codigo(rotulo_str, "NULL", " ", " ");
            rotulo++;
            T = Analisador_Lexico(T);
            if(strcmp(T.simbolo, "sdoispontos")== 0){
                T = Analisador_Lexico(T);
                if((strcmp(T.simbolo, "sinteiro")== 0) || (strcmp(T.simbolo, "sbooleano")== 0)){
                    No*q = pilha->Topo;
                    if(strcmp(T.simbolo, "sinteiro")== 0){
                        q->tipo = 5;
                    }else{
                        q->tipo = 6;
                    }
                    T = Analisador_Lexico(T);
                    if(strcmp(T.simbolo, "sponto_virgula")== 0){
                        T = Analisa_bloco(T);
                    }
                    else{
                        printf("[%d] Erro: falta ; na declaracao da funcao \n", linha);
                    }
                } else {
                    printf("[%d] Erro: Tipo nao correspondente (inteiro/booleano) na declaracao da funcao \n", linha);
                }
            } else {
                printf("[%d] Erro: ausencia de : na declaracao da funcao \n", linha);
            }
        } else {
            printf("[%d] Erro: funcao '%s' ja declarada anteriormente\n", linha, T.lexema);
        }

    } else {
        printf("[%d] Erro ao identificar a declaracao da funcao \n", linha);
    }
    Gera_Codigo(" ", "RETURN", " ", " ");
    Desempilha_volta_nivel(pilha);
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
                printf("[%d] Erro: esperado ';' ao final da analise do comando \n", linha);
                T = Analisador_Lexico(T);
            }
        }

        if (strcmp(T.simbolo, "sfim") == 0) {
            T = Analisador_Lexico(T);
        } else {
            printf("[%d] Erro: esperado 'fim' ao finalizar comando \n", linha);
        }
    } else {
        printf("[%d] Erro: esperado 'inicio' ao inicializar comando \n", linha);
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
        printf("[%d] Erro: comando invalido \n", linha);
    }
    return T;
}

Token Analisa_atrib_chprocedimento(Token T){
    Token T1 = Analisador_Lexico(T);
    if(strcmp(T1.simbolo, "satribuicao")== 0){
        if(Pesquisa_declfuncao_existe_tabela(T.lexema) == -1 && Pesquisa_declvar_tabela(T.lexema) == -1){
            printf("[%d] Erro: variavel '%s' nao declarada anteriormente\n", linha, T.lexema);
        }
        T = Analisa_comando_atribuicao(T1, T);

    } else {
        if(Pesquisa_champroc_tabela(T.lexema) == -1){
            printf("[%d] Erro: procedimento '%s' nao declarado anteriormente\n", linha, T.lexema);
        }
        T = Analisa_chamada_procedimento(T1, T);
    }
    return T;
}

Token Analisa_comando_atribuicao(Token T, Token T_var){
    char *op;
    char *tipo_posfixa;

    T = Analisador_Lexico(T);
    T = Analisa_expressao(T);

    while(!vaziaPilha(pilha_op)){
        op = pop_op(pilha_op);
        strcpy(posfixa[conta_string], op);
        conta_string++;
    }
    tipo_posfixa = Verifica_Tipo_Posfixa();

    if(strcmp(Pesquisa_Tipo(T_var.lexema), tipo_posfixa) != 0){
        printf("[%d] Erro: tipo da expressao diferente do tipo da variavel na atribuicao \n", linha);
    }
    // alterado
    int end_retorno = Pesquisa_declvar_tabela(T_var.lexema);
    if(end_retorno != -1){
        sprintf(end_str, "%d", end_retorno);
        Gera_Codigo(" ", "STR", end_str, " ");
    }

    return T;
}

Token Analisa_chamada_procedimento(Token T1, Token T){
    if(strcmp(T.simbolo, "sidentificador")== 0){
        int end_retorno = Pesquisa_champroc_tabela(T.lexema);
        if (end_retorno != -1){
            sprintf(end_str, "%c%d", c, end_retorno);
            Gera_Codigo(" ", "CALL", end_str, " ");
        } else {
            printf("[%d] Erro: procedimento '%s' nao declarado anteriormente \n", linha, T.lexema);
        }
    } else {
        printf("[%d] Erro ao identificar chamada de procedimento \n", linha);
    }
    return T1;
}

Token Analisa_se(Token T){
    char *op;
    char *tipo_posfixa;
    int auxrot1, auxrot2;

    T = Analisador_Lexico(T);
    T = Analisa_expressao(T);

    while(!vaziaPilha(pilha_op)){
        op = pop_op(pilha_op);
        strcpy(posfixa[conta_string], op);
        conta_string++;
    }
    tipo_posfixa = Verifica_Tipo_Posfixa();

    if(strcmp(tipo_posfixa, "/") != 0){
        printf("[%d] Erro: tipo da expressao diferente do tipo da variavel na funcao SE \n", linha);
    }

    if(strcmp(T.simbolo, "sentao")== 0){
        auxrot1 = rotulo;
        sprintf(rotulo_str, "%c%d", c, auxrot1);
        Gera_Codigo(" ", "JMPF", rotulo_str, " ");
        rotulo++;

        T = Analisador_Lexico(T);
        T = Analisa_comando_simples(T);

        if(strcmp(T.simbolo, "ssenao")== 0){
            auxrot2 = rotulo;
            sprintf(rotulo_str, "%c%d", c, auxrot2);
            Gera_Codigo(" ", "JMP", rotulo_str, " ");
            rotulo++;
            sprintf(rotulo_str, "%c%d", c, auxrot1);
            Gera_Codigo(rotulo_str, "NULL", " ", " ");

            T = Analisador_Lexico(T);
            T = Analisa_comando_simples(T);

            sprintf(rotulo_str, "%c%d", c, auxrot2);
            Gera_Codigo(rotulo_str, "NULL", " ", " ");

        } else {    
            sprintf(rotulo_str, "%c%d", c, auxrot1);
            Gera_Codigo(rotulo_str, "NULL", " ", " ");
        }
    } else {
        printf("[%d] Erro sintatico do comando SE (ausencia do ENTAO) \n", linha);
    }
    return T;
}

Token Analisa_enquanto(Token T){
    char *op;
    char *tipo_posfixa;
    int auxrot1, auxrot2;

    auxrot1 = rotulo;
    sprintf(rotulo_str, "%c%d", c, auxrot1);
    Gera_Codigo(rotulo_str, "NULL", " ", " ");
    rotulo++;

    T = Analisador_Lexico(T);
    T = Analisa_expressao(T);

    while(!vaziaPilha(pilha_op)){
        op = pop_op(pilha_op);
        strcpy(posfixa[conta_string], op);
        conta_string++;
    }

    tipo_posfixa = Verifica_Tipo_Posfixa();

    if(strcmp(tipo_posfixa, "/") != 0){
        printf("[%d] Erro: tipo da expressao diferente do tipo da variavel na funcao ENQUANTO \n", linha);
    }

    if(strcmp(T.simbolo, "sfaca")== 0){
        auxrot2 = rotulo;
        sprintf(rotulo_str, "%c%d", c, auxrot2);
        Gera_Codigo(" ", "JMPF", rotulo_str, " ");
        rotulo++;

        T = Analisador_Lexico(T);
        T = Analisa_comando_simples(T);

        sprintf(rotulo_str, "%c%d", c, auxrot1);
        Gera_Codigo(" ", "JMP", rotulo_str, " ");
        sprintf(rotulo_str, "%c%d", c, auxrot2);
        Gera_Codigo(rotulo_str, "NULL", " ", " ");

    } else {
        printf("[%d] Erro sintatico do comando ENQUANTO (ausencia do FACA) \n", linha);
    }
    return T;
}

Token Analisa_leia(Token T){
    T = Analisador_Lexico(T);
    if(strcmp(T.simbolo, "sabre_parenteses") == 0){
        T = Analisador_Lexico(T);
        if (strcmp(T.simbolo, "sidentificador") == 0){
            int end_retorno = Pesquisa_declvar_tabela(T.lexema);
            if(end_retorno != -1){
                Gera_Codigo(" ", "RD", " ", " ");
                sprintf(end_str, "%d", end_retorno);
                Gera_Codigo(" ", "STR", end_str, " ");

                char *tipo = Pesquisa_Tipo(T.lexema);
                if(strcmp(tipo, "#") != 0){
                    printf("[%d] Erro: tipo da variavel '%s' diferente do tipo esperado na funcao LEIA \n", linha, T.lexema);
                }
                T = Analisador_Lexico(T);
                if (strcmp(T.simbolo, "sfecha_parenteses")==0){
                    T = Analisador_Lexico(T);
                } else {
                    printf("[%d] Erro: ausencia de ')' no final do comando LEIA \n", linha);
                }  
            }else{
                printf("[%d] Erro: variavel '%s' nao declarada anteriormente\n", linha, T.lexema);
            }
        } else{
            printf("[%d] Erro ao fazer a leitura (necessario um identificador) \n", linha);
        }
    } else {
        printf("[%d] Erro: ausencia de '(' no inicio do comando LEIA \n", linha);
    }
    return T;
}

Token Analisa_escreva(Token T){
    T = Analisador_Lexico(T);
    if(strcmp(T.simbolo, "sabre_parenteses")== 0){
        T = Analisador_Lexico(T);
        if(strcmp(T.simbolo, "sidentificador")== 0){
            int end_retorno = Pesquisa_declfuncao_existe_tabela(T.lexema);
            if(Pesquisa_declfuncao_existe_tabela(T.lexema) != -1){
                sprintf(end_str, "%d", end_retorno);
                Gera_Codigo(" ", "CALL", end_str, " ");
                Gera_Codigo(" ", "LDV", "0", " ");
                Gera_Codigo(" ", "PRN", " ", " ");
            } 
            else if(Pesquisa_declvar_tabela(T.lexema) != -1){
                end_retorno = Pesquisa_declvar_tabela(T.lexema);
                sprintf(end_str, "%d", end_retorno);
                Gera_Codigo(" ", "LDV", end_str, " ");
                Gera_Codigo(" ", "PRN", " ", " ");
                
                char *tipo = Pesquisa_Tipo(T.lexema);
                if(strcmp(tipo, "#") != 0){
                    printf("[%d] Erro: tipo da variavel '%s' diferente do tipo esperado no comando escreva \n", linha, T.lexema);
                }
                T = Analisador_Lexico(T);
                if(strcmp(T.simbolo, "sfecha_parenteses")== 0){
                    T = Analisador_Lexico(T);
                } else {
                    printf("[%d] Erro: ausencia de ')' no final do comando ESCREVA \n", linha);
                }
            }else{
                printf("[%d] Erro: variavel/funcao '%s' nao declarada anteriormente\n", linha, T.lexema);
            }
        } else {
            printf("[%d] Erro ao fazer a escrita (necessario um identificador) \n", linha);
        }
    } else {
        printf("[%d] Erro: ausencia de '(' no inicio do comando ESCREVA \n", linha);
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

        Converte_posfixa(T);
        T = Analisador_Lexico(T);
        T = Analisa_expressao_simples(T);
    }
    return T;
}

Token Analisa_expressao_simples(Token T){
    if((strcmp(T.simbolo, "smais")== 0)){
        strcpy(T.simbolo, "spositivo");
        Converte_posfixa(T);
        T = Analisador_Lexico(T);
    }
    if((strcmp(T.simbolo, "smenos")== 0)){
        strcpy(T.simbolo, "snegativo");
        Converte_posfixa(T);
        T = Analisador_Lexico(T);
    }
    T = Analisa_termo(T);
    while((strcmp(T.simbolo, "smais")== 0) || (strcmp(T.simbolo, "smenos")== 0) || (strcmp(T.simbolo, "sou")== 0)){
        Converte_posfixa(T);
        T = Analisador_Lexico(T);
        T = Analisa_termo(T);
    }
    return T;
}

Token Analisa_termo(Token T){
    T = Analisa_fator(T);
    while((strcmp(T.simbolo, "smult")== 0) || (strcmp(T.simbolo, "sdiv")== 0) || (strcmp(T.simbolo, "se")== 0)){
        Converte_posfixa(T);
        T = Analisador_Lexico(T);
        T = Analisa_fator(T);
    }
    return T;
}

Token Analisa_fator(Token T){
    if(strcmp(T.simbolo, "sidentificador")== 0){
        if(Pesquisa_declvar_tabela(T.lexema) != -1 || Pesquisa_declfuncao_existe_tabela(T.lexema) != -1){
            if(Pesquisa_declfuncao_existe_tabela(T.lexema) != -1){
                T = Analisa_chamada_funcao(T);
            }else{
                Converte_posfixa(T);
                T = Analisador_Lexico(T);
            }
        }else{
            printf("[%d] Erro: variavel/funcao '%s' nao declarada anteriormente\n", linha, T.lexema);
        }
    } else if(strcmp(T.simbolo, "snumero")== 0){
        Converte_posfixa(T);
        T = Analisador_Lexico(T);

    } else if(strcmp(T.simbolo, "snao")== 0){
        Converte_posfixa(T);
        T = Analisador_Lexico(T);
        T = Analisa_fator(T);

    } else if(strcmp(T.simbolo, "sabre_parenteses")== 0){
        Converte_posfixa(T);
        T = Analisador_Lexico(T);
        T = Analisa_expressao(T);

        if(strcmp(T.simbolo, "sfecha_parenteses")== 0){
            Converte_posfixa(T);
            T = Analisador_Lexico(T);

        } else {
            printf("[%d] Erro: ausencia de ) na analise do fator apos sua abertura \n", linha);
        }
    } else if((strcmp(T.lexema, "verdadeiro")== 0) || (strcmp(T.lexema, "falso")== 0)){
        Converte_posfixa(T);
        T = Analisador_Lexico(T);

    } else {
        printf("[%d] Erro ao fazer a analise do fator \n", linha);
    }
    return T;
}

Token Analisa_chamada_funcao(Token T){
    if(strcmp(T.simbolo, "sidentificador")== 0){
        int end_retorno = Pesquisa_chamfuncao_tabela(T.lexema);
        if (end_retorno != -1){
            Converte_posfixa(T);
            T = Analisador_Lexico(T);
        } else {
            printf("[%d] Erro: funcao '%s' nao declarada anteriormente\n", linha, T.lexema);
        }
    } else {
        printf("[%d] Erro ao identificar chamada de funcao/variavel \n", linha);
    }
    return T;
}

void Converte_posfixa(Token T){
    char *op;

    if(strcmp(T.simbolo, "smais")== 0 || strcmp(T.simbolo, "smenos")== 0 ||
       strcmp(T.simbolo, "smult")== 0 || strcmp(T.simbolo, "sdiv")== 0 ||
       strcmp(T.simbolo, "sig")== 0 || strcmp(T.simbolo, "sdif")== 0 ||
       strcmp(T.simbolo, "smaior")== 0 || strcmp(T.simbolo, "smaiorig")== 0 ||
       strcmp(T.simbolo, "smenor")== 0 || strcmp(T.simbolo, "smenorig")== 0 ||
       strcmp(T.simbolo, "se")== 0 || strcmp(T.simbolo, "sou")== 0 ||
       strcmp(T.simbolo, "snao")== 0 || strcmp(T.simbolo, "sabre_parenteses")== 0 ||
       strcmp(T.simbolo, "snegativo") == 0 || strcmp(T.simbolo, "spositivo") == 0){

            // Processamento de operadores
            if(strcmp(T.simbolo, "sabre_parenteses")!= 0){
                while(!vaziaPilha(pilha_op) && (Precedencia_topo(pilha_op) >= Precedencia_atual(T)) && (Precedencia_topo(pilha_op) != 8)){
                        op = pop_op(pilha_op);
                        if (op == NULL) break;
                        strcpy(posfixa[conta_string], op);
                        conta_string++;
                }
            }

           if(strcmp(T.simbolo, "spositivo") == 0) {
                push(pilha_op, "@", -1, -1, -1);
           }
           else if(strcmp(T.simbolo, "snegativo") == 0){
                push(pilha_op, "$", -1, -1, -1);
           }
           else{
                push(pilha_op, T.lexema, -1, -1, -1);
           }

    } else if (strcmp(T.simbolo, "sfecha_parenteses")== 0){
        op = pop_op(pilha_op);
        // Processamento de fechamento de parenteses
        while(!vaziaPilha(pilha_op) && (strcmp(op, "(") != 0)){
            strcpy(posfixa[conta_string], op);
            conta_string++;
            op = pop_op(pilha_op);
        }

    } else if(strcmp(T.simbolo, "sidentificador")== 0 ||
              strcmp(T.simbolo, "snumero")== 0 ||
              strcmp(T.simbolo, "sverdadeiro")== 0 ||
              strcmp(T.simbolo, "sfalso")== 0){
        // Processamento de operandos
        strcpy(posfixa[conta_string], T.lexema);
        conta_string++;
    }
}

int Precedencia_topo(Pilha *p){
    // 8 (abre parenteses)
    // Aritméticos: 7 (+ positivo, - negativo) 6 (*,div) 5 (+,-)
    // Relacionais: 4 (todos iguais)
    // Lógicos: 3 (não) 2 (e) 1 (ou)
    No* q = p->Topo;

    if(q->lexema != NULL){
        if(strcmp(q->lexema, "(")== 0){
            return 8;
        }else if(strcmp(q->lexema, "pos")== 0 || strcmp(q->lexema,"neg") == 0){
            return 7;
        } else if(strcmp(q->lexema, "*") == 0 || strcmp(q->lexema, "div") == 0){
            return 6;
        } else if(strcmp(q->lexema, "+") == 0 || strcmp(q->lexema, "-") == 0){
            return 5;
        } else if(strcmp(q->lexema, ">") == 0 || strcmp(q->lexema, "<") == 0 || strcmp(q->lexema, ">=") == 0 ||
                  strcmp(q->lexema, "<=") == 0 || strcmp(q->lexema, "=") == 0 || strcmp(q->lexema, "!=") == 0){
            return 4;
        } else if(strcmp(q->lexema, "nao") == 0){
            return 3;
        } else if(strcmp(q->lexema, "e") == 0){
            return 2;
        } else if(strcmp(q->lexema, "ou") == 0){
            return 1;
        }
    }
}

int Precedencia_atual(Token T){
    // Aritméticos: 7 (+ positivo, - negativo) 6 (*,div) 5 (+,-)
    // Relacionais: 4 (todos iguais)
    // Lógicos: 3 (não) 2 (e) 1 (ou)

    if(T.lexema != NULL){
        if(strcmp(T.lexema, "pos")== 0 || strcmp(T.lexema,"neg") == 0){
            return 7;
        } else if(strcmp(T.lexema, "*") == 0 || strcmp(T.lexema, "div") == 0){
            return 6;
        } else if(strcmp(T.lexema, "+") == 0 || strcmp(T.lexema, "-") == 0){
            return 5;
        } else if(strcmp(T.lexema, ">") == 0 || strcmp(T.lexema, "<") == 0 || strcmp(T.lexema, ">=") == 0 ||
                  strcmp(T.lexema, "<=") == 0 || strcmp(T.lexema, "=") == 0 || strcmp(T.lexema, "!=") == 0){
            return 4;
        } else if(strcmp(T.lexema, "nao") == 0){
            return 3;
        } else if(strcmp(T.lexema, "e") == 0){
            return 2;
        } else if(strcmp(T.lexema, "ou") == 0){
            return 1;
        }
    }
}

char *Verifica_Tipo_Posfixa(){
    // invalido =   (0)
    // inteiro  = # (1)
    // booleano = / (2)
    Pilha* p_tipo = CriaPilha();
    char *op;
    char *op2;
    char *op_tipo;
    char *op2_tipo;

    if (conta_string == 1){
        op = posfixa[0];
        op_tipo = Pesquisa_Tipo(op);

        if(isdigit(op[0])){
            Gera_Codigo(" ", "LDC", op, " ");
        }
        else if(strcmp(op, "verdadeiro") == 0){
            Gera_Codigo(" ", "LDC", "1", " ");
        }
        else if(strcmp(op, "falso") == 0){
            Gera_Codigo(" ", "LDC", "0", " ");
        }
        else if(Pesquisa_declvar_tabela(op)!= -1){
            int end_retorno = Pesquisa_declvar_tabela(op);
            sprintf(end_str, "%d", end_retorno);
            Gera_Codigo(" ", "LDV", end_str, " ");
        }
        else if(Pesquisa_declfuncao_existe_tabela(op)!= -1){
            int end_retorno = Pesquisa_declfuncao_existe_tabela(op);
            sprintf(end_str, "%c%d", c, end_retorno);
            Gera_Codigo(" ", "CALL", end_str, " ");
            Gera_Codigo(" ", "LDV", "0", " ");
        }
        
        conta_string = 0;
        memset(posfixa, 0, sizeof(posfixa));
        return op_tipo;

    } else{
        for(int i=0; i<conta_string; i++){

            if(strcmp(posfixa[i], "+") == 0 || strcmp(posfixa[i], "-") == 0 ||
            strcmp(posfixa[i], "*") == 0 || strcmp(posfixa[i], "div") == 0){

                op = pop_op(p_tipo);
                op2 = pop_op(p_tipo);
                op_tipo = Pesquisa_Tipo(op);
                op2_tipo = Pesquisa_Tipo(op2);

                // verifica op2
                if(isdigit(op2[0])){
                    Gera_Codigo(" ", "LDC", op2, " ");
                }
                else if(Pesquisa_declvar_tabela(op2)!= -1){
                    int end_retorno = Pesquisa_declvar_tabela(op2);
                    sprintf(end_str, "%d", end_retorno);
                    Gera_Codigo(" ", "LDV", end_str, " ");
                }
                else if(Pesquisa_declfuncao_existe_tabela(op2)!= -1){
                    int end_retorno = Pesquisa_declfuncao_existe_tabela(op2);
                    sprintf(end_str, "%c%d", c, end_retorno);
                    Gera_Codigo(" ", "CALL", end_str, " ");
                    Gera_Codigo(" ", "LDV", "0", " ");
                }

                // Verifica op
                if(isdigit(op[0])){
                    Gera_Codigo(" ", "LDC", op, " ");
                }
                else if(Pesquisa_declvar_tabela(op)!= -1){
                    int end_retorno = Pesquisa_declvar_tabela(op);
                    sprintf(end_str, "%d", end_retorno);
                    Gera_Codigo(" ", "LDV", end_str, " ");
                }
                else if(Pesquisa_declfuncao_existe_tabela(op)!= -1){
                    int end_retorno = Pesquisa_declfuncao_existe_tabela(op);
                    sprintf(end_str, "%c%d", c, end_retorno);
                    Gera_Codigo(" ", "CALL", end_str, " ");
                    Gera_Codigo(" ", "LDV", "0", " ");
                }
                
                if(strcmp(op_tipo, "#") == 0 && strcmp(op2_tipo, "#") == 0){
                    push(p_tipo, "#", -1, -1, -1);
                    //verifica tipos
                    if(strcmp(posfixa[i], "+") == 0){
                        Gera_Codigo(" ", "ADD", " ", " ");
                    } else if(strcmp(posfixa[i], "-") == 0){
                        Gera_Codigo(" ", "SUB", " ", " ");
                    } else if(strcmp(posfixa[i], "*") == 0){
                        Gera_Codigo(" ", "MULT", " ", " ");
                    } else if(strcmp(posfixa[i], "div") == 0){
                        Gera_Codigo(" ", "DIV", " ", " ");
                    }
                } else {
                    printf("[%d] Erro: operacao aritmetica com tipos invalidos \n", linha);
                    return "0";
                }

            }else if (strcmp(posfixa[i], ">") == 0 || strcmp(posfixa[i], "<") == 0 ||
                    strcmp(posfixa[i], ">=") == 0 || strcmp(posfixa[i], "<=") == 0 ||
                    strcmp(posfixa[i], "=") == 0 || strcmp(posfixa[i], "!=") == 0){

                op = pop_op(p_tipo);
                op2 = pop_op(p_tipo);
                op_tipo = Pesquisa_Tipo(op);
                op2_tipo = Pesquisa_Tipo(op2);

                // verifica op2
                if(isdigit(op2[0])){
                    Gera_Codigo(" ", "LDC", op2, " ");
                }
                else if(Pesquisa_declvar_tabela(op2)!= -1){
                    int end_retorno = Pesquisa_declvar_tabela(op2);
                    sprintf(end_str, "%d", end_retorno);
                    Gera_Codigo(" ", "LDV", end_str, " ");
                }
                else if(Pesquisa_declfuncao_existe_tabela(op2)!= -1){
                    int end_retorno = Pesquisa_declfuncao_existe_tabela(op2);
                    sprintf(end_str, "%c%d", c, end_retorno);
                    Gera_Codigo(" ", "CALL", end_str, " ");
                    Gera_Codigo(" ", "LDV", "0", " ");
                }
                // Verifica op
                if(isdigit(op[0])){
                    Gera_Codigo(" ", "LDC", op, " ");
                }
                else if(Pesquisa_declvar_tabela(op)!= -1){
                    int end_retorno = Pesquisa_declvar_tabela(op);
                    sprintf(end_str, "%d", end_retorno);
                    Gera_Codigo(" ", "LDV", end_str, " ");
                }
                else if(Pesquisa_declfuncao_existe_tabela(op)!= -1){
                    int end_retorno = Pesquisa_declfuncao_existe_tabela(op);
                    sprintf(end_str, "%c%d", c, end_retorno);
                    Gera_Codigo(" ", "CALL", end_str, " ");
                    Gera_Codigo(" ", "LDV", "0", " ");
                }

                if(strcmp(op_tipo, "#") == 0 && strcmp(op2_tipo, "#") == 0){
                    push(p_tipo, "/", -1, -1, -1);
                    //verifica tipos
                    if(strcmp(posfixa[i], ">") == 0){
                        Gera_Codigo(" ", "CMA", " ", " ");
                    } else if(strcmp(posfixa[i], "<") == 0){
                        Gera_Codigo(" ", "CME", " ", " ");
                    } else if(strcmp(posfixa[i], ">=") == 0){
                        Gera_Codigo(" ", "CMAQ", " ", " ");
                    } else if(strcmp(posfixa[i], "<=") == 0){
                        Gera_Codigo(" ", "CMEQ", " ", " ");
                    } else if(strcmp(posfixa[i], "=") == 0){
                        Gera_Codigo(" ", "CEQ", " ", " ");
                    } else if(strcmp(posfixa[i], "!=") == 0){
                        Gera_Codigo(" ", "CDIF", " ", " ");
                    }

                } else {
                    printf("[%d] Erro: operacao aritmetica com tipos invalidos \n", linha);
                    return "0";
                }

            } else if(strcmp(posfixa[i], "@") == 0 || strcmp(posfixa[i], "$") == 0){

                op = pop_op(p_tipo);
                op_tipo = Pesquisa_Tipo(op);

                // Verifica op
                if(isdigit(op[0])){
                    Gera_Codigo(" ", "LDC", op, " ");
                }
                else if(Pesquisa_declvar_tabela(op)!= -1){
                    int end_retorno = Pesquisa_declvar_tabela(op);
                    sprintf(end_str, "%d", end_retorno);
                    Gera_Codigo(" ", "LDV", end_str, " ");
                }
                else if(Pesquisa_declfuncao_existe_tabela(op)!= -1){
                    int end_retorno = Pesquisa_declfuncao_existe_tabela(op);
                    sprintf(end_str, "%c%d", c, end_retorno);
                    Gera_Codigo(" ", "CALL", end_str, " ");
                    Gera_Codigo(" ", "LDV", "0", " ");
                }

                //verifica tipos
                if(strcmp(posfixa[i], "$") == 0){
                    Gera_Codigo(" ", "INV", " ", " ");
                }

                if(strcmp(op_tipo, "#") == 0){
                    push(p_tipo, "#", -1, -1, -1);

                } else {
                    printf("[%d] Erro: operacao aritmetica com tipos invalidos \n", linha);
                    return "0";
                }

            } else if(strcmp(posfixa[i], "e") == 0 || strcmp(posfixa[i], "ou") == 0){

                op = pop_op(p_tipo);
                op2 = pop_op(p_tipo);
                op_tipo = Pesquisa_Tipo(op);
                op2_tipo = Pesquisa_Tipo(op2);
            
                // verifica op2
                if(Pesquisa_declvar_tabela(op2)!= -1){
                    int end_retorno = Pesquisa_declvar_tabela(op2);
                    sprintf(end_str, "%d", end_retorno);
                    Gera_Codigo(" ", "LDV", end_str, " ");

                }
                else if(Pesquisa_declfuncao_existe_tabela(op2)!= -1){
                    int end_retorno = Pesquisa_declfuncao_existe_tabela(op2);
                    sprintf(end_str, "%c%d", c, end_retorno);
                    Gera_Codigo(" ", "CALL", end_str, " ");
                    Gera_Codigo(" ", "LDV", "0", " ");
                }

                // Verifica op
                if(Pesquisa_declvar_tabela(op)!= -1){
                    int end_retorno = Pesquisa_declvar_tabela(op);
                    sprintf(end_str, "%d", end_retorno);
                    Gera_Codigo(" ", "LDV", end_str, " ");
                    
                }    
                else if(Pesquisa_declfuncao_existe_tabela(op)!= -1){
                    int end_retorno = Pesquisa_declfuncao_existe_tabela(op);
                    sprintf(end_str, "%c%d", c, end_retorno);
                    Gera_Codigo(" ", "CALL", end_str, " ");
                    Gera_Codigo(" ", "LDV", "0", " ");
                }
                

                if(strcmp(op_tipo, "/") == 0 && strcmp(op2_tipo, "/") == 0){
                    push(p_tipo, "/", -1, -1, -1);
                    //verifica tipos
                    if(strcmp(posfixa[i], "e") == 0){
                        Gera_Codigo(" ", "AND", " ", " ");
                    } else if(strcmp(posfixa[i], "ou") == 0){
                        Gera_Codigo(" ", "OR", " ", " ");
                    }
                } else {
                    printf("[%d] Erro: operacao logica com tipos invalidos \n", linha);
                    return "0";
                }

            } else if(strcmp(posfixa[i], "nao") == 0){

                op = pop_op(p_tipo);
                op_tipo = Pesquisa_Tipo(op);

                // Verifica op
                if(Pesquisa_declvar_tabela(op)!= -1){
                    int end_retorno = Pesquisa_declvar_tabela(op);
                    sprintf(end_str, "%d", end_retorno);
                    Gera_Codigo(" ", "LDV", end_str, " ");
                }
                else if(Pesquisa_declfuncao_existe_tabela(op)!= -1){
                    int end_retorno = Pesquisa_declfuncao_existe_tabela(op);
                    sprintf(end_str, "%c%d", c, end_retorno);
                    Gera_Codigo(" ", "CALL", end_str, " ");
                    Gera_Codigo(" ", "LDV", "0", " ");
                }

                if(strcmp(op_tipo, "/") == 0){
                    push(p_tipo, "/", -1, -1, -1);
                    //verifica tipos
                    Gera_Codigo(" ", "NEG", " ", " ");
                } else {
                    printf("[%d] Erro: operacao logica com tipos invalidos \n", linha);
                    return "0";
                }

            } else {
                push(p_tipo, posfixa[i], -1, -1, -1);
            }
        }
    }

    op = pop_op(p_tipo);
    conta_string = 0;
    memset(posfixa, 0, sizeof(posfixa));
    return op;
}

char *Pesquisa_Tipo(char* simbolo){
    No* q = pilha->Topo;

    if(isdigit(simbolo[0])){
        return "#"; // inteiro
    }
    else if (strcmp(simbolo, "verdadeiro") == 0 || strcmp(simbolo, "falso") == 0){
        return "/"; // booleano
    } 
    else if(strcmp(simbolo, "#") == 0){
        return "#"; // inteiro
    } 
    else if(strcmp(simbolo, "/") == 0){
        return "/"; // booleano
    }
    else{
        while(q != NULL){
            if(strcmp(q->lexema, simbolo) == 0){
                if(q->tipo == 2 || q->tipo == 5){
                    return "#"; // inteiro
                } else if (q->tipo == 3 || q->tipo == 6){
                    return "/"; // booleano
                }
            }
            q = q->prox;
        }
        return "0"; // invalido
    }
}

// Analisador Semantico =======================================================================================================

int Pesquisa_duplicvar_tabela(char* nome_variavel){
    // Verifica se a variavel ja foi declarada ate a primeira marca
    if(consulta_variavel_escopo(pilha, nome_variavel) != NULL){
        return 1; // Retorna 1 se a variavel foi declarada
    }
    return 0; // Retorna 0 se a variavel nao foi declarada
}

void Pesquisa_tipo_tabela(char* simbolo){
    // Pesquisa na tabela o tipo do identificador
    No* q = pilha->Topo;
    while(q != NULL && q->escopo == 0){
        if(q->tipo == 0){
            if(strcmp(simbolo, "sinteiro") == 0){
                q->tipo = 2;    // var inteiro
            } else if(strcmp(simbolo, "sbooleano") == 0){
                q->tipo = 3;    // var booleano
            }
        }
        q = q->prox;
    }
}

int Pesquisa_declvar_tabela(char* nome_variavel){
    // Verifica se a variavel ja foi declarada (faz a busca na tabela inteira)
    if(consulta_primeira_ocorrencia_variavel(pilha, nome_variavel) != NULL){
        return consulta_primeira_ocorrencia_variavel(pilha, nome_variavel)->memoria;    // retorna a memoria
    }
    return -1;  // retorna -1 se nao foi declarada
}

int Pesquisa_declfuncao_existe_tabela(char* nome_variavel){
    // Verifica se a funcao ja foi declarada (faz a busca na tabela inteira)
    if(consulta_primeira_ocorrencia_funcao_existe(pilha, nome_variavel) != NULL){
        return consulta_primeira_ocorrencia_funcao_existe(pilha, nome_variavel)->memoria;
    }
    return -1;
}

int Pesquisa_declproc_tabela(char* nome_procedimento){
    // Verifica se o procedimento ja foi declarado ate a primeira marca
    if(consulta_procedimento_escopo(pilha, nome_procedimento) != NULL){
        return 1;
    }
    return 0;
}

int Pesquisa_declfunc_tabela(char* nome_funcao){
    // Verifica se a funcao ja foi declarada ate a primeira marca
    if(consulta_funcao_escopo(pilha, nome_funcao) != NULL){
        return 1;
    }
    return 0;
}

int Pesquisa_champroc_tabela(char* nome_procedimento){
    // Verifica se a procedimento ja foi declarada (faz a busca na tabela inteira)
    if(consulta_primeira_ocorrencia_procedimento(pilha, nome_procedimento) != NULL){
        return consulta_primeira_ocorrencia_procedimento(pilha, nome_procedimento)->memoria;
    }
    return -1;
}

int Pesquisa_chamfuncao_tabela(char* nome_funcao){
    // Verifica se a funcao ja foi declarada (faz a busca na tabela inteira)
    if(consulta_primeira_ocorrencia_funcao(pilha, nome_funcao) != NULL){
        return consulta_primeira_ocorrencia_funcao(pilha, nome_funcao)->memoria;
    }
    return -1;
}

int Pesquisa_primeira_marca_tabela(){
    // Pequisa em qual funcao se encontra para pegar o tipo
    if(consulta_escopo(pilha) != NULL){
        return consulta_escopo(pilha)->tipo; 
    }
    return -1; 
}

void Desempilha_volta_nivel(Pilha* p){
    No* q = p->Topo;
    while(q != NULL && q->escopo != 1){
        pop(p);
        q = p->Topo;
    }
    if(q != NULL && q->escopo == 1){
        q->escopo = 0;
    }
}

// Gera Codigo =======================================================================================================

void Gera_Codigo(char *rot, char *comando, char *tipo1, char *tipo2){

    arquivo_obj = fopen(nome_arquivo, "a");
    if (arquivo_obj == NULL) {
        perror("Erro ao criar arquivo OBJ");
    }
    
    fprintf(arquivo_obj, "%-4s %-8s %-4s %-4s\n", rot, comando, tipo1, tipo2);

    fclose(arquivo_obj);
    
}

