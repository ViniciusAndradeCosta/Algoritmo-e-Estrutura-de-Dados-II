#include "livro.h"
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAM_BUFFER 6

void atualiza_livro_txt(FILE *dat_in) {
    FILE *txt_out = fopen("livro.txt", "w");
    if (txt_out == NULL) {
        perror("Erro ao abrir livro.txt para escrita");
        return;
    }

    rewind(dat_in);
    TLivro *l;
    fprintf(txt_out, "============ LISTA DE LIVROS ============\n\n");
    while ((l = le_livro(dat_in)) != NULL) {
        fprintf(txt_out, "----------------------------------------\n");
        fprintf(txt_out, "C�digo: %d\n", l->cod);
        fprintf(txt_out, "T�tulo: %s\n", l->titulo);
        fprintf(txt_out, "Autor: %s\n", l->autor);
        fprintf(txt_out, "Editora: %s\n", l->editora);
        fprintf(txt_out, "Pre�o: R$ %.2f\n", l->preco);
        fprintf(txt_out, "P�ginas: %d\n", l->paginas);
        fprintf(txt_out, "Edi��o: %d\n", l->edicao);
        fprintf(txt_out, "Quantidade em estoque: %d\n", l->qtd);
        fprintf(txt_out, "----------------------------------------\n\n");
        free(l);
    }
    fclose(txt_out);
}

void limpar_buffer_livro() {
  int c;
  while ((c = getchar()) != '\n' && c != EOF) {
  }
}

int tamanho_registro_livro() { return sizeof(TLivro); }

int tamanho_arquivo_livro(FILE *arq) {
  fseek(arq, 0, SEEK_END);
  return ftell(arq) / tamanho_registro_livro();
}

void salva_livro(TLivro *livro, FILE *out) {
  fwrite(livro, sizeof(TLivro), 1, out);
}

TLivro *le_livro(FILE *in) {
  TLivro *livro = (TLivro *)malloc(sizeof(TLivro));
  if (livro == NULL) {
    perror("Erro ao alocar memoria para livro");
    return NULL;
  }
  if (fread(livro, sizeof(TLivro), 1, in) != 1) {
    free(livro);
    return NULL;
  }
  return livro;
}

TLivro *livro_cad(double preco, int cod, int paginas, int edicao, int qtd,
                  char *titulo, char *autor, char *editora) {
  TLivro *livro = (TLivro *)malloc(sizeof(TLivro));
  if (livro == NULL) {
    perror("Erro ao alocar memoria para livro");
    return NULL;
  }

  memset(livro, 0, sizeof(TLivro));

  livro->preco = preco;
  livro->cod = cod;
  livro->paginas = paginas;
  livro->edicao = edicao;
  livro->qtd = qtd;

  strncpy(livro->titulo, titulo, 49);
  livro->titulo[49] = '\0';

  strncpy(livro->autor, autor, 49);
  livro->autor[49] = '\0';

  strncpy(livro->editora, editora, 14);
  livro->editora[14] = '\0';

  return livro;
}

void cadastra_livro(FILE *out) {
  TLivro livro;
  memset(&livro, 0, sizeof(TLivro));

  printf("\nDigite o c�digo: ");
  scanf("%d", &livro.cod);
  limpar_buffer_livro();

  printf("Digite o titulo: ");
  fgets(livro.titulo, 50, stdin);
  livro.titulo[strcspn(livro.titulo, "\n")] = 0;

  printf("Digite o autor: ");
  fgets(livro.autor, 50, stdin);
  livro.autor[strcspn(livro.autor, "\n")] = 0;

  printf("Digite a editora: ");
  fgets(livro.editora, 15, stdin);
  livro.editora[strcspn(livro.editora, "\n")] = 0;

  printf("Digite o pre�o: ");
  scanf("%lf", &livro.preco);
  limpar_buffer_livro();

  printf("Digite o numero de paginas: ");
  scanf("%d", &livro.paginas);

  printf("Digite a edicao: ");
  scanf("%d", &livro.edicao);

  printf("Digite a quantidade dispon�vel: ");
  scanf("%d", &livro.qtd);

  fseek(out, 0, SEEK_END);
  salva_livro(&livro, out);
}

void imprime_livro(TLivro *livro) {
  printf("**********************************************\n");
  printf("C�digo do livro: %d\n", livro->cod);
  printf("T�tulo: %s\n", livro->titulo);
  printf("Autor: %s\n", livro->autor);
  printf("Editora: %s\n", livro->editora);
  printf("Pre�o: %.2f\n", livro->preco);
  printf("P�ginas: %d\n", livro->paginas);
  printf("Edi��o: %d\n", livro->edicao);
  printf("Quantidade dispon�vel: %d\n", livro->qtd);
  printf("**********************************************\n");
}

void le_livros(FILE *in) {
  printf("\n\nLendo livros do arquivo...\n\n");
  rewind(in);
  TLivro *l;
  while ((l = le_livro(in)) != NULL) {
    imprime_livro(l);
    free(l);
  }
}

void busca_sequencial_livro(FILE *in) {
  int n, comp = 0;
  TLivro *l = NULL;
  int encontrado = 0;

  printf("\n--- BUSCA SEQUENCIAL DE LIVRO ---\n");
  printf("Digite o c�digo do livro: ");
  scanf("%d", &n);
  limpar_buffer_livro();

  rewind(in);
  clock_t inicio = clock();

  while ((l = le_livro(in)) != NULL) {
    comp++;
    if (n == l->cod) {
      printf("\nLIVRO ENCONTRADO!\n");
      imprime_livro(l);
      encontrado = 1;
      free(l);
      break;
    }
    free(l);
  }

  if (!encontrado) {
    printf("\nLIVRO N�O ENCONTRADO.\n");
  }

  clock_t fim = clock();
  double tempoTotal = (double)(fim - inicio) / CLOCKS_PER_SEC;

  printf("-----------------------------------------\n");
  printf("Tempo de busca: %f segundos\n", tempoTotal);
  printf("N�mero de compara��es: %d\n", comp);
  printf("-----------------------------------------\n");

  FILE *log_file = fopen("log_buscas_livro.txt", "a");
  if (log_file != NULL) {
    fprintf(log_file,
            "Tipo de Busca: Sequencial; C�digo Procurado: %d; Encontrado: %s; "
            "Compara��es: %d; Tempo: %f segundos\n",
            n, encontrado ? "Sim" : "Nao", comp, tempoTotal);
    fclose(log_file);
  }
}

TLivro *busca_binaria_livro(FILE *in) {
  int tam = tamanho_arquivo_livro(in);
  if (tam <= 0) {
    printf("Arquivo vazio ou n�o � poss�vel realizar a busca.\n");
    return NULL;
  }

  int n, comp = 0;
  int left = 0, right = tam - 1;
  int encontrado = 0;
  TLivro *livro_encontrado = NULL;

  printf("\n--- BUSCA BIN�RIA DE LIVRO ---\n");
  printf("(Certifique-se de que o arquivo esteja ordenado por c�digo)\n");
  printf("Digite o c�digo do livro: ");
  scanf("%d", &n);
  limpar_buffer_livro();

  clock_t inicio = clock();

  while (left <= right) {
    int middle = left + (right - left) / 2;
    fseek(in, middle * tamanho_registro_livro(), SEEK_SET);
    TLivro *livro = le_livro(in);
    if (livro == NULL)
      break;

    comp++;
    if (n == livro->cod) {
      printf("\nLIVRO ENCONTRADO!\n");
      imprime_livro(livro);
      encontrado = 1;
      livro_encontrado = livro;
      break;
    } else if (livro->cod < n) {
      left = middle + 1;
    } else {
      right = middle - 1;
    }
    free(livro);
  }

  if (!encontrado) {
    printf("\nLIVRO N�O ENCONTRADO.\n");
  }

  clock_t fim = clock();
  double tempoTotal = (double)(fim - inicio) / CLOCKS_PER_SEC;

  printf("-----------------------------------------\n");
  printf("Tempo de busca: %f segundos\n", tempoTotal);
  printf("N�mero de compara��es: %d\n", comp);
  printf("-----------------------------------------\n");

  FILE *log_file = fopen("log_buscas_livro.txt", "a");
  if (log_file != NULL) {
    fprintf(log_file,
            "Tipo de Busca: Bin�ria; C�digo Procurado: %d; Encontrado: %s; "
            "Compara��es: %d; Tempo: %f segundos\n",
            n, encontrado ? "Sim" : "Nao", comp, tempoTotal);
    fclose(log_file);
  }

  return livro_encontrado;
}

TLivro *busca_livro_sis(FILE *in, int cod) {
  rewind(in);
  TLivro *f;
  while ((f = le_livro(in)) != NULL) {
    if (cod == f->cod) {
      return f;
    }
    free(f);
  }
  return NULL;
}

int busca_posicao_livro_sis(FILE *in, int cod) {
  rewind(in);
  TLivro *f;
  int i = 0;
  while ((f = le_livro(in)) != NULL) {
    if (cod == f->cod) {
      free(f);
      return i;
    }
    i++;
    free(f);
  }
  return -1;
}

int compara_livros(const void *a, const void *b) {
  TLivro *l1 = *(TLivro **)a;
  TLivro *l2 = *(TLivro **)b;
  if (l1->cod < l2->cod)
    return -1;
  if (l1->cod > l2->cod)
    return 1;
  return 0;
}

int classificacao_interna_livro(FILE *in, char *nome_base_particao) {
  rewind(in);
  int num_particoes = 0;
  TLivro *buffer[TAM_BUFFER];
  int count = 0;

  TLivro *l = le_livro(in);
  while (l != NULL) {
    buffer[count++] = l;
    if (count == TAM_BUFFER) {
      qsort(buffer, count, sizeof(TLivro *), compara_livros);
      char nome_particao[100];
      sprintf(nome_particao, "%s%d.dat", nome_base_particao, num_particoes);
      FILE *p = fopen(nome_particao, "wb");
      for (int i = 0; i < count; i++) {
        salva_livro(buffer[i], p);
        free(buffer[i]);
      }
      fclose(p);
      num_particoes++;
      count = 0;
    }
    l = le_livro(in);
  }

  if (count > 0) {
    qsort(buffer, count, sizeof(TLivro *), compara_livros);
    char nome_particao[100];
    sprintf(nome_particao, "%s%d.dat", nome_base_particao, num_particoes);
    FILE *p = fopen(nome_particao, "wb");
    for (int i = 0; i < count; i++) {
      salva_livro(buffer[i], p);
      free(buffer[i]);
    }
    fclose(p);
    num_particoes++;
  }
  return num_particoes;
}

void intercalacao_livro(char *nome_base_particao, int num_part,
                        char *nome_final) {
  FILE *out = fopen(nome_final, "wb");
  FILE *particoes[num_part];
  TLivro *registros[num_part];

  for (int i = 0; i < num_part; i++) {
    char nome_part[100];
    sprintf(nome_part, "%s%d.dat", nome_base_particao, i);
    particoes[i] = fopen(nome_part, "rb");
    registros[i] = le_livro(particoes[i]);
  }

  while (1) {
    int menor_cod = INT_MAX;
    int indice_menor = -1;

    for (int i = 0; i < num_part; i++) {
      if (registros[i] != NULL && registros[i]->cod < menor_cod) {
        menor_cod = registros[i]->cod;
        indice_menor = i;
      }
    }

    if (indice_menor == -1)
      break;

    salva_livro(registros[indice_menor], out);
    free(registros[indice_menor]);
    registros[indice_menor] = le_livro(particoes[indice_menor]);
  }

  for (int i = 0; i < num_part; i++) {
    fclose(particoes[i]);
    char nome_part[100];
    sprintf(nome_part, "%s%d.dat", nome_base_particao, i);
    remove(nome_part);
  }
  fclose(out);
}

void cria_livros_desordenado(FILE *out) {
  if(freopen("livro.dat", "w+b", out) == NULL){
      perror("Erro ao limpar o arquivo de livros");
      return;
  }
  rewind(out);

  const int tam = 20;
  int vet[tam];

  srand(time(NULL));
  for (int i = 0; i < tam; i++) {
    int a;
    int existe;
    do {
      a = 1 + rand() % 100;
      existe = 0;
      for (int j = 0; j < i; j++) {
        if (vet[j] == a) {
          existe = 1;
          break;
        }
      }
    } while (existe);
    vet[i] = a;
  }

  printf("Criando base de dados de livros desordenada...\n");
  for (int i = 0; i < tam; i++) {

    TLivro *l = livro_cad(20.0, vet[i], 223, 2, 7,
                          "Harry Potter", "JK Rowling", "Editora Coluna");
    if (l != NULL) {
      salva_livro(l, out);
      free(l);
    }
  }
  printf("Base de dados criada com sucesso.\n");
}
