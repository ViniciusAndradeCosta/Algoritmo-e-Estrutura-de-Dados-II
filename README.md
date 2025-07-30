# 🧠 Algoritmos e Estrutura de Dados II — Projeto Prático

Esta disciplina tem como foco a organização de dados em memória secundária, abordando estruturas e algoritmos fundamentais para o armazenamento e a manipulação eficiente de grandes volumes de dados fora da memória principal.

Durante o curso, os alunos aprendem a implementar e aplicar:

- Acesso sequencial e aleatório a arquivos;
- Estruturas de dados como árvores B, B+ e B*;
- Técnicas de ordenação externa e indexação;
- Compressão de dados e casamento de padrões em texto;
- Hashing e tratamento de colisões.

A proposta prática da disciplina é desenvolver sistemas completos, onde essas estruturas são aplicadas na resolução de problemas reais — como no gerenciamento de bibliotecas, apresentado neste repositório.

---

## 📘 Sistema de Gerenciamento de Biblioteca — Parte 1 e Parte 2

Este repositório apresenta as Partes 1 e 2 de um sistema completo de gerenciamento de biblioteca, desenvolvido como atividade prática de Algoritmos e Estrutura de Dados II.

Na **Parte 1**, o objetivo é construir uma base funcional do sistema, focando na manipulação de arquivos binários e na aplicação de técnicas de busca, ordenação e controle de entidades.

Na **Parte 2**, são aplicadas **estruturas de indexação com árvores B, B+ e B*** para otimizar o acesso a arquivos. Também é realizada uma **comparação entre diferentes métodos de ordenação**, avaliando seu desempenho em termos de tempo e número de comparações.

---

## 📖 Visão Geral do Projeto

A aplicação foi desenvolvida em linguagem C, com persistência de dados utilizando arquivos binários (`.dat`) e geração de arquivos de texto (`.txt`) para visualização. São gerenciadas três entidades principais:

- Clientes  
- Livros  
- Funcionários

As funcionalidades são divididas em etapas, com foco progressivo em técnicas mais avançadas.

---

## ✨ Funcionalidades Implementadas

### 1. Gerenciamento de Entidades
- Cadastro, leitura e listagem de:
  - Clientes
  - Funcionários
  - Livros
- Geração de bases de dados iniciais desordenadas

### 2. Operações de Biblioteca
- Aluguel de Livros
- Devolução de Livros
- Cálculo e aplicação de multas

### 3. Busca de Dados
- Busca Sequencial
- Busca Binária (em arquivos previamente ordenados)
- Logs de desempenho com tempo e comparações

### 4. Ordenação Externa
- Classificação Interna: uso de `qsort` para partições
- Intercalação Balanceada: fusão dos blocos ordenados
- **Comparação entre métodos de ordenação**: análise de desempenho

### 5. Estruturas de Indexação (Parte 2)
- Implementação de Árvores B, B+, e B*
- Inserção com `split`, remoção com `merge`
- Navegação entre folhas em árvores B+

---

## 🧠 Estruturas e Algoritmos Utilizados

### Estruturas de Dados
- `TCliente`
- `TLivro`
- `TFunc`

### Algoritmos
- Busca Linear (`busca_sequencial_*`)
- Busca Binária (`busca_binaria_*`)
- Ordenação Externa (`classificacao_interna_*`, `intercalacao_*`)
- Árvores B, B+, B* (`inserir_arvoreB`, `remover_arvoreB`, etc.)

---

## 🗂 Estrutura do Projeto

```
.
├── main.c                  # Lógica principal e menu
├── cliente.c/.h            # Cliente (funções e definições)
├── livro.c/.h              # Livro (funções e definições)
├── funcionario.c/.h        # Funcionário (funções e definições)
├── arvores.c/.h            # Árvores B, B+, B*
├── cliente.dat             # Arquivo binário de clientes
├── livro.dat               # Arquivo binário de livros
├── funcionario.dat         # Arquivo binário de funcionários
├── *.txt                   # Logs e visualizações
└── ...
```

---

## ⚙️ Como Compilar e Executar

### Requisitos
- Compilador C (ex.: `gcc`)

### Passos

```bash
# Compilação
gcc main.c cliente.c livro.c funcionario.c arvores.c -o sistema_biblioteca

# Execução
./sistema_biblioteca
```

---

## 🔮 Continuidade do Projeto

Este projeto será estendido nas próximas etapas da disciplina:

### ✅ Parte 1 — Base do Sistema
- Manipulação de arquivos
- Busca e ordenação externa

### ✅ Parte 2 — Indexação com Árvores
- Árvores B, B+, B*
- Otimização do acesso a arquivos

### 🔜 Parte 3 — Hashing em Arquivos
- Tabelas Hash com função de dispersão
- Tratamento de colisões
- Análise de desempenho em memória secundária

---

Desenvolvido como parte da disciplina **Algoritmos e Estrutura de Dados II**.
