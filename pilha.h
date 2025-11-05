#ifndef PILHA_H_INCLUDED
#define PILHA_H_INCLUDED

// escopo = 1: procedimento / funcao / programa
// escopo = 0: nada

// tipo = 0: nao declarado
// tipo = 1: nome do programa
// tipo = 2: var inteiro
// tipo = 3: var booleano
// tipo = 4: procedimento
// tipo = 5: funcao inteiro
// tipo = 6: funcao booleano

// end = -1: nao guarda na memoria

typedef struct nos
{
   const char lexema[50];
   int escopo;
   int tipo;
   int memoria;
   struct nos *prox;
} No;

typedef struct pilha
{
   No *Topo;
} Pilha;

Pilha *CriaPilha(void)
{
   Pilha *p;
   p = (Pilha *)malloc(sizeof(Pilha));
   p->Topo = NULL;
   return p;
};

No *ins_ini(No *t, const char l, int e, int t, int m)
{
   No *aux = (No *)malloc(sizeof(No));
   aux->lexema = l;
   aux->escopo = e;
   aux->tipo = t;
   aux->memoria = m;
   aux->prox = t;
   return aux;
};

void push(Pilha *p, const char l, int e, int t, int m)
{
   p->Topo = ins_ini(p->Topo, l, e, t, m);
};

No *ret_ini(No *l)
{
   No *p = l->prox;
   free(l);
   return p;
};

void pop(Pilha *p)
{
   if (p == NULL)
   {
      printf("Pilha vazia.\n");
      exit(1); /* aborta programa */
   }
   p->Topo = ret_ini(p->Topo);

};

Pilha *libera(Pilha *p)
{
   No *q = p->Topo;
   while (q != NULL)
   {
      No *t = q->prox;
      free(q);
      q = t;
   }
   free(p);
   return (NULL);
};

void imprimePilha(Pilha *p)
{
   No *q;
   if (!p)
   {
      printf("\n\n\tPILHA VAZIA!!!!\n\n");
   }
   else
   {
      printf("\n\n\tImpressao da Pilha: ");
      for (q = p->Topo; q != NULL; q = q->prox)
      {
         printf("%s", q->lexema);
         printf("%2d", q->escopo);
         printf("%s", q->tipo);
         printf("%2d", q->memoria);
      }
      printf("\n\n");
   }
};

int vaziaPilha(Pilha *p)
{
   if (p->Topo == NULL)
   {
      return 1; // pilha vazia
   }
   return 0;
}

//aLTERADA
No *consulta_variavel_escopo(Pilha *p, char *nome_variavel)
{
    if (p == NULL || p->Topo == NULL) {
        printf("\nPilha vazia.\n");
        return NULL;
    }

    No *q = p->Topo;
    while (q != NULL && q->escopo == 0) {
        if (strcmp(q->lexema, nome_variavel) == 0)
            return q;
        q = q->prox; // *NO ANDA E ENCADEADO
    }
    return NULL; // Retorna NULL caso a variavel nao tenha sido encontrada
}
//aLTERADA
No *consulta_primeira_ocorrencia_variavel(Pilha *p, char *nome_variavel)
{
    if (p == NULL || p->Topo == NULL) {
        printf("\nPilha vazia.\n");
        return NULL;
    }

    No *q = p->Topo;
    while (q != NULL) {
        if ((strcmp(q->lexema, nome_variavel) == 0) && (q->tipo == 2 || q->tipo == 3))
            return q; // PARA RETORNAR O TIPO*
        q = q->prox; // *NO ANDA E ENCADEADO
    }
    return NULL; // *TEM QUE SER NULL MESMORetorna NULL caso a variavel nao tenha sido encontrada
}
//aLTERADA
No *consulta_primeira_ocorrencia_funcao_e_variavel(Pilha *p, char *nome)
{
    if (p == NULL || p->Topo == NULL) {
        printf("\nPilha vazia.\n");
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

//Alterada
No *consulta_primeira_ocorrencia_funcao(Pilha *p, char *nome_variavel)
{
   No *q = p->Topo;

   if (!p){
      printf("\n\n\tPILHA VAZIA!!!!\n\n");
      return NULL;
   }
   else{
      while (q != NULL) {
         if ((strcmp(q->lexema, nome) == 0) &&
               (q->tipo == 5 || q->tipo == 6))
               return q;
         q = q->prox;
      }
   }
   return NULL; // Retorna NULL caso a variavel nao tenha sido encontrada
}

No *consulta_primeira_ocorrencia_procedimento(Pilha *p, char *nome_variavel)
{
   No *q = p->Topo;

   if (!p){
      printf("\n\n\tPILHA VAZIA!!!!\n\n");
      return NULL;
   }
   else{
      while (q != NULL) {
         if ((strcmp(q->lexema, nome) == 0) && (q->tipo == 4))
               return q;
         q = q->prox;
      }
   }
   return NULL; // Retorna NULL caso a variavel nao tenha sido encontrada
}

No *consulta_procedimento_escopo(Pilha *p, char *nome_procedimento)
{
   No *q = p->Topo;

   if (!p){
      printf("\n\n\tPILHA VAZIA!!!!\n\n");
      return NULL;
   }else{
      while(q->escopo == 0 && q != NULL){
         if ((strcmp(q->lexema, nome_procedimento) == 0) && (q->tipo == 4))
               return q; // Retorna o no onde o procedimento foi encontrado
         q = q->prox;
      }
   }
   return NULL; // Retorna NULL caso o procedimento nao tenha sido encontrado
}

No *consulta_funcao_escopo(Pilha *p, char *nome_funcao)
{
   No *q = p->Topo;

   if (!p){
      printf("\n\n\tPILHA VAZIA!!!!\n\n");
      return NULL;
   }else{
      while(q->escopo == 0 && q != NULL){
         if ((strcmp(q->lexema, nome_funcao) == 0) && (q->tipo == 5 || q->tipo == 6))
               return q; // Retorna o no onde a funcao foi encontrada
         q = q->prox;
      }
   }
   return NULL; // Retorna NULL caso a funcao nao tenha sido encontrada
}

#endif // PILHA_H_INCLUDED
