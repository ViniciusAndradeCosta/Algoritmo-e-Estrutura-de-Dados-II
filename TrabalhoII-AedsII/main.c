#include "cliente.h"
#include "funcionario.h"
#include "livro.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

// >>> MUDAN�A AQUI: Inclus�o de cabe�alhos para criar diret�rios <<<
#if defined(_WIN32)
#include <direct.h> // Para _mkdir no Windows
#else
#include <sys/stat.h> // Para mkdir no Linux/Unix
#endif


// REQUISITO 3.c: Estrutura para armazenar os "logs" dos testes:
// tempo de execu��o e n�mero de parti��es.
typedef struct {
    char method_name[100];
    double time_taken;
    int partitions_generated;
    int valid; // 0 se n�o houver dados, 1 se os dados forem v�lidos
} SortStats;

// REQUISITO 3.c: Vari�veis globais para armazenar os resultados dos dois m�todos.
SortStats stats_merge = {"Merge Sort (Classifica��o Interna)", 0.0, 0, 0};
SortStats stats_selection = {"Sele��o por Substitui��o", 0.0, 0, 0};
int g_test_database_exists = 0; // Flag para controlar se a base de teste foi criada
const char* g_test_filename = "cliente_teste_para_ordenacao.dat";

// >>> MUDAN�A AQUI: Defini��o dos nomes dos diret�rios de parti��o <<<
const char* dir_merge = "partitions_merge";
const char* dir_selection = "partitions_selection";

// >>> MUDAN�A AQUI: Nova fun��o para criar diret�rios de forma port�vel <<<
void criar_diretorio_se_nao_existir(const char* nome_dir) {
    #if defined(_WIN32)
        _mkdir(nome_dir);
    #else
        mkdir(nome_dir, 0777); // 0777 s�o as permiss�es de leitura/escrita/execu��o
    #endif
    // Ignoramos erros caso o diret�rio j� exista
}


//gerencia a opera��o de aluguel de um livro por um cliente.
void alugar_livro(FILE *livro, FILE *cliente) {
  int codc; // c�digo (ID) do cliente
  int codl; // c�digo do livro
  long pos_livro, pos_cliente; // posi��es dos registros nos arquivos

  printf("DIGITE O ID DO CLIENTE: ");
  scanf("%d", &codc);
  printf("DIGITE O C�DIGO DO LIVRO: ");
  scanf("%d", &codl);

  TCliente *c = busca_cliente_sis(cliente, codc);
  TLivro *l = busca_livro_sis(livro, codl);

  if (c == NULL) {
    printf("\nERRO: Cliente com ID %d n�o encontrado.\n", codc);
    if(l) free(l);
    return;
  }
  if (l == NULL) {
    printf("\nERRO: Livro com c�digo %d n�o encontrado.\n", codl);
    if(c) free(c);
    return;
  }

  if (l->qtd <= 0) {
    printf("\nTODOS OS EXEMPLARES DESTE LIVRO EST�O ALUGADOS!\n");
  } else if (c->multas > 100) {
    printf("\nO CLIENTE POSSUI MULTAS PENDENTES EM EXCESSO!\n");
  } else if (c->codl != 0) {
    printf("\nO CLIENTE J� POSSUI UM LIVRO ALUGADO (C�d: %d).\nDEVOLVA O LIVRO PRIMEIRO!\n", c->codl);
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

//gerencia a opera��o de devolu��o de um livro por um cliente.
void devolver_livro(FILE *livro, FILE *cliente) {
  int codc;
  printf("DIGITE O ID DO CLIENTE: ");
  scanf("%d", &codc);

  TCliente *c = busca_cliente_sis(cliente, codc);

  if (c == NULL) {
      printf("\nERRO: Cliente com ID %d n�o encontrado.\n", codc);
      return;
  }
  if (c->codl == 0) {
    printf("\nESTE CLIENTE N�O POSSUI LIVROS ALUGADOS!\n");
    free(c);
    return;
  }

  TLivro *l = busca_livro_sis(livro, c->codl);
  if (l == NULL) {
      printf("\nERRO: O livro alugado (c�d %d) n�o foi encontrado na base de dados.\n", c->codl);
      free(c);
      return;
  }

  if (c->multas > 0) {
    printf("\nCLIENTE COM MULTAS, LEMBRE-SE DE COBR�-LAS!\n");
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
      printf("\nERRO: Cliente com ID %d n�o encontrado.\n", codc);
      return;
  }
  if (c->codl == 0) {
    printf("\nCLIENTE N�O TEM LIVROS ALUGADOS PARA GERAR MULTA!\n");
    free(c);
    return;
  }

  TLivro *l = busca_livro_sis(livro, c->codl);
   if (l == NULL) {
      printf("\nERRO: O livro alugado (c�d %d) n�o foi encontrado na base de dados.\n", c->codl);
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
    printf("\n=============== MENU DE OP��ES ===============\n");
    printf("1.  Cadastrar Funcion�rio\n");
    printf("2.  Cadastrar Cliente\n");
    printf("3.  Cadastrar Livro\n");
    printf("---------------------------------------------\n");
    printf("4.  Imprimir Todos os Funcion�rios\n");
    printf("5.  Imprimir Todos os Clientes\n");
    printf("6.  Imprimir Todos os Livros\n");
    printf("---------------------------------------------\n");
    printf("7.  Busca Sequencial\n");
    printf("8.  Busca Bin�ria (requer base ordenada)\n");
    printf("---------------------------------------------\n");
    printf("9.  Alugar Livro\n");
    printf("10. Devolver Livro\n");
    printf("11. Aplicar Multa\n");
    printf("---------------------------------------------\n");
    printf("12. Recriar Base de Funcion�rios (Desordenada)\n");
    printf("13. Recriar Base de Livros (Desordenada)\n");
    printf("14. Recriar Base de Clientes (Desordenada)\n");
    printf("15. Ordenar Arquivos (Merge Sort - Classifica��o Interna)\n");
    printf("16. Ordenar Clientes (Sele��o por Substitui��o + Intercala��o �tima)\n");
    printf("17. Comparar M�todos de Ordena��o (Clientes)\n");
    printf("---------------------------------------------\n");
    printf("18. Limpar Terminal\n");
    printf("0.  Sair\n");
    printf("=============================================\n");
}

// REQUISITO 3: Esta fun��o permite ao usu�rio criar uma base de dados de teste
// com um tamanho espec�fico, cumprindo a parte de "bases de diferentes tamanhos".
void criar_base_teste_clientes() {
    FILE *client_file;
    int tam_base;

    printf("\n--- CRIAR BASE DE CLIENTES PARA TESTE ---\n");
    printf("Digite a quantidade de clientes desordenados a serem gerados: ");
    scanf("%d", &tam_base);
    if (tam_base <= 0) {
        printf("Tamanho inv�lido.\n");
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
    printf("Agora voc� pode usar as op��es 15 e 16 para testar os algoritmos de ordena��o.\n");
}


// REQUISITO 3.a: Esta fun��o executa o m�todo de ordena��o da Atividade I.
void executar_ordenacao_merge() {
    if (!g_test_database_exists) {
        printf("\nERRO: Voc� precisa primeiro criar uma base de clientes desordenada usando a op��o 14.\n");
        return;
    }

    printf("\n--- EXECUTANDO: Merge Sort (Classifica��o Interna) ---\n");
    FILE* client_file = fopen(g_test_filename, "r+b");
    if (!client_file) {
        perror("Nao foi possivel abrir o arquivo de teste.");
        return;
    }

    char base_path[100];
    sprintf(base_path, "%s/part_cli_merge_", dir_merge);

    // REQUISITO 3.c: Inicia a medi��o de tempo (log de tempo).
    clock_t inicio = clock();
    int num_p = classificacao_interna_cliente(client_file, base_path);
    intercalacao_cliente(base_path, num_p, "cli_ordenado_merge.dat");
    clock_t fim = clock();

    // REQUISITO 3.c: Finaliza e armazena os dados de desempenho (tempo e parti��es).
    stats_merge.time_taken = (double)(fim - inicio) / CLOCKS_PER_SEC;
    stats_merge.partitions_generated = num_p;
    stats_merge.valid = 1;

    printf("-> Teste conclu�do! As parti��es foram salvas em '%s/'\n", dir_merge);
    printf("   Resultados armazenados para compara��o.\n");

    fclose(client_file);
    remove("cli_ordenado_merge.dat");
}

// REQUISITO 3.b: Esta fun��o executa o novo m�todo de ordena��o (Req. 1 + Req. 2).
void executar_ordenacao_selecao() {
    if (!g_test_database_exists) {
        printf("\nERRO: Voc� precisa primeiro criar uma base de clientes desordenada usando a op��o 14.\n");
        return;
    }

    printf("\n--- EXECUTANDO: Sele��o por Substitui��o + Intercala��o �tima ---\n");
    FILE* client_file = fopen(g_test_filename, "r+b");
    if (!client_file) {
        perror("Nao foi possivel abrir o arquivo de teste.");
        return;
    }

    char base_path[100];
    sprintf(base_path, "%s/part_cli_subst_", dir_selection);

    // REQUISITO 3.c: Inicia a medi��o de tempo (log de tempo).
    clock_t inicio = clock();
    int num_p = selecao_substituicao_cliente(client_file, base_path);
    intercalacao_otima_cliente(base_path, num_p, "cli_ordenado_subst.dat");
    clock_t fim = clock();

    // REQUISITO 3.c: Finaliza e armazena os dados de desempenho (tempo e parti��es).
    stats_selection.time_taken = (double)(fim - inicio) / CLOCKS_PER_SEC;
    stats_selection.partitions_generated = num_p;
    stats_selection.valid = 1;

    printf("-> Teste conclu�do! As parti��es foram salvas em '%s/'\n", dir_selection);
    printf("   Resultados armazenados para compara��o.\n");

    fclose(client_file);
    remove("cli_ordenado_subst.dat");
}

// REQUISITO 3.c: Esta fun��o gera o "breve relat�rio" comparativo.
void exibir_comparacao_simplificada() {
    printf("\n--- RELAT�RIO DE COMPARA��O DE ORDENA��O ---\n");

    if (!stats_merge.valid || !stats_selection.valid) {
        printf("\nERRO: � necess�rio executar ambos os testes de ordena��o (op��es 15 e 16) para comparar.\n");
        if (!stats_merge.valid) printf("- Teste 'Merge Sort' pendente.\n");
        if (!stats_selection.valid) printf("- Teste 'Sele��o por Substitui��o' pendente.\n");
        return;
    }

    // REQUISITO 3.c: Apresenta os dados coletados (logs) para o m�todo 1.
    printf("\n> M�todo 1: %s\n", stats_merge.method_name);
    printf("  - Tempo gasto: %.6f segundos\n", stats_merge.time_taken);
    printf("  - Parti��es geradas: %d\n", stats_merge.partitions_generated);

    // REQUISITO 3.c: Apresenta os dados coletados (logs) para o m�todo 2.
    printf("\n> M�todo 2: %s\n", stats_selection.method_name);
    printf("  - Tempo gasto: %.6f segundos\n", stats_selection.time_taken);
    printf("  - Parti��es geradas: %d\n", stats_selection.partitions_generated);

    // REQUISITO 3.c: Apresenta a an�lise/opini�o final, completando o relat�rio.
    printf("\n---------------------------------------------\n");
    printf("An�lise: ");
    if (stats_selection.time_taken < stats_merge.time_taken) {
        printf("O m�todo de 'Sele��o por Substitui��o' foi mais eficiente em tempo de execu��o.\n");
    } else if (stats_merge.time_taken < stats_selection.time_taken) {
        printf("O m�todo de 'Merge Sort (Classifica��o Interna)' foi mais eficiente em tempo de execu��o.\n");
    } else {
        printf("Ambos os m�todos tiveram tempos de execu��o praticamente id�nticos.\n");
    }
}


int main(int argc, char **argv) {
  setlocale(LC_ALL, "Portuguese");
  int aux = -1, aux2 = -1;
  FILE *func, *livr, *client;

  printf("Criando diret�rios para parti��es (se n�o existirem)...\n");
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
      printf("Erro fatal: N�o foi poss�vel abrir ou criar os arquivos de dados.\n");
      exit(1);
  }

  printf("Sincronizando arquivos de visualiza��o .txt...\n");
  atualiza_funcionario_txt(func);
  atualiza_livro_txt(livr);
  atualiza_cliente_txt(client, livr);

  while (aux != 0) {
    menu();
    printf("\nDigite a op��o desejada: ");
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
        printf("\n--- Submenu Busca Sequencial ---\n1. Funcion�rio\n2. Livro\n3. Cliente\nEscolha: ");
        scanf("%d", &aux2);
        if (aux2 == 1) busca_sequencial_funcionario(func);
        else if (aux2 == 2) busca_sequencial_livro(livr);
        else if (aux2 == 3) busca_sequencial_cliente(client, livr);
        break;
      case 8:
        printf("\n--- Submenu Busca Bin�ria ---\n1. Funcion�rio\n2. Livro\n3. Cliente\nEscolha: ");
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
      // REQUISITO 3: O usu�rio escolhe a op��o 14 para criar a base de teste.
      case 14:
        criar_base_teste_clientes();
        break;
      // REQUISITO 3.a: O usu�rio escolhe a op��o 15 para rodar o m�todo da Atividade I.
      case 15:
        executar_ordenacao_merge();
        break;
      // REQUISITO 3.b: O usu�rio escolhe a op��o 16 para rodar o novo m�todo.
      case 16:
        executar_ordenacao_selecao();
        break;
      // REQUISITO 3.c: O usu�rio escolhe a op��o 17 para ver o relat�rio.
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
        printf("Op��o inv�lida!\n");
        break;
    }
  }

  fclose(func);
  fclose(livr);
  fclose(client);
  // Remove o arquivo de teste ao sair para n�o deixar lixo
  if(g_test_database_exists) {
      remove(g_test_filename);
  }
  return 0;
}
