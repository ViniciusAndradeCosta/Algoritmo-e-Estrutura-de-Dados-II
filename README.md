# 🧠 Algoritmos e Estrutura de Dados II — Projeto Prático

Esta disciplina tem como foco a **organização de dados em memória secundária**, abordando estruturas e algoritmos fundamentais para o armazenamento e a manipulação eficiente de grandes volumes de dados fora da memória principal.

Durante o curso, os alunos aprendem a implementar e aplicar:

* Acesso sequencial e aleatório a arquivos;
* Estruturas de dados como árvores B, B+ e B\*;
* Técnicas de ordenação externa e indexação;
* Compressão de dados e casamento de padrões em texto;
* Hashing e tratamento de colisões.

A proposta prática da disciplina é **desenvolver sistemas completos**, onde essas estruturas são aplicadas na resolução de problemas reais — como no gerenciamento de bibliotecas, apresentado neste repositório.

---

## 📘 Sistema de Gerenciamento de Biblioteca — Parte 1 de 3

Este repositório apresenta a **Parte 1** de um sistema completo de gerenciamento de biblioteca, desenvolvido como atividade prática de **Algoritmos e Estrutura de Dados II**.

O objetivo desta fase é construir uma base funcional do sistema, focando na manipulação de arquivos binários e na aplicação de técnicas de busca, ordenação e controle de entidades. As próximas etapas irão introduzir estruturas de indexação e técnicas de acesso mais avançadas.

---

## 📖 Visão Geral do Projeto

A aplicação foi desenvolvida em **linguagem C**, com persistência de dados utilizando arquivos binários (`.dat`) e geração de arquivos de texto (`.txt`) para visualização. São gerenciadas três entidades principais:

* **Clientes**
* **Livros**
* **Funcionários**

Nesta primeira etapa, são implementadas operações de cadastro, consulta, aluguel, devolução, cálculo de multas, além de algoritmos de ordenação externa e buscas eficientes.

---

## ✨ Funcionalidades Implementadas

### 1. Gerenciamento de Entidades

* Cadastro, leitura e listagem de:

  * Clientes
  * Funcionários
  * Livros
* Geração de bases de dados iniciais desordenadas

### 2. Operações de Biblioteca

* **Aluguel de Livros**
* **Devolução de Livros**
* **Cálculo e aplicação de multas**

### 3. Busca de Dados

* **Busca Sequencial**
* **Busca Binária** (em arquivos previamente ordenados)
* **Logs de desempenho** com tempo e comparações

### 4. Ordenação Externa (Merge Sort)

* **Classificação Interna:** uso de `qsort` para partições
* **Intercalação Balanceada:** fusão dos blocos ordenados

---

## 🧠 Estruturas e Algoritmos Utilizados

### Estruturas de Dados

* `TCliente`
* `TLivro`
* `TFunc`

### Algoritmos

* **Busca Linear** (`busca_sequencial_*`)
* **Busca Binária** (`busca_binaria_*`)
* **Ordenação Externa:** `classificacao_interna_*`, `intercalacao_*`

---

## 🗂 Estrutura do Projeto

```
.
├── main.c                  # Lógica principal e menu
├── cliente.c/.h            # Cliente (funções e definições)
├── livro.c/.h              # Livro (funções e definições)
├── funcionario.c/.h        # Funcionário (funções e definições)
├── cliente.dat             # Arquivo binário de clientes
├── livro.dat               # Arquivo binário de livros
├── funcionario.dat         # Arquivo binário de funcionários
├── *.txt                   # Logs e visualizações
└── ...
```

---

## ⚙️ Como Compilar e Executar

### Requisitos

* Compilador C (ex.: `gcc`)

### Passos

```bash
# Compilação
gcc main.c cliente.c livro.c funcionario.c -o sistema_biblioteca

# Execução
./sistema_biblioteca
```

---

## 🔮 Continuidade do Projeto

Este projeto será estendido nas próximas etapas da disciplina:

### Parte 2 — Indexação com Árvores

* Árvores B, B+, B\*
* Otimização do acesso a arquivos
* Inserção com `split`, remoção com `merge`, navegação entre folhas

### Parte 3 — Hashing em Arquivos

* Tabelas Hash com função de dispersão
* Tratamento de colisões
* Análise de desempenho em memória secundária

---


