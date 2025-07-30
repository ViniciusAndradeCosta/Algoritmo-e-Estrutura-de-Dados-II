#include "funcionario.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#define TAM_BUFFER 6

//FUNÇÕES UTILITÁRIAS E AUXILIARES
void limpar_buffer_func() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int tamanho_registro_func() {
    return sizeof(TFunc);
}

int tamanho_arquivo_func(FILE *arq) {
    if (arq == NULL) return 0;
    fseek(arq, 0, SEEK_END);
    long tam = ftell(arq);
    return (int)(tam / tamanho_registro_func());
}

//LEITURA E ESCRITA DE FUNCIONÁRIO
void salva_funcionario(TFunc *func, FILE *out) {
    fwrite(func, sizeof(TFunc), 1, out);
}

TFunc *le_funcionario(FILE *in) {
    TFunc *func = (TFunc *)malloc(sizeof(TFunc));
    if (fread(func, sizeof(TFunc), 1, in) != 1) {
        free(func);
        return NULL;
    }
    return func;
}

void atualiza_funcionario_txt(FILE *dat_in) {
    FILE *txt_out = fopen("funcionario.txt", "w");
    if (txt_out == NULL) {
        perror("Erro ao abrir funcionario.txt para escrita");
        return;
    }

    rewind(dat_in);
    TFunc *f;
    fprintf(txt_out, "========= LISTA DE FUNCIONÁRIOS =========\n\n");
    while ((f = le_funcionario(dat_in)) != NULL) {
        fprintf(txt_out, "----------------------------------------\n");
        fprintf(txt_out, "Código: %d\n", f->cod);
        fprintf(txt_out, "Nome: %s\n", f->nome);
        fprintf(txt_out, "CPF: %s\n", f->cpf);
        fprintf(txt_out, "Data de Nascimento: %s\n", f->data_nascimento);
        fprintf(txt_out, "Salário: R$ %.2f\n", f->salario);
        fprintf(txt_out, "Endereço: %s\n", f->endereco);
        fprintf(txt_out, "Email: %s\n", f->email);
        fprintf(txt_out, "Telefone: %s\n", f->telefone);
        fprintf(txt_out, "----------------------------------------\n\n");
        free(f);
    }
    fclose(txt_out);
    }


//ORDENAÇÃO MERGE SORT
int compara_funcionarios(const void *a, const void *b) {
    TFunc *f1 = *(TFunc **)a;
    TFunc *f2 = *(TFunc **)b;
    if (f1->cod < f2->cod) return -1;
    if (f1->cod > f2->cod) return 1;
    return 0;
}

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
            if (p == NULL) {
                perror("Erro ao criar partição");
                exit(1);
            }
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

void intercalacao_funcionario(char *nome_base_particao, int num_part, char *nome_final) {
    FILE *out = fopen(nome_final, "wb");
    if (out == NULL) {
        perror("Erro ao criar arquivo de saída final");
        return;
    }

    FILE *particoes[num_part];
    TFunc *registros[num_part];
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
        if (particoes[i] != NULL) fclose(particoes[i]);
        char nome_part[100];
        sprintf(nome_part, "%s%d.dat", nome_base_particao, i);
        remove(nome_part);
    }
    fclose(out);
}

//FUNÇÕES DE BUSCA
void busca_sequencial_funcionario(FILE *in) {
    int comp = 0;
    int cod;
    TFunc *f = NULL;
    int encontrado = 0;

    printf("\n--- BUSCA SEQUENCIAL DE FUNCIONÁRIO ---\n");
    printf("Digite o código do funcionário: ");
    scanf("%d", &cod);
    limpar_buffer_func();

    rewind(in);
    clock_t inicio = clock();
    while ((f = le_funcionario(in)) != NULL) {
        comp++;
        if (cod == f->cod) {
            printf("\nFUNCIONÁRIO ENCONTRADO!\n");
            imprime_funcionario(f);
            encontrado = 1;
            free(f);
            break;
        }
        free(f);
    }

    if (!encontrado) {
        printf("\nFUNCIONÁRIO NÃO ENCONTRADO.\n");
    }

    clock_t fim = clock();
    double tempoTotal = (double)(fim - inicio) / CLOCKS_PER_SEC;

    printf("-----------------------------------------\n");
    printf("Tempo de busca: %f segundos\n", tempoTotal);
    printf("Número de comparações: %d\n", comp);
    printf("-----------------------------------------\n");
    FILE *log_file = fopen("log_buscas_funcionario.txt", "a");
    if (log_file != NULL) {
        fprintf(log_file, "Tipo de Busca: Sequencial; Código Procurado: %d; Encontrado: %s; Comparações: %d; Tempo: %f segundos\n",
                cod, encontrado ? "Sim" : "Nao", comp, tempoTotal);
        fclose(log_file);
    }
}

TFunc *busca_binaria_funcionario(FILE *in) {
    int tam = tamanho_arquivo_func(in);
    if (tam <= 0) {
        printf("Arquivo vazio ou não é possível realizar a busca.\n");
        return NULL;
    }

    int n;
    int left = 0;
    int right = tam - 1;
    int comp = 0;
    int encontrado = 0;
    TFunc *func_encontrado = NULL;

    printf("\n--- BUSCA BINÁRIA DE FUNCIONÁRIO ---\n");
    printf("(Certifique-se de que o arquivo esteja ordenado por código)\n");
    printf("Digite o código do funcionário: ");
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
            printf("\nFUNCIONÁRIO ENCONTRADO!\n");
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
        printf("\nFUNCIONÁrio NÃO ENCONTRADO.\n");
    }

    clock_t fim = clock();
    double tempoTotal = (double)(fim - inicio) / CLOCKS_PER_SEC;

    printf("-----------------------------------------\n");
    printf("Tempo de busca: %f segundos\n", tempoTotal);
    printf("Número de comparações: %d\n", comp);
    printf("-----------------------------------------\n");

    FILE *log_file = fopen("log_buscas_funcionario.txt", "a");
    if (log_file != NULL) {
        fprintf(log_file, "Tipo de Busca: Binária; Código Procurado: %d; Encontrado: %s; Comparações: %d; Tempo: %f segundos\n",
                n, encontrado ? "Sim" : "Nao", comp, tempoTotal);
        fclose(log_file);
    }
    return func_encontrado;
}

//CADASTRO, CRIAÇÃO E IMPRESSÃO DE DADOS
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

void imprime_funcionario(TFunc *func) {
    printf("**********************************************\n");
    printf("Funcionário de código: %d\n", func->cod);
    printf("Nome: %s\n", func->nome);
    printf("CPF: %s\n", func->cpf);
    printf("Data de Nascimento: %s\n", func->data_nascimento);
    printf("Salário: %.2f\n", func->salario);
    printf("Endereço: %s\n", func->endereco);
    printf("Email: %s\n", func->email);
    printf("Telefone: %s\n", func->telefone);
    printf("**********************************************\n");
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

void cria_funcionario(FILE *out) {
    TFunc func;
    memset(&func, 0, sizeof(TFunc));

    printf("\nDigite o código: ");
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

//FUNÇÃO PARA CRIAR BASE DESORDENADA
void cria_funcionarios_desordenado(FILE *out) {
    if(freopen("funcionario.dat", "w+b", out) == NULL){
        perror("Erro ao limpar o arquivo de funcionários");
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
            a = 1 + rand() % 100;
            for (int j = 0; j < i; j++) {
                if (a == vet[j]) {
                    existe = 1;
                    break;
                }
            }
        } while (existe);
        vet[i] = a;
    }

    printf("Criando base de dados de funcionários desordenada...\n");
    for (int i = 0; i < tam; i++) {
        char nome_temp[50];
        sprintf(nome_temp, "Funcionario %d", vet[i]);
        TFunc *f = funcionario_cad(vet[i], nome_temp, "123.456.789-10",
                                   "15/06/1985", 3500.0, "Rua de tal", "31987200414",
                                   "funcionario@gmail.com");
        salva_funcionario(f, out);
        free(f);
    }
    printf("Base de dados criada com sucesso.\n");
}
