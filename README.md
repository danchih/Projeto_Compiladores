# Projeto de Compiladores — Linguagem e Máquina Virtual

## Visão Geral

Este projeto tem como objetivo desenvolver um **compilador de uma única fase**, implementado inteiramente em **linguagem C**. O sistema completo transforma um código-fonte escrito na linguagem criada em um **arquivo objeto** que pode ser executado em uma **Máquina Virtual** desenvolvida especialmente para este projeto.

O compilador foi estruturado em quatro etapas essenciais, que juntas formam o pipeline completo de análise e geração de código.

---

## Etapas do Compilador

### 1. Analisador Lexical
Responsável por percorrer o texto fonte e dividi-lo em **tokens**, que são elementos fundamentais com identidade própria, como identificadores, números, operadores e palavras-chave.

### 2. Analisador Sintático
Avalia se a sequência de tokens está **gramaticalmente correta**, seguindo as regras formais da linguagem. Esta fase identifica estruturas como expressões, comandos, blocos e declarações.

### 3. Analisador Semântico
Interpreta o **significado** das construções sintáticas.  
Verifica:
- Declaração e escopo de variáveis  
- Compatibilidade de tipos  
- Operações válidas  
- Regras semânticas da linguagem  

Também fornece informações essenciais para a fase seguinte.

### 4. Geração de Código
A partir das informações semânticas, o compilador produz um **arquivo objeto (.obj)** contendo instruções adequadas para serem executadas na Máquina Virtual.  
É a etapa final da compilação.

---

## Interfaces Gráficas

Foram criadas duas interfaces para facilitar o uso do compilador e da máquina virtual:

### `interface_compilador.py`
- Executa todas as etapas do compilador (lexical, sintático, semântico e geração de código);
- Exibe mensagens de erro com clareza;
- Gera o arquivo objeto `.obj`.

### `interface_MaquinaVirtual.py`
- Executa arquivos `.obj` produzidos pelo compilador;
- Exibe o estado interno da máquina;
- Permite acompanhar instrução por instrução, facilitando depuração e aprendizado.

---
## Tecnologias Utilizadas:

[![My Skills](https://skillicons.dev/icons?i=py,vscode,c)](https://skillicons.dev)

---

## Integrantes

### - Daniela Akemi Hayashi

### - Flavia Cristina Medeiros

### - Giovana Salazar Alarcon

---

