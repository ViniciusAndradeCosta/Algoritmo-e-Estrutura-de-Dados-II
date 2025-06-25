#include "funcionario.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#define TAM_BUFFER 6

void limpar_buffer_func() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int tamanho_registro_func() { return sizeof(TFunc); }

int tamanho_arquivo_func(FILE *arq) {
  fseek(arq, 0, SEEK_END);
  int tam = ftell(arq) / tamanho_registro_func();
  return tam;
}

// Salva funcionario no arquivo out
void salva_funcionario(TFunc *func, FILE *out) {
  fwrite(func, sizeof(TFunc), 1, out);
}

// Imprime funcionario
void imprime_funcionario(TFunc *func) {
  printf("**********************************************\n");
  printf("Funcion�rio de c�digo: %d\n", func->cod);
  printf("Nome: %s\n", func->nome);
  printf("CPF: %s\n", func->cpf);
  printf("Data de Nascimento: %s\n", func->data_nascimento);
  printf("Sal�rio: %.2f\n", func->salario);
  printf("Endere�o: %s\n", func->endereco);
  printf("Email: %s\n", func->email);
  printf("Telefone: %s\n", func->telefone);
  printf("**********************************************\n");
}

// Le um funcionario do arquivo in na posicao atual do cursor
TFunc *le_funcionario(FILE *in) {
  TFunc *func = (TFunc *)malloc(sizeof(TFunc));
  if (fread(func, sizeof(TFunc), 1, in) != 1) {
    free(func);
    return NULL;
  }
  return func;
}

// Cria funcionario
void cria_funcionario(FILE *out) {
  TFunc func;
  memset(&func, 0, sizeof(TFunc));

  printf("\nDigite o c�digo: ");
  scanf("%d", &func.cod);
  limpar_buffer_func();

  printf("Digite o nome: ");
  fgets(func.nome, 50, stdin);
  func.nome[strcspn(func.nome, "\n")] = 0;

  printf("Digite o cpf: ");
  fgets(func.cpf, 15, stdin);
  func.cpf[strcspn(func.cpf, "\n")] = 0;

  printf("Digite a data de nascimento (DD/MM/AAAA): ");
  fgets(func.data_nascimento, 11, stdin);
  func.data_nascimento[strcspn(func.data_nascimento, "\n")] = 0;
  limpar_buffer_func();

  printf("Digite o salario: ");
  scanf("%lf", &func.salario);
  limpar_buffer_func();

  printf("Digite o endereco: ");
  fgets(func.endereco, 100, stdin);
  func.endereco[strcspn(func.endereco, "\n")] = 0;

  printf("Digite o telefone: ");
  fgets(func.telefone, 20, stdin);
  func.telefone[strcspn(func.telefone, "\n")] = 0;

  printf("Digite o email: ");
  fgets(func.email, 50, stdin);
  func.email[strcspn(func.email, "\n")] = 0;

  fseek(out, 0, SEEK_END);
  salva_funcionario(&func, out);
}

// Fun��o de compara��o para qsort
int compara_funcionarios(const void *a, const void *b) {
    TFunc *f1 = *(TFunc **)a;
    TFunc *f2 = *(TFunc **)b;
    if (f1->cod < f2->cod) return -1;
    if (f1->cod > f2->cod) return 1;
    return 0;
}

// Classifica��o interna para funcion�rios
int classificacao_interna_funcionario(FILE *in, char *nome_base_particao) {
    rewind(in);
    int num_particoes = 0;
    TFunc *buffer[TAM_BUFFER];
    int count = 0;

    TFunc *f = le_funcionario(in);
    while (f != NULL) {
        buffer[count++] = f;
        if (count == TAM_BUFFER) {
            qsort(buffer, count, sizeof(TFunc*), compara_funcionarios);
            char nome_particao[100];
            sprintf(nome_particao, "%s%d.dat", nome_base_particao, num_particoes);
            FILE *p = fopen(nome_particao, "wb");
            for (int i = 0; i < count; i++) {
                salva_funcionario(buffer[i], p);
                free(buffer[i]);
            }
            fclose(p);
            num_particoes++;
            count = 0;
        }
        f = le_funcionario(in);
    }

    if (count > 0) {
        qsort(buffer, count, sizeof(TFunc*), compara_funcionarios);
        char nome_particao[100];
        sprintf(nome_particao, "%s%d.dat", nome_base_particao, num_particoes);
        FILE *p = fopen(nome_particao, "wb");
        for (int i = 0; i < count; i++) {
            salva_funcionario(buffer[i], p);
            free(buffer[i]);
        }
        fclose(p);
        num_particoes++;
    }
    return num_particoes;
}

// Intercala��o para funcion�rios
void intercalacao_funcionario(char *nome_base_particao, int num_part, char *nome_final) {
    FILE *out = fopen(nome_final, "wb");
    FILE *particoes[num_part];
    TFunc *registros[num_part];

    for (int i = 0; i < num_part; i++) {
        char nome_part[100];
        sprintf(nome_part, "%s%d.dat", nome_base_particao, i);
        particoes[i] = fopen(nome_part, "rb");
        registros[i] = le_funcionario(particoes[i]);
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

        if (indice_menor == -1) break;

        salva_funcionario(registros[indice_menor], out);
        free(registros[indice_menor]);
        registros[indice_menor] = le_funcionario(particoes[indice_menor]);
    }

    for (int i = 0; i < num_part; i++) {
        fclose(particoes[i]);
        char nome_part[100];
        sprintf(nome_part, "%s%d.dat", nome_base_particao, i);
        remove(nome_part);
    }
    fclose(out);
}


TFunc *funcionario_cad(int cod, char *nome, char *cpf, char *data_nascimento,
                       double salario, char *endereco, char *telefone,
                       char *email) {
  TFunc *func = (TFunc *)malloc(sizeof(TFunc));
  if (func)
    memset(func, 0, sizeof(TFunc));

  func->cod = cod;
  strcpy(func->nome, nome);
  strcpy(func->cpf, cpf);
  strcpy(func->data_nascimento, data_nascimento);
  func->salario = salario;
  strcpy(func->endereco, endereco);
  strcpy(func->telefone, telefone);
  strcpy(func->email, email);

  return func;
}

void cria_funcionarios_desordenado(FILE *out) {
  rewind(out);
  const int tam = 5;
  int vet[tam];

  srand(time(NULL));
  for (int i = 0; i < tam; i++) {
      int a;
      int existe;
      do {
          existe = 0;
          a = 1 + rand() % 50;
          for (int j = 0; j < i; j++) {
              if (a == vet[j]) {
                  existe = 1;
                  break;
              }
          }
      } while (existe);
      vet[i] = a;
  }

  printf("Criando base de dados de funcion�rios desordenada...\n");
  for (int i = 0; i < tam; i++) {
    char nome_temp[50];
    sprintf(nome_temp, "Funcionario %d", vet[i]);
    TFunc *f = funcionario_cad(vet[i], nome_temp, "123.456.789-10",
                               "15/06/1985", 3500, "Rua de tal", "31987200414",
                               "funcionario@gmail.com");
    salva_funcionario(f, out);
    free(f);
  }
  printf("Base de dados criada com sucesso.\n");
}

void le_funcionarios(FILE *in) {
  printf("\n\nLendo funcionarios do arquivo...\n\n");
  rewind(in);
  TFunc *f;
  while ((f = le_funcionario(in)) != NULL) {
    imprime_funcionario(f);
    free(f);
  }
}

// Em funcionario.c
void busca_sequencial_funcionario(FILE *in) {
  int comp = 0;
  int cod;
  TFunc *f = NULL;
  int encontrado = 0; // Flag

  printf("\n--- BUSCA SEQUENCIAL DE FUNCION�RIO ---\n");
  printf("Digite o c�digo do funcion�rio: ");
  scanf("%d", &cod);
  limpar_buffer_func();

  rewind(in);
  clock_t inicio = clock();
  while ((f = le_funcionario(in)) != NULL) {
    comp++;
    if (cod == f->cod) {
      printf("\nFUNCION�RIO ENCONTRADO!\n");
      imprime_funcionario(f);
      encontrado = 1;
      free(f);
      break;
    }
    free(f);
  }

  if (!encontrado) {
      printf("\nFUNCION�RIO N�O ENCONTRADO.\n");
  }

  clock_t fim = clock();
  double tempoTotal = (double)(fim - inicio) / CLOCKS_PER_SEC;

  printf("-----------------------------------------\n");
  printf("Tempo de busca: %f segundos\n", tempoTotal);
  printf("N�mero de compara��es: %d\n", comp);
  printf("-----------------------------------------\n");

  // *** IN�CIO DA MODIFICA��O PARA SALVAR LOG ***
  FILE *log_file = fopen("log_buscas_funcionario.txt", "a");
  if (log_file != NULL) {
      fprintf(log_file, "Tipo de Busca: Sequencial; C�digo Procurado: %d; Encontrado: %s; Compara��es: %d; Tempo: %f segundos\n",
              cod, encontrado ? "Sim" : "Nao", comp, tempoTotal);
      fclose(log_file);
  }
  // *** FIM DA MODIFICA��O ***
}

// Em funcionario.c
TFunc *busca_binaria_funcionario(FILE *in) {
    int tam = tamanho_arquivo_func(in);
    if (tam <= 0) {
        printf("Arquivo vazio ou n�o � poss�vel realizar a busca.\n");
        return NULL;
    }

    int n;
    int left = 0;
    int right = tam - 1;
    int comp = 0;
    int encontrado = 0; // Flag
    TFunc *func_encontrado = NULL;

    printf("\n--- BUSCA BIN�RIA DE FUNCION�RIO ---\n");
    printf("(Certifique-se de que o arquivo esteja ordenado por c�digo)\n");
    printf("Digite o c�digo do funcion�rio: ");
    scanf("%d", &n);
    limpar_buffer_func();

    clock_t inicio = clock();

    while (left <= right) {
        int middle = left + (right - left) / 2;
        fseek(in, middle * tamanho_registro_func(), SEEK_SET);
        TFunc *func = le_funcionario(in);
        if (func == NULL) break;

        comp++;
        if (n == func->cod) {
            printf("\nFUNCION�RIO ENCONTRADO!\n");
            imprime_funcionario(func);
            encontrado = 1;
            func_encontrado = func;
            break;
        } else if (func->cod < n) {
            left = middle + 1;
        } else {
            right = middle - 1;
        }
        free(func);
    }

    if (!encontrado) {
        printf("\nFUNCION�RIO N�O ENCONTRADO.\n");
    }

    clock_t fim = clock();
    double tempoTotal = (double)(fim - inicio) / CLOCKS_PER_SEC;

    printf("-----------------------------------------\n");
    printf("Tempo de busca: %f segundos\n", tempoTotal);
    printf("N�mero de compara��es: %d\n", comp);
    printf("-----------------------------------------\n");

    // *** IN�CIO DA MODIFICA��O PARA SALVAR LOG ***
    FILE *log_file = fopen("log_buscas_funcionario.txt", "a");
    if (log_file != NULL) {
        fprintf(log_file, "Tipo de Busca: Bin�ria; C�digo Procurado: %d; Encontrado: %s; Compara��es: %d; Tempo: %f segundos\n",
                n, encontrado ? "Sim" : "Nao", comp, tempoTotal);
        fclose(log_file);
    }
    // *** FIM DA MODIFICA��O ***

    return func_encontrado;
}
