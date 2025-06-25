#ifndef FUNCIONARIOS_FUNCIONARIO_H
#define FUNCIONARIOS_FUNCIONARIO_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Funcionario {
  int cod;
  char nome[50];
  char cpf[15];
  char data_nascimento[10];
  double salario;
  char endereco[100];
  char telefone[20];
  char email[50];
} TFunc;

// Imprime funcionario
void imprime_funcionario(TFunc *func);

// Cria funcionario. Lembrar de usar free(funcionario)
void cria_funcionario(FILE *out);

// Salva funcionario no arquivo out, na posicao atual do cursor
void salva_funcionario(TFunc *func, FILE *out);

// Le um funcionario do arquivo in na posicao atual do cursor
void le_funcionarios(FILE *in);

// Retorna um ponteiro para funcionario lido do arquivo
TFunc *le_funcionario(FILE *in);

// Retorna tamanho do funcionario em bytes
int tamanho_registro_func();
int tamanho_arquivo_func(FILE *arq);

void busca_sequencial_funcionario(FILE *in);

TFunc *busca_binaria_funcionario(FILE *in);

// Funções de ordenação externa
int classificacao_interna_funcionario(FILE *in, char *nome_base_particao);
void intercalacao_funcionario(char *nome_base_particao, int num_part, char *nome_final);

void cria_funcionarios_desordenado(FILE *out);

TFunc *funcionario_cad(int cod, char *nome, char *cpf, char *data_nascimento, double salario, char *endereco, char *telefone, char *email);

#endif // FUNCIONARIOS_FUNCIONARIO_H
