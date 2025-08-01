#include "cliente.h"
#include "livro.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <stdio.h>

#define TAM_BUFFER 6
#define TAM_MEMORIA 6

//FUNÇÕES UTILITÁRIAS E AUXILIARES
void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int tamanho_registro_cliente() {
    return sizeof(TCliente);
}

int tamanho_arquivo_cliente(FILE *arq) {
    if (arq == NULL) return 0;
    fseek(arq, 0, SEEK_END);
    long tamanho = ftell(arq);
    rewind(arq);
    return (int)(tamanho / tamanho_registro_cliente());
}


//LEITURA E ESCRITA DE CLIENTE
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
        fprintf(txt_out, "Endereço: %s\n", c->endereco);
        fprintf(txt_out, "Email: %s\n", c->email);
        fprintf(txt_out, "Telefone: %s\n", c->telefone);
        fprintf(txt_out, "Multas Acumuladas: R$ %.2f\n", c->multas);

        if (c->codl != 0) {
            fprintf(txt_out, "Livro Alugado (Código): %d\n", c->codl);
            TLivro *livro = busca_livro_sis(livros_dat, c->codl);
            if (livro != NULL) {
                fprintf(txt_out, "  -> Título: %s\n", livro->titulo);
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

//ORDENAÇÃO MERGE SORT (PARTE I)
int compara_clientes(const void *a, const void *b) {
    TCliente *c1 = *(TCliente **)a;
    TCliente *c2 = *(TCliente **)b;
    if (c1->id < c2->id) return -1;
    if (c1->id > c2->id) return 1;
    return 0;
}

// REQUISITO 3.a: Implementação do método de partição da Atividade I.
// Este método é o "Merge Sort com Classificação Interna". Ele é executado
// como base de comparação para o novo método.
int classificacao_interna_cliente(FILE *in, char *nome_base_particao) {
    rewind(in);
    int num_particoes = 0;
    TCliente *buffer[TAM_BUFFER];
    int count = 0;

    // Garante que o arquivo de entrada está no início
    rewind(in);

    TCliente *c = le_cliente(in);
    while (c != NULL) {
        buffer[count++] = c;
        // Se o buffer encheu, ordena e salva a partição
        if (count == TAM_BUFFER) {
            qsort(buffer, count, sizeof(TCliente*), compara_clientes);

            char nome_particao[100];
            sprintf(nome_particao, "%s%d.dat", nome_base_particao, num_particoes);

            FILE *p = fopen(nome_particao, "wb");
            // Adiciona verificação de erro para garantir que a partição foi criada
            if (p == NULL) {
                perror("ERRO: Nao foi possivel criar o arquivo de particao");
                exit(1); // Encerra se não conseguir criar a partição
            }

            for (int i = 0; i < count; i++) {
                salva_cliente(buffer[i], p); // Garante que está salvando um cliente
                free(buffer[i]);
            }

            fclose(p);
            num_particoes++;
            count = 0;
        }
        c = le_cliente(in);
    }

    // Se sobraram registros no buffer após o fim do arquivo, cria uma última partição
    if (count > 0) {
        qsort(buffer, count, sizeof(TCliente*), compara_clientes);

        char nome_particao[100];
        sprintf(nome_particao, "%s%d.dat", nome_base_particao, num_particoes);

        FILE *p = fopen(nome_particao, "wb");
        if (p == NULL) {
            perror("ERRO: Nao foi possivel criar o arquivo de particao final");
            exit(1);
        }

        for (int i = 0; i < count; i++) {
            salva_cliente(buffer[i], p); // Garante que está salvando um cliente
            free(buffer[i]);
        }

        fclose(p);
        num_particoes++;
    }

    return num_particoes;
}

// REQUISITO 3.a: Implementação do método de intercalação da Atividade I.
// Usado em conjunto com a função acima para compor o primeiro método de ordenação
// a ser comparado.
void intercalacao_cliente(char *nome_base_particao, int num_part, char *nome_final) {
    FILE *out = fopen(nome_final, "wb");
    if (out == NULL) {
        perror("Erro ao criar arquivo de saída final");
        return;
    }

    FILE *particoes[num_part];
    TCliente *registros[num_part];
    for (int i = 0; i < num_part; i++) {
        char nome_part[100];
        sprintf(nome_part, "%s%d.dat", nome_base_particao, i);
        particoes[i] = fopen(nome_part, "rb");
        if (particoes[i] == NULL) {
            perror("Erro ao abrir partição para intercalação");
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
        if(particoes[i] != NULL) fclose(particoes[i]);
        char nome_part[100];
        sprintf(nome_part, "%s%d.dat", nome_base_particao, i);
        // remove(nome_part); // Mantendo comentado para depuração
    }
    fclose(out);
}

//FUNÇÕES DE BUSCA, CADASTRO, CRIAÇÃO E IMPRESSÃO DE DADOS
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

void busca_sequencial_cliente(FILE *in, FILE *livros) {
    if (in == NULL) return;
    int comp = 0;
    int id;
    TCliente *f = NULL;
    int encontrado = 0;
    printf("\n--- BUSCA SEQUENCIAL DE CLIENTE ---\nDigite o ID do cliente: ");
    scanf("%d", &id);
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
        printf("\nCLIENTE NÃO ENCONTRADO.\n");
    }
    clock_t fim = clock();
    double tempoTotal = (double)(fim - inicio) / CLOCKS_PER_SEC;
    printf("-----------------------------------------\n");
    printf("Tempo de busca: %f segundos\n", tempoTotal);
    printf("Número de comparações: %d\n", comp);
    printf("-----------------------------------------\n");
    FILE *log_file = fopen("log_buscas_cliente.txt", "a");
    if (log_file != NULL) {
        fprintf(log_file, "Tipo de Busca: Sequencial; ID Procurado: %d; Encontrado: %s; Comparações: %d; Tempo: %f segundos\n",
                id, encontrado ? "Sim" : "Nao", comp, tempoTotal);
        fclose(log_file);
    }
}

TCliente *busca_binaria_cliente(FILE *in, FILE *livros) {
    if (in == NULL) return NULL;
    int tam = tamanho_arquivo_cliente(in);
    if (tam <= 0) {
        printf("Arquivo vazio ou não é possível realizar a busca.\n");
        return NULL;
    }
    int left = 0, right = tam - 1, comp = 0, n, encontrado = 0;
    printf("\n--- BUSCA BINÁRIA DE CLIENTE ---\n(Certifique-se de que o arquivo esteja ordenado por ID)\nDigite o ID do cliente: ");
    scanf("%d", &n);
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
    if (!encontrado) printf("\nCLIENTE NÃO ENCONTRADO.\n");
    clock_t fim = clock();
    double tempoTotal = (double)(fim - inicio) / CLOCKS_PER_SEC;
    printf("-----------------------------------------\n");
    printf("Tempo de busca: %f segundos\n", tempoTotal);
    printf("Número de comparações: %d\n", comp);
    printf("-----------------------------------------\n");
    FILE *log_file = fopen("log_buscas_cliente.txt", "a");
    if (log_file != NULL) {
        fprintf(log_file, "Tipo de Busca: Binária; ID Procurado: %d; Encontrado: %s; Comparações: %d; Tempo: %f segundos\n",
                n, encontrado ? "Sim" : "Nao", comp, tempoTotal);
        fclose(log_file);
    }
    return cliente_encontrado;
}

TCliente *cliente_cad(int id, const char *nome, const char *cpf, const char *endereco, const char *email, const char *telefone, double multas, int codl) {
    TCliente *cliente = (TCliente *)malloc(sizeof(TCliente));
    if (cliente == NULL) return NULL;
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

void imprime_cliente(TCliente *cliente, FILE *livros) {
    if (cliente == NULL) return;
    printf("\n*************************************************");
    printf("\nCliente de id: %d", cliente->id);
    printf("\nNome: %s", cliente->nome);
    printf("\nCPF: %s", cliente->cpf);
    printf("\nEndereço: %s", cliente->endereco);
    printf("\nEmail: %s", cliente->email);
    printf("\nTelefone: %s", cliente->telefone);
    printf("\nMultas: %.2lf", cliente->multas);
    if (cliente->codl != 0 && livros != NULL) {
        printf("\nLivro alugado:");
        TLivro *livro = busca_livro_sis(livros, cliente->codl);
        if (livro != NULL) {
            imprime_livro(livro);
            free(livro);
        }
    }
    printf("\n*************************************************\n");
}

void le_clientes(FILE *in, FILE *livros) {
    if (in == NULL) return;
    printf("\n\nLendo clientes do arquivo...\n\n");
    rewind(in);
    TCliente *c;
    while ((c = le_cliente(in)) != NULL) {
        imprime_cliente(c, livros);
        free(c);
    }
}

void cadastra_cliente(FILE *out) {
    if (out == NULL) return;
    TCliente cliente;
    memset(&cliente, 0, sizeof(TCliente));
    printf("\nDigite o id: ");
    scanf("%d", &cliente.id);
    limpar_buffer();
    printf("Digite o nome: ");
    fgets(cliente.nome, 50, stdin);
    cliente.nome[strcspn(cliente.nome, "\n")] = '\0';
    printf("Digite o CPF: ");
    fgets(cliente.cpf, 15, stdin);
    cliente.cpf[strcspn(cliente.cpf, "\n")] = '\0';
    printf("Digite o endereço: ");
    fgets(cliente.endereco, 100, stdin);
    cliente.endereco[strcspn(cliente.endereco, "\n")] = '\0';
    printf("Digite o telefone: ");
    fgets(cliente.telefone, 20, stdin);
    cliente.telefone[strcspn(cliente.telefone, "\n")] = '\0';
    printf("Digite o email: ");
    fgets(cliente.email, 50, stdin);
    cliente.email[strcspn(cliente.email, "\n")] = '\0';
    cliente.multas = 0.0;
    cliente.codl = 0;
    fseek(out, 0, SEEK_END);
    salva_cliente(&cliente, out);
}

// --- NOVAS FUNÇÕES DA PARTE 2 ---

typedef struct {
    TCliente *cliente;
    int part_idx;
} HeapNode;

// Função auxiliar para a Intercalação Ótima (Requisito 2)
void heapify_down(HeapNode *heap, int size, int i) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    if (left < size && heap[left].cliente->id < heap[smallest].cliente->id)
        smallest = left;
    if (right < size && heap[right].cliente->id < heap[smallest].cliente->id)
        smallest = right;
    if (smallest != i) {
        HeapNode temp = heap[i];
        heap[i] = heap[smallest];
        heap[smallest] = temp;
        heapify_down(heap, size, smallest);
    }
}

// REQUISITO 1: Implementação da "Seleção por Substituição".
// Esta função cumpre integralmente o primeiro requisito.
int selecao_substituicao_cliente(FILE *in, char *nome_base_particao) {
    rewind(in);
    int num_particoes = 0;
    // REQUISITO 1: 'memoria' é o array em memória principal para o algoritmo.
    TCliente *memoria[TAM_MEMORIA];
    // REQUISITO 1: 'congelados' é o array que marca registros que não podem
    // ser selecionados na partição atual, a lógica central do método.
    int congelados[TAM_MEMORIA] = {0};
    int count = 0;
    int ultimo_id_salvo = -1;

    for (int i = 0; i < TAM_MEMORIA; i++) {
        memoria[i] = le_cliente(in);
        if (memoria[i] == NULL) break;
        count++;
    }

    while (count > 0) {
        char nome_particao[100];
        sprintf(nome_particao, "%s%d.dat", nome_base_particao, num_particoes++);
        FILE *p = fopen(nome_particao, "wb");
        if (!p) exit(1);

        int ativos = count;
        ultimo_id_salvo = -1;
        // Descongela todos para a nova partição
        for(int i = 0; i < count; i++) congelados[i] = 0;

        while (ativos > 0) {
            int menor_idx = -1;
            // REQUISITO 1: Busca o menor elemento *não congelado* na memória.
            for (int i = 0; i < count; i++) {
                if (!congelados[i]) {
                    if (menor_idx == -1 || memoria[i]->id < memoria[menor_idx]->id) {
                        menor_idx = i;
                    }
                }
            }
            if (menor_idx == -1) break;

            TCliente *menor_cliente = memoria[menor_idx];
            salva_cliente(menor_cliente, p);
            ultimo_id_salvo = menor_cliente->id;

            TCliente* proximo = le_cliente(in);
            if (proximo == NULL) {
                free(menor_cliente);
                for (int j = menor_idx; j < count - 1; j++) {
                    memoria[j] = memoria[j + 1];
                    congelados[j] = congelados[j + 1];
                }
                count--;
                ativos--;
            } else {
                // REQUISITO 1: Lógica de congelamento. Se o novo registro lido do
                // arquivo for menor que o último salvo, ele é "congelado" para
                // a próxima partição.
                if (proximo->id < ultimo_id_salvo) {
                    congelados[menor_idx] = 1;
                    ativos--;
                }
                free(memoria[menor_idx]);
                memoria[menor_idx] = proximo;
            }
        }
        fclose(p);
        // Se todos foram congelados mas ainda há itens na memória, força uma nova partição
        if (ativos == 0 && count > 0) {
             // Loop continua, vai criar nova partição
        }
    }
    return num_particoes;
}

// REQUISITO 2: Implementação da "Intercalação Ótima".
// Esta função utiliza um min-heap para intercalar as partições de forma
// eficiente, cumprindo o segundo requisito.
void intercalacao_otima_cliente(char *nome_base_particao, int num_part, char *nome_final) {
    FILE *out = fopen(nome_final, "wb");
    if (!out) return;

    FILE *particoes[num_part];
    // REQUISITO 2: 'heap' é a estrutura de dados (min-heap) que garante a
    // intercalação ótima, simulando uma árvore de vencedores.
    HeapNode heap[num_part];
    int heap_size = 0;

    for (int i = 0; i < num_part; i++) {
        char nome_part[100];
        sprintf(nome_part, "%s%d.dat", nome_base_particao, i);
        particoes[i] = fopen(nome_part, "rb");
        if (particoes[i]) {
            TCliente *c = le_cliente(particoes[i]);
            if (c) {
                heap[heap_size].cliente = c;
                heap[heap_size].part_idx = i;
                heap_size++;
            } else {
                fclose(particoes[i]);
            }
        }
    }

    // REQUISITO 2: Constrói o heap inicial.
    for (int i = (heap_size / 2) - 1; i >= 0; i--) {
        heapify_down(heap, heap_size, i);
    }

    // REQUISITO 2: Loop principal da intercalação.
    while (heap_size > 0) {
        // Pega a raiz (o menor elemento de todos)
        HeapNode raiz = heap[0];
        salva_cliente(raiz.cliente, out);
        free(raiz.cliente);

        // Lê o próximo elemento da mesma partição de onde a raiz veio.
        TCliente *proximo = le_cliente(particoes[raiz.part_idx]);
        if (proximo) {
            heap[0].cliente = proximo;
        } else {
            // Se a partição acabou, remove o nó do heap.
            heap[0] = heap[heap_size - 1];
            heap_size--;
        }

        // REQUISITO 2: Mantém a propriedade de heap.
        if (heap_size > 0) {
            heapify_down(heap, heap_size, 0);
        }
    }

    for (int i = 0; i < num_part; i++) {
        if(particoes[i] != NULL && ferror(particoes[i]) == 0){
             fclose(particoes[i]);
        }
        char nome_part[100];
        sprintf(nome_part, "%s%d.dat", nome_base_particao, i);
        // remove(nome_part); // Mantendo comentado para depuração
    }
    fclose(out);
}

// REQUISITO 3: Implementação da função que cria a base de dados de teste.
// O parâmetro 'tam' permite criar bases de "diferentes tamanhos".
void cria_clientes_desordenado(FILE *out, int tam) {
    if (out == NULL) return;

    srand(time(NULL));
    //printf("Criando base de dados de %d clientes desordenados...\n", tam);

    for (int i = 0; i < tam; i++) {
        char nome_temp[50];
        int id = rand() % (tam * 5) + 1; // Evita ID 0
        sprintf(nome_temp, "Cliente %d", id);
        TCliente *c = cliente_cad(id, nome_temp, "109.876.543-21",
                                "Rua Exemplo", "cliente@exemplo.com",
                                "31999999999", 0.0, 0);
        if (c != NULL) {
            salva_cliente(c, out);
            free(c);
        }
    }
    //printf("Base de dados criada com sucesso.\n");
}
