#ifndef CLIENTES_CLIENTE_H
#define CLIENTES_CLIENTE_H
#include <stdio.h>
#include <stdlib.h>

typedef struct Cliente {
  int id;
  char nome[50];
  char cpf[15];
  char endereco[100];
  char email[50];
  char telefone[20];
  double multas;
  int codl;

} TCliente;

void imprime_cliente(TCliente *cliente, FILE *livros);

void cadastra_cliente(FILE *out);

// Salva cliente no arquivo out, na posicao atual do cursor
void salva_cliente(TCliente *cliente, FILE *out);

TCliente *le_cliente(FILE *in);

void le_clientes(FILE *in, FILE *livros);

void busca_sequencial_cliente(FILE *in, FILE *livros);

TCliente *busca_binaria_cliente(FILE *in, FILE *livros);

// Funções para ordenação externa
int classificacao_interna_cliente(FILE *in, char *nome_base_particao);
void intercalacao_cliente(char *nome_base_particao, int num_part, char *nome_final);


int tamanho_arquivo_cliente(FILE *arq);

long tamanho_cliente();

int tamanho_registro_cliente();

TCliente *busca_cliente_sis(FILE *in, int id);

long busca_posicao_cliente_sis(FILE *in, int id);

TCliente *cliente_cad(int id, const char *nome, const char *cpf, const char *endereco, const char *email, const char *telefone, double multas, int codl);

void cria_clientes_desordenado(FILE *out);

#endif
