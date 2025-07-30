#include "cliente.h"
#include "funcionario.h"
#include "livro.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

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
  printf("\n\n==================== MENU DE OP��ES ====================\n");
  printf("1.  Cadastrar Funcion�rio\n");
  printf("2.  Cadastrar Cliente\n");
  printf("3.  Cadastrar Livro\n");
  printf("------------------------------------------------------\n");
  printf("4.  Imprimir Todos os Funcion�rios\n");
  printf("5.  Imprimir Todos os Clientes\n");
  printf("6.  Imprimir Todos os Livros\n");
  printf("------------------------------------------------------\n");
  printf("7.  Busca Sequencial\n");
  printf("8.  Busca Bin�ria (requer base ordenada)\n");
  printf("------------------------------------------------------\n");
  printf("9.  Alugar Livro\n");
  printf("10. Devolver Livro\n");
  printf("11. Aplicar Multa\n");
  printf("------------------------------------------------------\n");
  printf("12. Recriar Base de Funcion�rios (Desordenada)\n");
  printf("13. Recriar Base de Livros (Desordenada)\n");
  printf("14. Recriar Base de Clientes (Desordenada)\n");
  printf("15. Ordenar Arquivos (Merge Sort - Classifica��o Interna)\n");
  printf("16. Ordenar Clientes (Sele��o por Substitui��o + Intercala��o �tima)\n");
  printf("17. Comparar M�todos de Ordena��o (Clientes)\n");
  printf("------------------------------------------------------\n");
  printf("18. Limpar Terminal\n");
  printf("0.  Sair\n");
  printf("======================================================\n");
}

// >>>>>>>>> ALTERA��O: Fun��o de compara��o agora usa uma base de tamanho configur�vel <<<<<<<<<<<<<
void comparar_ordenacao_clientes() {
    FILE *client_file, *log_file;
    clock_t inicio, fim;
    double tempo_merge, tempo_substituicao;
    int num_p_merge, num_p_substituicao;
    int tam_base;

    printf("\nDigite o tamanho da base de dados para o teste (ex: 1000, 5000, 10000): ");
    scanf("%d", &tam_base);
    if (tam_base <= 0) {
        printf("Tamanho inv�lido.\n");
        return;
    }

    printf("\n--- COMPARA��O DE M�TODOS DE ORDENA��O (Base: %d Clientes) ---\n", tam_base);
    log_file = fopen("log_ordenacao_cliente.txt", "a");
    if (!log_file) {
        perror("N�o foi poss�vel abrir o arquivo de log");
        return;
    }
    fprintf(log_file, "=================================================\n");
    fprintf(log_file, "IN�CIO DO TESTE - BASE DE %d REGISTROS\n", tam_base);
    fprintf(log_file, "=================================================\n");

    // --- M�todo 1 (Classifica��o Interna + Intercala��o B�sica) ---
    client_file = fopen("cliente_teste_merge.dat", "w+b");
    cria_clientes_desordenado(client_file, tam_base);
    rewind(client_file);

    inicio = clock();
    num_p_merge = classificacao_interna_cliente(client_file, "part_cli_merge_");
    intercalacao_cliente("part_cli_merge_", num_p_merge, "cli_ordenado_merge.dat");
    fim = clock();
    tempo_merge = (double)(fim - inicio) / CLOCKS_PER_SEC;

    printf("\nM�todo 1 (Class. Interna + Interc. B�sica):\n");
    printf("  - Tempo total: %f segundos\n", tempo_merge);
    printf("  - Parti��es geradas: %d\n", num_p_merge);
    fprintf(log_file, "M�todo 1: Classifica��o Interna + Intercala��o B�sica\n");
    fprintf(log_file, "Tempo: %f s\nParti��es: %d\n\n", tempo_merge, num_p_merge);
    fclose(client_file);
    remove("cliente_teste_merge.dat");


    // --- M�todo 2 (Sele��o por Substitui��o + Intercala��o �tima) ---
    client_file = fopen("cliente_teste_subst.dat", "w+b");
    cria_clientes_desordenado(client_file, tam_base);
    rewind(client_file);

    inicio = clock();
    num_p_substituicao = selecao_substituicao_cliente(client_file, "part_cli_subst_");
    intercalacao_otima_cliente("part_cli_subst_", num_p_substituicao, "cli_ordenado_subst.dat");
    fim = clock();
    tempo_substituicao = (double)(fim - inicio) / CLOCKS_PER_SEC;

    printf("\nM�todo 2 (Sel. por Substitui��o + Interc. �tima):\n");
    printf("  - Tempo total: %f segundos\n", tempo_substituicao);
    printf("  - Parti��es geradas: %d\n", num_p_substituicao);
    fprintf(log_file, "M�todo 2: Sele��o por Substitui��o + Intercala��o �tima\n");
    fprintf(log_file, "Tempo: %f s\nParti��es: %d\n\n", tempo_substituicao, num_p_substituicao);
    fclose(client_file);
    remove("cliente_teste_subst.dat");

    // --- Relat�rio Final ---
    fprintf(log_file, "--- An�lise Comparativa ---\n");
    fprintf(log_file, "A Sele��o por Substitui��o gerou %d parti��es, enquanto a Classifica��o Interna gerou %d.\n", num_p_substituicao, num_p_merge);
    if (num_p_substituicao < num_p_merge) {
        fprintf(log_file, "-> Vantagem para a Sele��o por Substitui��o, que reduz o esfor�o na intercala��o.\n");
    } else {
        fprintf(log_file, "-> O n�mero de parti��es foi similar, o que pode ocorrer para bases de dados muito pequenas ou aleat�rias.\n");
    }

    fprintf(log_file, "\nEm termos de tempo, o m�todo 1 levou %f s e o m�todo 2 levou %f s.\n", tempo_merge, tempo_substituicao);
    if (tempo_substituicao < tempo_merge) {
        fprintf(log_file, "-> O m�todo 2 foi %.2fx mais r�pido, beneficiado pelo menor n�mero de parti��es e pela efici�ncia do heap na intercala��o.\n", tempo_merge / tempo_substituicao);
    } else {
        fprintf(log_file, "-> O m�todo 1 foi %.2fx mais r�pido, indicando que para esta base de dados o overhead dos algoritmos mais complexos do m�todo 2 n�o foi compensado.\n", tempo_substituicao / tempo_merge);
    }
     fprintf(log_file, "FIM DO TESTE\n\n");

    fclose(log_file);
    printf("\nRelat�rio de compara��o salvo em 'log_ordenacao_cliente.txt'.\n");
    remove("cli_ordenado_merge.dat");
    remove("cli_ordenado_subst.dat");
}


int main(int argc, char **argv) {
  setlocale(LC_ALL, "Portuguese");
  int aux = -1, aux2 = -1;
  FILE *func, *livr, *client;

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
        le_funcionarios(func);
        atualiza_funcionario_txt(func);
        break;
      case 13:
        cria_livros_desordenado(livr);
        le_livros(livr);
        atualiza_livro_txt(livr);
        break;
      case 14: {
        int tam_base;
        printf("Digite o n�mero de clientes para criar na base: ");
        scanf("%d", &tam_base);
        cria_clientes_desordenado(client, tam_base);
        le_clientes(client, livr);
        atualiza_cliente_txt(client, livr);
        break;
      }
      case 15:
        printf("\n--- Submenu Ordenar Arquivos ---\n1. Funcion�rios\n2. Livros\n3. Clientes\nEscolha: ");
        scanf("%d", &aux2);
        if (aux2 == 1) {
            printf("Ordenando arquivo de funcion�rios...\n");
            int num_p = classificacao_interna_funcionario(func, "part_func_");
            intercalacao_funcionario("part_func_", num_p, "func_ordenado.dat");
            fclose(func);
            remove("funcionario.dat");
            rename("func_ordenado.dat", "funcionario.dat");
            func = fopen("funcionario.dat", "r+b");
            printf("Arquivo de funcion�rios ordenado com sucesso!\n");
            atualiza_funcionario_txt(func);
        } else if (aux2 == 2) {
            printf("Ordenando arquivo de livros...\n");
            int num_p = classificacao_interna_livro(livr, "part_livro_");
            intercalacao_livro("part_livro_", num_p, "livro_ordenado.dat");
            fclose(livr);
            remove("livro.dat");
            rename("livro_ordenado.dat", "livro.dat");
            livr = fopen("livro.dat", "r+b");
            printf("Arquivo de livros ordenado com sucesso!\n");
            atualiza_livro_txt(livr);
        } else if (aux2 == 3) {
            printf("Ordenando arquivo de clientes...\n");
            int num_p = classificacao_interna_cliente(client, "part_cli_");
            intercalacao_cliente("part_cli_", num_p, "cli_ordenado.dat");
            fclose(client);
            remove("cliente.dat");
            rename("cli_ordenado.dat", "cliente.dat");
            client = fopen("cliente.dat", "r+b");
            printf("Arquivo de clientes ordenado com sucesso!\n");
            atualiza_cliente_txt(client, livr);
        }
        break;
      case 16: {
          printf("Ordenando arquivo de clientes (Sele��o por Substitui��o)...\n");
          int num_p = selecao_substituicao_cliente(client, "part_cli_subst_");
          intercalacao_otima_cliente("part_cli_subst_", num_p, "cli_ordenado_subst.dat");
          fclose(client);
          remove("cliente.dat");
          rename("cli_ordenado_subst.dat", "cliente.dat");
          client = fopen("cliente.dat", "r+b");
          printf("Arquivo de clientes ordenado com sucesso!\n");
          atualiza_cliente_txt(client, livr);
        }
        break;
      case 17:
        comparar_ordenacao_clientes();
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
  return 0;
}
