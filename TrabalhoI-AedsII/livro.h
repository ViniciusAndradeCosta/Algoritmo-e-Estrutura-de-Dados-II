#ifndef LIVROS_LIVRO_H
#define LIVROS_LIVRO_H
#include <stdio.h>
#include <stdlib.h>

typedef struct Livro {

  double preco;
  int cod;
  int paginas;
  int edicao;
  int qtd;
  char titulo[50];
  char autor[50];
  char editora[15];
} TLivro;

void imprime_livro(TLivro *livro);

void cadastra_livro(FILE *out);

void salva_livro(TLivro *livro, FILE *out);

TLivro *le_livro(FILE *in);

void le_livros(FILE *in);

int tamanho_registro_livro();
int tamanho_arquivo_livro(FILE *arq);

void busca_sequencial_livro(FILE *in);
TLivro *busca_binaria_livro(FILE *in);

int classificacao_interna_livro(FILE *in, char *nome_base_particao);
void intercalacao_livro(char *nome_base_particao, int num_part, char *nome_final);

TLivro *busca_livro_sis(FILE *in, int cod);
int busca_posicao_livro_sis(FILE *in, int cod);

TLivro *livro_cad(double preco, int cod, int paginas, int edicao, int qtd,
                  char *titulo, char *autor, char *editora);

void cria_livros_desordenado(FILE *out);

void atualiza_livro_txt(FILE *dat_in);


#endif
