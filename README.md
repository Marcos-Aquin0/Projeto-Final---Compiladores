# Projeto Final - Compiladores

## Sobre o Projeto

Este projeto é um **compilador** para a linguagem **C-**, desenvolvido por alunos da **Universidade Federal de São Paulo (UNIFESP)** como parte da disciplina de **Compiladores**. O compilador é capaz de realizar as seguintes etapas do processo de compilação:

- **Análise Léxica** (utilizando Flex);
- **Análise Sintática** (utilizando Bison);
- **Análise Semântica**;
- **Tabela de Símbolos**;
- **Geração de Código Intermediário**.

A linguagem-alvo, **C-**, é uma versão simplificada da linguagem C, ideal para aprendizado e implementação de compiladores.

---

## Tecnologias Utilizadas

- **Linguagem de Programação**: C;
- **Analisador Léxico**: Flex;
- **Analisador Sintático**: Bison.

---

## Estrutura do Projeto

- `lexical_analyzer.l`: Definições e regras para a análise léxica da linguagem C-;
- `parser.y`: Regras de gramática para a análise sintática;
- `symbol_table.c` e `symbol_table.h`: Implementação da Tabela de Símbolos;
- `semantic_analysis.c` e `semantic_analysis.h`: Regras de análise semântica;
- `intermediate_code.c` e `intermediate_code.h`: Geração de código intermediário;
- `main.c`: Arquivo principal para integração e execução do compilador.

---

## Funcionalidades Atuais

### 1. **Analisador Léxico**
Identifica e categoriza tokens da linguagem C-, como:
- Palavras-chave: `if`, `else`, `int`, `return`, `while`, etc.;
- Operadores: `+`, `-`, `*`, `/`, `==`, `!=`, `<`, `<=`, `>` e `>=`;
- Delimitadores: `{`, `}`, `(`, `)`, `;`, `,`, etc.;
- Identificadores e números.

### 2. **Analisador Sintático**
Verifica a conformidade do código com a gramática definida para a linguagem C-.

### 3. **Mecanismo de Erros**
Mensagens claras e detalhadas para erros léxicos e sintáticos, incluindo a linha do erro.

---

## Como Usar

### Pré-requisitos

- **Flex**: Para gerar o analisador léxico;
- **Bison**: Para gerar o analisador sintático;
- **Compilador C** (ex.: `gcc`).

### Passos

1. Clone o repositório:
   ```bash
   git clone https://github.com/Marcos-Aquin0/Projeto-Final---Compiladores
   cd Projeto-Final---Compiladores
   ```

2. Vá para a pasta `Projeto_final`:
   ```bash
   cd Projeto_final
   ```

3. Compile o projeto com o Makefile:
   ```bash
   make
   ```

4. Execute o compilador com um arquivo de teste:
   ```bash
   ./cminus_compiler < Tests/sort.c-
   ```

5. Apague os executáveis gerados após o uso (opcional):
   ```bash
   make clean
   ```
   
---

## Próximas Etapas

- Implementar a **Análise Semântica**;
- Finalizar a **Tabela de Símbolos**;
- Adicionar suporte para **Geração de Código Intermediário**;
- Adicionar suporte para **otimizações básicas** no código intermediário.

---

## Contribuidores

Este projeto foi desenvolvido por alunos da UNIFESP para a disciplina de Compiladores. Agradecimentos ao professor e colegas por todo o suporte e aprendizado ao longo do desenvolvimento.

---

## Licença

Este projeto é de uso acadêmico e não possui uma licença específica.
