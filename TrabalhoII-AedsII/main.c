#include "cliente.h"
#include "funcionario.h"
#include "livro.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

// >>> MUDANÇA AQUI: Inclusão de cabeçalhos para criar diretórios <<<
#if defined(_WIN32)
#include <direct.h> // Para _mkdir no Windows
#else
#include <sys/stat.h> // Para mkdir no Linux/Unix
#endif


// REQUISITO 3.c: Estrutura para armazenar os "logs" dos testes:
// tempo de execução e número de partições.
typedef struct {
    char method_name[100];
    double time_taken;
    int partitions_generated;
    int valid; // 0 se não houver dados, 1 se os dados forem válidos
} SortStats;

// REQUISITO 3.c: Variáveis globais para armazenar os resultados dos dois métodos.
SortStats stats_merge = {"Merge Sort (Classificação Interna)", 0.0, 0, 0};
SortStats stats_selection = {"Seleção por Substituição", 0.0, 0, 0};
int g_test_database_exists = 0; // Flag para controlar se a base de teste foi criada
const char* g_test_filename = "cliente_teste_para_ordenacao.dat";

// >>> MUDANÇA AQUI: Definição dos nomes dos diretórios de partição <<<
const char* dir_merge = "partitions_merge";
const char* dir_selection = "partitions_selection";

// >>> MUDANÇA AQUI: Nova função para criar diretórios de forma portável <<<
void criar_diretorio_se_nao_existir(const char* nome_dir) {
    #if defined(_WIN32)
        _mkdir(nome_dir);
    #else
        mkdir(nome_dir, 0777); // 0777 são as permissões de leitura/escrita/execução
    #endif
    // Ignoramos erros caso o diretório já exista
}


//gerencia a operação de aluguel de um livro por um cliente.
void alugar_livro(FILE *livro, FILE *cliente) {
  int codc; // código (ID) do cliente
  int codl; // código do livro
  long pos_livro, pos_cliente; // posições dos registros nos arquivos

  printf("DIGITE O ID DO CLIENTE: ");
  scanf("%d", &codc);
  printf("DIGITE O CÓDIGO DO LIVRO: ");
  scanf("%d", &codl);

  TCliente *c = busca_cliente_sis(cliente, codc);
  TLivro *l = busca_livro_sis(livro, codl);

  if (c == NULL) {
    printf("\nERRO: Cliente com ID %d não encontrado.\n", codc);
    if(l) free(l);
    return;
  }
  if (l == NULL) {
    printf("\nERRO: Livro com código %d não encontrado.\n", codl);
    if(c) free(c);
    return;
  }

  if (l->qtd <= 0) {
    printf("\nTODOS OS EXEMPLARES DESTE LIVRO ESTÃO ALUGADOS!\n");
  } else if (c->multas > 100) {
    printf("\nO CLIENTE POSSUI MULTAS PENDENTES EM EXCESSO!\n");
  } else if (c->codl != 0) {
    printf("\nO CLIENTE JÁ POSSUI UM LIVRO ALUGADO (Cód: %d).\nDEVOLVA O LIVRO PRIMEIRO!\n", c->codl);
  } else {
    c->codl = codl;
    l->qtd--;

    pos_livro = busca_posicao_livro_sis(livro, codl);
    pos_cliente = busca_posicao_cliente_sis(cliente, codc);

    fseek(livro, pos_livro * tamanho_registro_livro(), SEEK_SET);
    salva_livro(l, livro);

    fseek(cliente, pos_cliente * tamanho_registro_cliente(), SEEK_SET);
    salva_cliente(c, cliente);

    printf("\nLIVRO ALUGADO COM SUCESSO!\n");
    imprime_cliente(c, livro);
  }

  free(c);
  free(l);
}

//gerencia a operação de devolução de um livro por um cliente.
void devolver_livro(FILE *livro, FILE *cliente) {
  int codc;
  printf("DIGITE O ID DO CLIENTE: ");
  scanf("%d", &codc);

  TCliente *c = busca_cliente_sis(cliente, codc);

  if (c == NULL) {
      printf("\nERRO: Cliente com ID %d não encontrado.\n", codc);
      return;
  }
  if (c->codl == 0) {
    printf("\nESTE CLIENTE NÃO POSSUI LIVROS ALUGADOS!\n");
    free(c);
    return;
  }

  TLivro *l = busca_livro_sis(livro, c->codl);
  if (l == NULL) {
      printf("\nERRO: O livro alugado (cód %d) não foi encontrado na base de dados.\n", c->codl);
      free(c);
      return;
  }

  if (c->multas > 0) {
    printf("\nCLIENTE COM MULTAS, LEMBRE-SE DE COBRÁ-LAS!\n");
    c->multas = 0;
  }

  l->qtd++;
  long pos_livro = busca_posicao_livro_sis(livro, c->codl);
  fseek(livro, pos_livro * tamanho_registro_livro(), SEEK_SET);
  salva_livro(l, livro);
  free(l);

  c->codl = 0;
  long pos_cliente = busca_posicao_cliente_sis(cliente, codc);
  fseek(cliente, pos_cliente * tamanho_registro_cliente(), SEEK_SET);
  salva_cliente(c, cliente);
  free(c);

  printf("\nLIVRO DEVOLVIDO COM SUCESSO!\n");
}

//aplica uma multa a um cliente com base no livro que ele alugou.
void multar(FILE *livro, FILE *cliente) {
  int codc;
  printf("DIGITE O ID DO CLIENTE A SER MULTADO: ");
  scanf("%d", &codc);

  TCliente *c = busca_cliente_sis(cliente, codc);

  if (c == NULL) {
      printf("\nERRO: Cliente com ID %d não encontrado.\n", codc);
      return;
  }
  if (c->codl == 0) {
    printf("\nCLIENTE NÃO TEM LIVROS ALUGADOS PARA GERAR MULTA!\n");
    free(c);
    return;
  }

  TLivro *l = busca_livro_sis(livro, c->codl);
   if (l == NULL) {
      printf("\nERRO: O livro alugado (cód %d) não foi encontrado na base de dados.\n", c->codl);
      free(c);
      return;
  }

  c->multas += l->preco * 0.05;

  long pos_cliente = busca_posicao_cliente_sis(cliente, codc);
  fseek(cliente, pos_cliente * tamanho_registro_cliente(), SEEK_SET);
  salva_cliente(c, cliente);

  printf("\nMULTA DE R$%.2f APLICADA COM SUCESSO!\n", l->preco * 0.05);
  printf("Total de multas do cliente: R$%.2f\n", c->multas);

  free(c);
  free(l);
}

void menu() {
    printf("\n=============== MENU DE OPÇÕES ===============\n");
    printf("1.  Cadastrar Funcionário\n");
    printf("2.  Cadastrar Cliente\n");
    printf("3.  Cadastrar Livro\n");
    printf("---------------------------------------------\n");
    printf("4.  Imprimir Todos os Funcionários\n");
    printf("5.  Imprimir Todos os Clientes\n");
    printf("6.  Imprimir Todos os Livros\n");
    printf("---------------------------------------------\n");
    printf("7.  Busca Sequencial\n");
    printf("8.  Busca Binária (requer base ordenada)\n");
    printf("---------------------------------------------\n");
    printf("9.  Alugar Livro\n");
    printf("10. Devolver Livro\n");
    printf("11. Aplicar Multa\n");
    printf("---------------------------------------------\n");
    printf("12. Recriar Base de Funcionários (Desordenada)\n");
    printf("13. Recriar Base de Livros (Desordenada)\n");
    printf("14. Recriar Base de Clientes (Desordenada)\n");
    printf("15. Ordenar Arquivos (Merge Sort - Classificação Interna)\n");
    printf("16. Ordenar Clientes (Seleção por Substituição + Intercalação Ótima)\n");
    printf("17. Comparar Métodos de Ordenação (Clientes)\n");
    printf("---------------------------------------------\n");
    printf("18. Limpar Terminal\n");
    printf("0.  Sair\n");
    printf("=============================================\n");
}

// REQUISITO 3: Esta função permite ao usuário criar uma base de dados de teste
// com um tamanho específico, cumprindo a parte de "bases de diferentes tamanhos".
void criar_base_teste_clientes() {
    FILE *client_file;
    int tam_base;

    printf("\n--- CRIAR BASE DE CLIENTES PARA TESTE ---\n");
    printf("Digite a quantidade de clientes desordenados a serem gerados: ");
    scanf("%d", &tam_base);
    if (tam_base <= 0) {
        printf("Tamanho inválido.\n");
        return;
    }

    client_file = fopen(g_test_filename, "w+b");
    if (!client_file) {
        perror("Nao foi possivel criar o arquivo de teste.");
        return;
    }

    cria_clientes_desordenado(client_file, tam_base);
    fclose(client_file);

    g_test_database_exists = 1;
    // Reseta os resultados anteriores, pois uma nova base foi criada
    stats_merge.valid = 0;
    stats_selection.valid = 0;

    printf("\nBase de dados '%s' com %d clientes foi criada com sucesso.\n", g_test_filename, tam_base);
    printf("Agora você pode usar as opções 15 e 16 para testar os algoritmos de ordenação.\n");
}


// REQUISITO 3.a: Esta função executa o método de ordenação da Atividade I.
void executar_ordenacao_merge() {
    if (!g_test_database_exists) {
        printf("\nERRO: Você precisa primeiro criar uma base de clientes desordenada usando a opção 14.\n");
        return;
    }

    printf("\n--- EXECUTANDO: Merge Sort (Classificação Interna) ---\n");
    FILE* client_file = fopen(g_test_filename, "r+b");
    if (!client_file) {
        perror("Nao foi possivel abrir o arquivo de teste.");
        return;
    }

    char base_path[100];
    sprintf(base_path, "%s/part_cli_merge_", dir_merge);

    // REQUISITO 3.c: Inicia a medição de tempo (log de tempo).
    clock_t inicio = clock();
    int num_p = classificacao_interna_cliente(client_file, base_path);
    intercalacao_cliente(base_path, num_p, "cli_ordenado_merge.dat");
    clock_t fim = clock();

    // REQUISITO 3.c: Finaliza e armazena os dados de desempenho (tempo e partições).
    stats_merge.time_taken = (double)(fim - inicio) / CLOCKS_PER_SEC;
    stats_merge.partitions_generated = num_p;
    stats_merge.valid = 1;

    printf("-> Teste concluído! As partições foram salvas em '%s/'\n", dir_merge);
    printf("   Resultados armazenados para comparação.\n");

    fclose(client_file);
    remove("cli_ordenado_merge.dat");
}

// REQUISITO 3.b: Esta função executa o novo método de ordenação (Req. 1 + Req. 2).
void executar_ordenacao_selecao() {
    if (!g_test_database_exists) {
        printf("\nERRO: Você precisa primeiro criar uma base de clientes desordenada usando a opção 14.\n");
        return;
    }

    printf("\n--- EXECUTANDO: Seleção por Substituição + Intercalação Ótima ---\n");
    FILE* client_file = fopen(g_test_filename, "r+b");
    if (!client_file) {
        perror("Nao foi possivel abrir o arquivo de teste.");
        return;
    }

    char base_path[100];
    sprintf(base_path, "%s/part_cli_subst_", dir_selection);

    // REQUISITO 3.c: Inicia a medição de tempo (log de tempo).
    clock_t inicio = clock();
    int num_p = selecao_substituicao_cliente(client_file, base_path);
    intercalacao_otima_cliente(base_path, num_p, "cli_ordenado_subst.dat");
    clock_t fim = clock();

    // REQUISITO 3.c: Finaliza e armazena os dados de desempenho (tempo e partições).
    stats_selection.time_taken = (double)(fim - inicio) / CLOCKS_PER_SEC;
    stats_selection.partitions_generated = num_p;
    stats_selection.valid = 1;

    printf("-> Teste concluído! As partições foram salvas em '%s/'\n", dir_selection);
    printf("   Resultados armazenados para comparação.\n");

    fclose(client_file);
    remove("cli_ordenado_subst.dat");
}

// REQUISITO 3.c: Esta função gera o "breve relatório" comparativo.
void exibir_comparacao_simplificada() {
    printf("\n--- RELATÓRIO DE COMPARAÇÃO DE ORDENAÇÃO ---\n");

    if (!stats_merge.valid || !stats_selection.valid) {
        printf("\nERRO: É necessário executar ambos os testes de ordenação (opções 15 e 16) para comparar.\n");
        if (!stats_merge.valid) printf("- Teste 'Merge Sort' pendente.\n");
        if (!stats_selection.valid) printf("- Teste 'Seleção por Substituição' pendente.\n");
        return;
    }

    // REQUISITO 3.c: Apresenta os dados coletados (logs) para o método 1.
    printf("\n> Método 1: %s\n", stats_merge.method_name);
    printf("  - Tempo gasto: %.6f segundos\n", stats_merge.time_taken);
    printf("  - Partições geradas: %d\n", stats_merge.partitions_generated);

    // REQUISITO 3.c: Apresenta os dados coletados (logs) para o método 2.
    printf("\n> Método 2: %s\n", stats_selection.method_name);
    printf("  - Tempo gasto: %.6f segundos\n", stats_selection.time_taken);
    printf("  - Partições geradas: %d\n", stats_selection.partitions_generated);

    // REQUISITO 3.c: Apresenta a análise/opinião final, completando o relatório.
    printf("\n---------------------------------------------\n");
    printf("Análise: ");
    if (stats_selection.time_taken < stats_merge.time_taken) {
        printf("O método de 'Seleção por Substituição' foi mais eficiente em tempo de execução.\n");
    } else if (stats_merge.time_taken < stats_selection.time_taken) {
        printf("O método de 'Merge Sort (Classificação Interna)' foi mais eficiente em tempo de execução.\n");
    } else {
        printf("Ambos os métodos tiveram tempos de execução praticamente idênticos.\n");
    }
}


int main(int argc, char **argv) {
  setlocale(LC_ALL, "Portuguese");
  int aux = -1, aux2 = -1;
  FILE *func, *livr, *client;

  printf("Criando diretórios para partições (se não existirem)...\n");
  criar_diretorio_se_nao_existir(dir_merge);
  criar_diretorio_se_nao_existir(dir_selection);


  if ((func = fopen("funcionario.dat", "r+b")) == NULL) {
      func = fopen("funcionario.dat", "w+b");
  }
  if ((livr = fopen("livro.dat", "r+b")) == NULL) {
      livr = fopen("livro.dat", "w+b");
  }
  if ((client = fopen("cliente.dat", "r+b")) == NULL) {
      client = fopen("cliente.dat", "w+b");
  }

  if (func == NULL || livr == NULL || client == NULL) {
      printf("Erro fatal: Não foi possível abrir ou criar os arquivos de dados.\n");
      exit(1);
  }

  printf("Sincronizando arquivos de visualização .txt...\n");
  atualiza_funcionario_txt(func);
  atualiza_livro_txt(livr);
  atualiza_cliente_txt(client, livr);

  while (aux != 0) {
    menu();
    printf("\nDigite a opção desejada: ");
    scanf("%d", &aux);

    switch (aux) {
      case 1:
        cria_funcionario(func);
        atualiza_funcionario_txt(func);
        break;
      case 2:
        cadastra_cliente(client);
        atualiza_cliente_txt(client, livr);
        break;
      case 3:
        cadastra_livro(livr);
        atualiza_livro_txt(livr);
        break;
      case 4: le_funcionarios(func); break;
      case 5: le_clientes(client, livr); break;
      case 6: le_livros(livr); break;
      case 7:
        printf("\n--- Submenu Busca Sequencial ---\n1. Funcionário\n2. Livro\n3. Cliente\nEscolha: ");
        scanf("%d", &aux2);
        if (aux2 == 1) busca_sequencial_funcionario(func);
        else if (aux2 == 2) busca_sequencial_livro(livr);
        else if (aux2 == 3) busca_sequencial_cliente(client, livr);
        break;
      case 8:
        printf("\n--- Submenu Busca Binária ---\n1. Funcionário\n2. Livro\n3. Cliente\nEscolha: ");
        scanf("%d", &aux2);
        if (aux2 == 1) busca_binaria_funcionario(func);
        else if (aux2 == 2) busca_binaria_livro(livr);
        else if (aux2 == 3) busca_binaria_cliente(client, livr);
        break;
      case 9:
        alugar_livro(livr, client);
        atualiza_livro_txt(livr);
        atualiza_cliente_txt(client, livr);
        break;
      case 10:
        devolver_livro(livr, client);
        atualiza_livro_txt(livr);
        atualiza_cliente_txt(client, livr);
        break;
      case 11:
        multar(livr, client);
        atualiza_cliente_txt(client, livr);
        break;
      case 12:
        cria_funcionarios_desordenado(func);
        atualiza_funcionario_txt(func);
        break;
      case 13:
        cria_livros_desordenado(livr);
        atualiza_livro_txt(livr);
        break;
      // REQUISITO 3: O usuário escolhe a opção 14 para criar a base de teste.
      case 14:
        criar_base_teste_clientes();
        break;
      // REQUISITO 3.a: O usuário escolhe a opção 15 para rodar o método da Atividade I.
      case 15:
        executar_ordenacao_merge();
        break;
      // REQUISITO 3.b: O usuário escolhe a opção 16 para rodar o novo método.
      case 16:
        executar_ordenacao_selecao();
        break;
      // REQUISITO 3.c: O usuário escolhe a opção 17 para ver o relatório.
      case 17:
        exibir_comparacao_simplificada();
        break;
      case 18:
        system("clear || cls");
        break;
      case 0:
        printf("Saindo do programa...\n");
        break;
      default:
        printf("Opção inválida!\n");
        break;
    }
  }

  fclose(func);
  fclose(livr);
  fclose(client);
  // Remove o arquivo de teste ao sair para não deixar lixo
  if(g_test_database_exists) {
      remove(g_test_filename);
  }
  return 0;
}
