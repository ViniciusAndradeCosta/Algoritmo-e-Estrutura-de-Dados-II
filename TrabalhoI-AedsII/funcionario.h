#ifndef FUNCIONARIOS_FUNCIONARIO_H
#define FUNCIONARIOS_FUNCIONARIO_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Funcionario {
  int cod;
  char nome[50];
  char cpf[15];
  char data_nascimento[11];
  double salario;
  char endereco[100];
  char telefone[20];
  char email[50];
} TFunc;

void imprime_funcionario(TFunc *func);

void cria_funcionario(FILE *out);

void salva_funcionario(TFunc *func, FILE *out);

void le_funcionarios(FILE *in);

TFunc *le_funcionario(FILE *in);

int tamanho_registro_func();
int tamanho_arquivo_func(FILE *arq);

void busca_sequencial_funcionario(FILE *in);

TFunc *busca_binaria_funcionario(FILE *in);

int classificacao_interna_funcionario(FILE *in, char *nome_base_particao);
void intercalacao_funcionario(char *nome_base_particao, int num_part, char *nome_final);

void cria_funcionarios_desordenado(FILE *out);

TFunc *funcionario_cad(int cod, char *nome, char *cpf, char *data_nascimento, double salario, char *endereco, char *telefone, char *email);

void atualiza_funcionario_txt(FILE *dat_in);

#endif // FUNCIONARIOS_FUNCIONARIO_H
