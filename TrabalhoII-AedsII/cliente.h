#ifndef CLIENTES_CLIENTE_H
#define CLIENTES_CLIENTE_H

#include "livro.h"
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

// Funções de CRUD e Utilitários
void imprime_cliente(TCliente *cliente, FILE *livros);
void cadastra_cliente(FILE *out);
void salva_cliente(TCliente *cliente, FILE *out);
TCliente *le_cliente(FILE *in);
void le_clientes(FILE *in, FILE *livros);
void busca_sequencial_cliente(FILE *in, FILE *livros);
TCliente *busca_binaria_cliente(FILE *in, FILE *livros);
int tamanho_arquivo_cliente(FILE *arq);
int tamanho_registro_cliente();
TCliente *busca_cliente_sis(FILE *in, int id);
long busca_posicao_cliente_sis(FILE *in, int id);
TCliente *cliente_cad(int id, const char *nome, const char *cpf,
                      const char *endereco, const char *email,
                      const char *telefone, double multas, int codl);
void atualiza_cliente_txt(FILE *dat_in, FILE *livros_dat);

// REQUISITO 3: Esta função é a ferramenta para criar bases de dados de
// "diferentes tamanhos", conforme solicitado no requisito 3. A alteração na assinatura
// para receber o tamanho 'tam' é crucial para a comparação de desempenho.
void cria_clientes_desordenado(FILE *out, int tam);

// --- FUNÇÕES DE ORDENAÇÃO (PARTE I) ---

// REQUISITO 3.a: Esta é a declaração do método de partição da Atividade I
// (Classificação Interna), que será usado para a comparação.
int classificacao_interna_cliente(FILE *in, char *nome_base_particao);

// REQUISITO 3.a: Esta é a declaração do método de intercalação da Atividade I
// (Intercalação Básica), que será usado para a comparação.
void intercalacao_cliente(char *nome_base_particao, int num_part, char *nome_final);

// --- NOVAS FUNÇÕES DA PARTE 2 ---

// REQUISITO 1: Declaração da função que implementa a "Seleção por Substituição",
// cumprindo o primeiro requisito da atividade.
int selecao_substituicao_cliente(FILE *in, char *nome_base_particao);

// REQUISITO 2: Declaração da função que implementa a "Intercalação Ótima"
// (usando um heap), cumprindo o segundo requisito da atividade.
void intercalacao_otima_cliente(char *nome_base_particao, int num_part, char *nome_final);

#endif // CLIENTES_CLIENTE_H
