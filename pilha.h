#ifndef PILHA_H_INCLUDED
#define PILHA_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// escopo = 1: procedimento / funcao / programa
// escopo = 0: nada

// tipo = 0: nao declarado
// tipo = 1: nome do programa
// tipo = 2: var inteiro
// tipo = 3: var booleano
// tipo = 4: procedimento
// tipo = 5: funcao inteiro
// tipo = 6: funcao booleano

typedef struct nos
{
   char lexema[50];
   int escopo;
   int tipo;
   int memoria;
   struct nos *prox;
} No;

typedef struct pilha
{
   No *Topo;
} Pilha;

/* Cria pilha */
Pilha *CriaPilha(void)
{
   Pilha *p = (Pilha *)malloc(sizeof(Pilha));
   if (!p) {
      //fprintf(stderr, "Erro de alocacao em criar a pilha\n");
      exit(1);
   }
   p->Topo = NULL;
   return p;
}

/* Insere no inicio (retorna novo topo) */
No *ins_ini(No *topo, const char *l, int e, int t, int m)
{
   No *aux = (No *)malloc(sizeof(No));
   if (!aux) {
       //fprintf(stderr, "Erro de alocacao na insercao de um elemento na pilha\n");
       exit(1);
   }
   if (l)
       strncpy(aux->lexema, l, sizeof(aux->lexema) - 1);
   else
       aux->lexema[0] = '\0';
   aux->lexema[sizeof(aux->lexema) - 1] = '\0';
   aux->escopo = e;
   aux->tipo = t;
   aux->memoria = m;
   aux->prox = topo;
   return aux;
}

/* push: empilha */
void push(Pilha *p, const char *l, int e, int t, int m)
{
   if (!p) return;
   p->Topo = ins_ini(p->Topo, l, e, t, m);
}

/* retira o primeiro nó e retorna o novo topo; se l == NULL retorna NULL */
No *ret_ini(No *l)
{
   if (!l) return NULL;
   No *p = l->prox;
   free(l);
   return p;
}

/* pop: remove topo; não retorna valor */
void pop(Pilha *p)
{
   if (p == NULL || p->Topo == NULL)
   {
      /* Pilha vazia: apenas ignora ou você pode tratar como erro */
      //fprintf(stderr, "Erro: Remocao invalida. Pilha esta vazia.\n");
      return;
   }
   p->Topo = ret_ini(p->Topo);
}

/*
 * pop_op: remove topo e retorna uma cópia alocada do lexema
 * Observação: o chamador é responsável por free() da string retornada.
 */
char *pop_op(Pilha *p)
{
   if (p == NULL || p->Topo == NULL)
   {
      //fprintf(stderr, "Erro: Remocao invalida. Pilha esta vazia.\n");
      return NULL;
   }

   /* Faz cópia do lexema antes de liberar o nó */
   char *lexema_copy = (char *)malloc(strlen(p->Topo->lexema) + 1);
   if (!lexema_copy) {
       //fprintf(stderr, "Erro de alocacao na remocao do elemento na pilha\n");
       exit(1);
   }
   strcpy(lexema_copy, p->Topo->lexema);

   p->Topo = ret_ini(p->Topo);

   return lexema_copy;
}

/* libera toda a pilha e retorna NULL */
Pilha *libera(Pilha *p)
{
   if (!p) return NULL;
   No *q = p->Topo;
   while (q != NULL)
   {
      No *t = q->prox;
      free(q);
      q = t;
   }
   free(p);
   return NULL;
}

/* imprime pilha (ajustado para tipos corretos e proteção contra pilha vazia) */
void imprimePilha(Pilha *p)
{
   No *q;
   if (!p || p->Topo == NULL)
   {
      printf("Erro de impressao. Pilha esta vazia\n");
      return;
   }

   printf("\n\n\tImpressao da Pilha:\n");
   for (q = p->Topo; q != NULL; q = q->prox)
   {
      /* lexema (string), escopo (int), tipo (int), memoria (int) */
      printf("  %s  esc:%2d  tipo:%2d  mem:%2d\n", q->lexema, q->escopo, q->tipo, q->memoria);
   }
   printf("\n");
}

/* verifica se vazia; retorna 1 = vazia, 0 = nao vazia */
int vaziaPilha(Pilha *p)
{
   if (p == NULL || p->Topo == NULL)
   {
      return 1; // pilha vazia
   }
   return 0;
}

/* CONSULTAS (corrigidas para evitar dereferência nula e corrigir condição dos while) */

/* consulta_variavel_escopo: procura variavel com escopo == 0 a partir do topo */
No *consulta_variavel_escopo(Pilha *p, const char *nome_variavel)
{
    if (p == NULL || p->Topo == NULL) {
        /* Pilha vazia */
        return NULL;
    }

    No *q = p->Topo;
    /* testar q != NULL antes de acessar q->escopo */
    while (q != NULL && q->escopo == 0) {
        if (strcmp(q->lexema, nome_variavel) == 0)
            return q;
        q = q->prox;
    }
    return NULL;
}

/* procura a primeira ocorrencia de variavel (tipo 2 ou 3) */
No *consulta_primeira_ocorrencia_variavel(Pilha *p, const char *nome_variavel)
{
    if (p == NULL || p->Topo == NULL) {
        return NULL;
    }

    No *q = p->Topo;
    while (q != NULL) {
        if ((strcmp(q->lexema, nome_variavel) == 0) && (q->tipo == 2 || q->tipo == 3))
            return q;
        q = q->prox;
    }
    return NULL;
}

/* procura funcao ou variavel (tipos 2,3,5,6) */
No *consulta_primeira_ocorrencia_funcao_e_variavel(Pilha *p, const char *nome)
{
    if (p == NULL || p->Topo == NULL) {
        return NULL;
    }

    No *q = p->Topo;
    while (q != NULL) {
        if ((strcmp(q->lexema, nome) == 0) && (q->tipo == 2 || q->tipo == 3 || q->tipo == 5 || q->tipo == 6))
            return q;
        q = q->prox;
    }
    return NULL;
}

/* procura primeira ocorrencia de funcao (tipo 5 ou 6) */
No *consulta_primeira_ocorrencia_funcao(Pilha *p, const char *nome_variavel)
{
   if (!p || !p->Topo){
      return NULL;
   }

   No *q = p->Topo;
   while (q != NULL) {
      if ((strcmp(q->lexema, nome_variavel) == 0) && (q->tipo == 5 || q->tipo == 6))
         return q;
      q = q->prox;
   }
   return NULL;
}

/* procura procedimento (tipo 4) na parte de escopo 0 */
No *consulta_primeira_ocorrencia_procedimento(Pilha *p, const char *nome_variavel)
{
   if (!p || !p->Topo) {
      return NULL;
   }

   No *q = p->Topo;
   while (q != NULL) {
      if ((strcmp(q->lexema, nome_variavel) == 0) && (q->tipo == 4))
         return q;
      q = q->prox;
   }
   return NULL;
}

/* consulta_procedimento_escopo: procura procedimento com escopo==0 começando do topo */
No *consulta_procedimento_escopo(Pilha *p, const char *nome_procedimento)
{
   if (!p || !p->Topo) {
      return NULL;
   }

   No *q = p->Topo;
   while (q != NULL && q->escopo == 0) {
      if ((strcmp(q->lexema, nome_procedimento) == 0) && (q->tipo == 4))
         return q;
      q = q->prox;
   }
   return NULL;
}

/* consulta_funcao_escopo: procura funcao com escopo==0 */
No *consulta_funcao_escopo(Pilha *p, const char *nome_funcao)
{
   if (!p || !p->Topo) {
      return NULL;
   }

   No *q = p->Topo;
   while (q != NULL && q->escopo == 0) {
      if ((strcmp(q->lexema, nome_funcao) == 0) && (q->tipo == 5 || q->tipo == 6))
         return q;
      q = q->prox;
   }
   return NULL;
}

#endif // PILHA_H_INCLUDED
