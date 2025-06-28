#include "cliente.h"
#include "funcionario.h"
#include "livro.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h> // Biblioteca para configuração de local

void alugar_livro(FILE *livro, FILE *cliente) {
  int codc;
  int codl;
  int pos_livro, pos_cliente;

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
  int pos_livro = busca_posicao_livro_sis(livro, c->codl);
  fseek(livro, pos_livro * tamanho_registro_livro(), SEEK_SET);
  salva_livro(l, livro);
  free(l);

  c->codl = 0;
  int pos_cliente = busca_posicao_cliente_sis(cliente, codc);
  fseek(cliente, pos_cliente * tamanho_registro_cliente(), SEEK_SET);
  salva_cliente(c, cliente);
  free(c);

  printf("\nLIVRO DEVOLVIDO COM SUCESSO!\n");
}

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

  c->multas += l->preco * 0.05; // Acumula multas

  int pos_cliente = busca_posicao_cliente_sis(cliente, codc);
  fseek(cliente, pos_cliente * tamanho_registro_cliente(), SEEK_SET);
  salva_cliente(c, cliente);

  printf("\nMULTA DE R$%.2f APLICADA COM SUCESSO!\n", l->preco * 0.05);
  printf("Total de multas do cliente: R$%.2f\n", c->multas);

  free(c);
  free(l);
}

void menu() {
  printf("\n\n==================== MENU DE OPÇÕES ====================\n");
  printf("1.  Cadastrar Funcionário\n");
  printf("2.  Cadastrar Cliente\n");
  printf("3.  Cadastrar Livro\n");
  printf("------------------------------------------------------\n");
  printf("4.  Imprimir Todos os Funcionários\n");
  printf("5.  Imprimir Todos os Clientes\n");
  printf("6.  Imprimir Todos os Livros\n");
  printf("------------------------------------------------------\n");
  printf("7.  Busca Sequencial\n");
  printf("8.  Busca Binária (requer base ordenada)\n");
  printf("------------------------------------------------------\n");
  printf("9.  Alugar Livro\n");
  printf("10. Devolver Livro\n");
  printf("11. Aplicar Multa\n");
  printf("------------------------------------------------------\n");
  printf("12. Recriar Base de Funcionários (Desordenada)\n");
  printf("13. Recriar Base de Livros (Desordenada)\n");
  printf("14. Recriar Base de Clientes (Desordenada)\n");
  printf("15. Ordenar Arquivos (Merge Sort)\n");
  printf("------------------------------------------------------\n");
  printf("16. Limpar Terminal\n");
  printf("0.  Sair\n");
  printf("======================================================\n");
}


int main(int argc, char **argv) {
  // Configura o local para Português, permitindo acentuação
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
        le_funcionarios(func);
        atualiza_funcionario_txt(func);
        break;
      case 13:
        cria_livros_desordenado(livr);
        le_livros(livr);
        atualiza_livro_txt(livr);
        break;
      case 14:
        cria_clientes_desordenado(client);
        le_clientes(client, livr);
        atualiza_cliente_txt(client, livr);
        break;
      case 15:
        printf("\n--- Submenu Ordenar Arquivos ---\n1. Funcionários\n2. Livros\n3. Clientes\nEscolha: ");
        scanf("%d", &aux2);
        if (aux2 == 1) {
            printf("Ordenando arquivo de funcionários...\n");
            int num_p = classificacao_interna_funcionario(func, "part_func");
            intercalacao_funcionario("part_func", num_p, "func_ordenado.dat");
            fclose(func);
            remove("funcionario.dat");
            rename("func_ordenado.dat", "funcionario.dat");
            func = fopen("funcionario.dat", "r+b");
            printf("Arquivo de funcionários ordenado com sucesso!\n");
            atualiza_funcionario_txt(func);
        } else if (aux2 == 2) {
            printf("Ordenando arquivo de livros...\n");
            int num_p = classificacao_interna_livro(livr, "part_livro");
            intercalacao_livro("part_livro", num_p, "livro_ordenado.dat");
            fclose(livr);
            remove("livro.dat");
            rename("livro_ordenado.dat", "livro.dat");
            livr = fopen("livro.dat", "r+b");
            printf("Arquivo de livros ordenado com sucesso!\n");
            atualiza_livro_txt(livr);
        } else if (aux2 == 3) {
            printf("Ordenando arquivo de clientes...\n");
            int num_p = classificacao_interna_cliente(client, "part_cli");
            intercalacao_cliente("part_cli", num_p, "cli_ordenado.dat");
            fclose(client);
            remove("cliente.dat");
            rename("cli_ordenado.dat", "cliente.dat");
            client = fopen("cliente.dat", "r+b");
            printf("Arquivo de clientes ordenado com sucesso!\n");
            atualiza_cliente_txt(client, livr);
        }
        break;
      case 16: system("clear || cls"); break;
      case 0: printf("Saindo do programa...\n"); break;
      default: printf("Opção inválida!\n"); break;
    }
  }

  fclose(func);
  fclose(livr);
  fclose(client);

  return 0;
}
