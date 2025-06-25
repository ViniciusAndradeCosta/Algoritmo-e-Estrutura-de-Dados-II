#ifndef LIVROS_LIVRO_H
#define LIVROS_LIVRO_H
#include <stdio.h>
#include <stdlib.h>

typedef struct Livro {
  int cod;

  char autor[50];
  char titulo[50];
  double preco;
  char editora[15];
  int paginas;
  int edicao;
  int qtd;

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

// Funções de ordenação externa
int classificacao_interna_livro(FILE *in, char *nome_base_particao);
void intercalacao_livro(char *nome_base_particao, int num_part, char *nome_final);

TLivro *busca_livro_sis(FILE *in, int cod);
int busca_posicao_livro_sis(FILE *in, int cod);

TLivro *livro_cad(int cod, char *autor, char *titulo, char *editora,
                  double preco, int paginas, int edicao, int qtd);

void cria_livros_desordenado(FILE *out);

#endif
