#include "cliente.h"
#include "livro.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <stdio.h>

#define TAM_BUFFER 6

void atualiza_cliente_txt(FILE *dat_in, FILE *livros_dat) {
    FILE *txt_out = fopen("cliente.txt", "w");
    if (txt_out == NULL) {
        perror("Erro ao abrir cliente.txt para escrita");
        return;
    }

    rewind(dat_in);
    TCliente *c;
    fprintf(txt_out, "============ LISTA DE CLIENTES ============\n\n");
    while ((c = le_cliente(dat_in)) != NULL) {
        fprintf(txt_out, "----------------------------------------\n");
        fprintf(txt_out, "ID do Cliente: %d\n", c->id);
        fprintf(txt_out, "Nome: %s\n", c->nome);
        fprintf(txt_out, "CPF: %s\n", c->cpf);
        fprintf(txt_out, "Endere�o: %s\n", c->endereco);
        fprintf(txt_out, "Email: %s\n", c->email);
        fprintf(txt_out, "Telefone: %s\n", c->telefone);
        fprintf(txt_out, "Multas Acumuladas: R$ %.2f\n", c->multas);

        if (c->codl != 0) {
            fprintf(txt_out, "Livro Alugado (C�digo): %d\n", c->codl);
             TLivro *livro = busca_livro_sis(livros_dat, c->codl);
             if (livro != NULL) {
                 fprintf(txt_out, "  -> T�tulo: %s\n", livro->titulo);
                 free(livro);
             }
        } else {
            fprintf(txt_out, "Nenhum livro alugado.\n");
        }
        fprintf(txt_out, "----------------------------------------\n\n");
        free(c);
    }
    fclose(txt_out);
}

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int compara_clientes(const void *a, const void *b) {
    TCliente *c1 = *(TCliente **)a;
    TCliente *c2 = *(TCliente **)b;
    if (c1->id < c2->id) return -1;
    if (c1->id > c2->id) return 1;
    return 0;
}

int classificacao_interna_cliente(FILE *in, char *nome_base_particao) {
    rewind(in);
    int num_particoes = 0;
    TCliente *buffer[TAM_BUFFER];
    int count = 0;

    TCliente *c = le_cliente(in);
    while (c != NULL) {
        buffer[count++] = c;
        if (count == TAM_BUFFER) {
            qsort(buffer, count, sizeof(TCliente*), compara_clientes);

            char nome_particao[100];
            sprintf(nome_particao, "%s%d.dat", nome_base_particao, num_particoes);
            FILE *p = fopen(nome_particao, "wb");
            if (p == NULL) {
                perror("Erro ao criar parti��o");
                exit(1);
            }

            for (int i = 0; i < count; i++) {
                salva_cliente(buffer[i], p);
                free(buffer[i]);
            }

            fclose(p);
            num_particoes++;
            count = 0;
        }
        c = le_cliente(in);
    }

    if (count > 0) {
        qsort(buffer, count, sizeof(TCliente*), compara_clientes);

        char nome_particao[100];
        sprintf(nome_particao, "%s%d.dat", nome_base_particao, num_particoes);
        FILE *p = fopen(nome_particao, "wb");
        if (p == NULL) {
            perror("Erro ao criar parti��o");
            exit(1);
        }

        for (int i = 0; i < count; i++) {
            salva_cliente(buffer[i], p);
            free(buffer[i]);
        }

        fclose(p);
        num_particoes++;
    }

    return num_particoes;
}

void intercalacao_cliente(char *nome_base_particao, int num_part, char *nome_final) {
    FILE *out = fopen(nome_final, "wb");
    if (out == NULL) {
        perror("Erro ao criar arquivo de sa�da final");
        return;
    }

    FILE *particoes[num_part];
    TCliente *registros[num_part];

    for (int i = 0; i < num_part; i++) {
        char nome_part[100];
        sprintf(nome_part, "%s%d.dat", nome_base_particao, i);
        particoes[i] = fopen(nome_part, "rb");
        if (particoes[i] == NULL) {
            perror("Erro ao abrir parti��o para intercala��o");
            for (int j = 0; j < i; j++) fclose(particoes[j]);
            fclose(out);
            return;
        }
        registros[i] = le_cliente(particoes[i]);
    }

    while (1) {
        int menor_id = INT_MAX;
        int indice_menor = -1;

        for (int i = 0; i < num_part; i++) {
            if (registros[i] != NULL && registros[i]->id < menor_id) {
                menor_id = registros[i]->id;
                indice_menor = i;
            }
        }

        if (indice_menor == -1) break;

        salva_cliente(registros[indice_menor], out);
        free(registros[indice_menor]);
        registros[indice_menor] = le_cliente(particoes[indice_menor]);
    }

    for (int i = 0; i < num_part; i++) {
        fclose(particoes[i]);
        char nome_part[100];
        sprintf(nome_part, "%s%d.dat", nome_base_particao, i);
        remove(nome_part);
    }

    fclose(out);
}


TCliente *busca_cliente_sis(FILE *in, int id) {
    if (in == NULL) return NULL;

    rewind(in);
    TCliente *cliente;

    while ((cliente = le_cliente(in)) != NULL) {
        if (cliente->id == id) {
            return cliente;
        }
        free(cliente);
    }

    return NULL;
}

long busca_posicao_cliente_sis(FILE *in, int id) {
    if (in == NULL) return -1;

    rewind(in);
    TCliente *cliente;
    long pos = 0;

    while (fseek(in, pos * tamanho_registro_cliente(), SEEK_SET) == 0) {
        cliente = le_cliente(in);
        if (cliente == NULL) break;
        if (cliente->id == id) {
            free(cliente);
            return pos;
        }
        free(cliente);
        pos++;
    }

    return -1;
}

long tamanho_cliente() {
  return sizeof(TCliente);
}

void imprime_cliente(TCliente *cliente, FILE *livros) {
  if (cliente == NULL) {
    printf("\nCliente inv�lido (NULL)\n");
    return;
  }

  printf("\n*************************************************");
  printf("\nCliente de id: %d", cliente->id);
  printf("\nNome: %s", cliente->nome);
  printf("\nCPF: %s", cliente->cpf);
  printf("\nEndere�o: %s", cliente->endereco);
  printf("\nEmail: %s", cliente->email);
  printf("\nTelefone: %s", cliente->telefone);
  printf("\nMultas: %.2lf", cliente->multas);

  if (cliente->codl != 0 && livros != NULL) {
    printf("\nLivro alugado:\n");
    TLivro *livro = busca_livro_sis(livros, cliente->codl);
    if (livro != NULL) {
      imprime_livro(livro);
      free(livro);
    }
  }
  printf("\n*************************************************\n");
}

void cadastra_cliente(FILE *out) {
  if (out == NULL) {
    printf("Erro: arquivo de sa�da inv�lido\n");
    return;
  }

  TCliente cliente;
  memset(&cliente, 0, sizeof(TCliente));

  printf("\nDigite o id: ");
  if (scanf("%d", &cliente.id) != 1) {
    printf("Erro ao ler ID\n");
    limpar_buffer();
    return;
  }
  limpar_buffer();

  printf("Digite o nome (max 49 caracteres): ");
  fgets(cliente.nome, 50, stdin);
  cliente.nome[strcspn(cliente.nome, "\n")] = '\0';

  printf("Digite o CPF (max 14 caracteres): ");
  fgets(cliente.cpf, 15, stdin);
  cliente.cpf[strcspn(cliente.cpf, "\n")] = '\0';

  printf("Digite o endere�o (max 99 caracteres): ");
  fgets(cliente.endereco, 100, stdin);
  cliente.endereco[strcspn(cliente.endereco, "\n")] = '\0';

  printf("Digite o telefone (max 19 caracteres): ");
  fgets(cliente.telefone, 20, stdin);
  cliente.telefone[strcspn(cliente.telefone, "\n")] = '\0';

  printf("Digite o email (max 49 caracteres): ");
  fgets(cliente.email, 50, stdin);
  cliente.email[strcspn(cliente.email, "\n")] = '\0';

  cliente.multas = 0.0;
  cliente.codl = 0;

  fseek(out, 0, SEEK_END);
  salva_cliente(&cliente, out);
}

void salva_cliente(TCliente *cliente, FILE *out) {
  if (cliente == NULL || out == NULL) return;
  fwrite(cliente, sizeof(TCliente), 1, out);
}

TCliente *le_cliente(FILE *in) {
  if (in == NULL) return NULL;
  TCliente *cliente = (TCliente *)malloc(sizeof(TCliente));
  if (cliente == NULL) return NULL;

  if (fread(cliente, sizeof(TCliente), 1, in) != 1) {
    free(cliente);
    return NULL;
  }
  return cliente;
}

void le_clientes(FILE *in, FILE *livros) {
  if (in == NULL) {
    printf("Erro: arquivo de entrada inv�lido\n");
    return;
  }

  printf("\n\nLendo clientes do arquivo...\n\n");
  rewind(in);

  TCliente *c;
  while ((c = le_cliente(in)) != NULL) {
    imprime_cliente(c, livros);
    free(c);
  }
}

int tamanho_registro_cliente() {
  return sizeof(TCliente);
}

int tamanho_arquivo_cliente(FILE *arq) {
  if (arq == NULL) return 0;
  fseek(arq, 0, SEEK_END);
  long tamanho = ftell(arq);
  return (int)(tamanho / tamanho_registro_cliente());
}

void busca_sequencial_cliente(FILE *in, FILE *livros) {
  if (in == NULL) {
    printf("Erro: arquivo de entrada inv�lido\n");
    return;
  }

  int comp = 0;
  int id;
  TCliente *f = NULL;
  int encontrado = 0;

  printf("\n--- BUSCA SEQUENCIAL DE CLIENTE ---\n");
  printf("Digite o ID do cliente: ");
  if (scanf("%d", &id) != 1) {
    printf("Erro ao ler ID.\n");
    limpar_buffer();
    return;
  }
  limpar_buffer();

  rewind(in);
  clock_t inicio = clock();

  while ((f = le_cliente(in)) != NULL) {
    comp++;
    if (id == f->id) {
      printf("\nCLIENTE ENCONTRADO!\n");
      imprime_cliente(f, livros);
      encontrado = 1;
      free(f);
      break;
    }
    free(f);
  }

  if (!encontrado) {
      printf("\nCLIENTE N�O ENCONTRADO.\n");
  }

  clock_t fim = clock();
  double tempoTotal = (double)(fim - inicio) / CLOCKS_PER_SEC;

  printf("-----------------------------------------\n");
  printf("Tempo de busca: %f segundos\n", tempoTotal);
  printf("N�mero de compara��es: %d\n", comp);
  printf("-----------------------------------------\n");

  FILE *log_file = fopen("log_buscas_cliente.txt", "a");
  if (log_file != NULL) {
      fprintf(log_file, "Tipo de Busca: Sequencial; ID Procurado: %d; Encontrado: %s; Compara��es: %d; Tempo: %f segundos\n",
              id, encontrado ? "Sim" : "Nao", comp, tempoTotal);
      fclose(log_file);
  }
}

TCliente *busca_binaria_cliente(FILE *in, FILE *livros) {
    if (in == NULL) {
        printf("Erro: arquivo de entrada inv�lido.\n");
        return NULL;
    }

    int tam = tamanho_arquivo_cliente(in);
    if (tam <= 0) {
        printf("Arquivo vazio ou n�o � poss�vel realizar a busca.\n");
        return NULL;
    }

    int left = 0;
    int right = tam - 1;
    int comp = 0;
    int n;
    int encontrado = 0;

    printf("\n--- BUSCA BIN�RIA DE CLIENTE ---\n");
    printf("(Certifique-se de que o arquivo esteja ordenado por ID)\n");
    printf("Digite o ID do cliente: ");

    if (scanf("%d", &n) != 1) {
        printf("Erro ao ler ID.\n");
        limpar_buffer();
        return NULL;
    }
    limpar_buffer();

    clock_t inicio = clock();
    TCliente *cliente_encontrado = NULL;

    while (left <= right) {
        int middle = left + (right - left) / 2;
        fseek(in, middle * tamanho_registro_cliente(), SEEK_SET);
        TCliente *cliente = le_cliente(in);
        if (cliente == NULL) break;

        comp++;
        if (n == cliente->id) {
            printf("\nCLIENTE ENCONTRADO!\n");
            imprime_cliente(cliente, livros);
            encontrado = 1;
            cliente_encontrado = cliente;
            break;
        } else if (cliente->id < n) {
            left = middle + 1;
        } else {
            right = middle - 1;
        }
        free(cliente);
    }

    if (!encontrado) {
        printf("\nCLIENTE N�O ENCONTRADO.\n");
    }

    clock_t fim = clock();
    double tempoTotal = (double)(fim - inicio) / CLOCKS_PER_SEC;

    printf("-----------------------------------------\n");
    printf("Tempo de busca: %f segundos\n", tempoTotal);
    printf("N�mero de compara��es: %d\n", comp);
    printf("-----------------------------------------\n");

    FILE *log_file = fopen("log_buscas_cliente.txt", "a");
    if (log_file != NULL) {
        fprintf(log_file, "Tipo de Busca: Bin�ria; ID Procurado: %d; Encontrado: %s; Compara��es: %d; Tempo: %f segundos\n",
                n, encontrado ? "Sim" : "Nao", comp, tempoTotal);
        fclose(log_file);
    }

    return cliente_encontrado;
}


TCliente *cliente_cad(int id, const char *nome, const char *cpf, const char *endereco, const char *email, const char *telefone, double multas, int codl) {
    TCliente *cliente = (TCliente *)malloc(sizeof(TCliente));
    if (cliente == NULL) {
        printf("Erro ao alocar mem�ria para cliente\n");
        return NULL;
    }

    memset(cliente, 0, sizeof(TCliente));

    cliente->id = id;
    strncpy(cliente->nome, nome, 49);
    cliente->nome[49] = '\0';
    strncpy(cliente->cpf, cpf, 14);
    cliente->cpf[14] = '\0';
    strncpy(cliente->endereco, endereco, 99);
    cliente->endereco[99] = '\0';
    strncpy(cliente->email, email, 49);
    cliente->email[49] = '\0';
    strncpy(cliente->telefone, telefone, 19);
    cliente->telefone[19] = '\0';
    cliente->multas = multas;
    cliente->codl = codl;

    return cliente;
}

void cria_clientes_desordenado(FILE *out) {
    if (out == NULL) {
        printf("Erro: arquivo de sa�da inv�lido\n");
        return;
    }

    if(freopen("cliente.dat", "w+b", out) == NULL){
        perror("Erro ao limpar o arquivo de clientes");
        return;
    }

    rewind(out);

    const int tam = 10;
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

    printf("Criando base de dados de clientes desordenada...\n");
    for (int i = 0; i < tam; i++) {
        char nome_temp[50];
        sprintf(nome_temp, "Cliente %d", vet[i]);
        TCliente *c = cliente_cad(vet[i], nome_temp, "109.876.543-21",
                                "Rua Exemplo", "cliente@exemplo.com",
                                "31999999999", 0.0, 0);
        if (c != NULL) {
            salva_cliente(c, out);
            free(c);
        }
    }
    printf("Base de dados criada com sucesso.\n");
}
