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
- `globals.h`: variáveis e bibliotecas principais/globais.
- 1.`lex.flex`: Definições e regras para a análise léxica da linguagem C-;
- 2.`parser.y` e `parser.h`: Regras de gramática para a análise sintática;
- 3.`asnt.c` e `asnt.h`: Implementação e impressão da árvore sintática;
- 4.`symtab.c` e `symtab.h`: Construção da tabela de símbolos a partir da árvore sintática;
- 5.`semantic.c` e `semantic.h`: Análise semântica.
- 6.`cinter.c` e `cinter.h`: gerador de código intermediário.
- `global_debug.c` definição das funções glboais de debug para utilizar a flag
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

### 4. **Gerador de Código Intermediário**
Traduz o código de entrada C- em código de três endereços, similar ao assembly em MIPS.

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

4. Execute o compilador com um arquivo de teste e as flags de impressão da árvore:
   ```bash
   ./cminus_compiler --print-full-tree < Tests/sort.c-
   ```
   ```bash
   ./cminus_compiler --print-tree < Tests/sort.c-
   ```

5. Apague os executáveis gerados após o uso (opcional):
   ```bash
   make clean
   ```
   
---

## Próximas Etapas
- variaveis criadas dentro de blocos
- provavelmente está reservando o registrador (r5, r27) em algum lugar, mas sem usar
- verificar a liberação de temporários e imediatos
- rever gcd e sort

- integração com o processador MIPS (lab de AOC), modelagem sysml faltante e relatório
- o pc é + 1 e a memória +4?
- nop precisa iniciar os registradores sp, fp, ra e 63 como 0 , 62 com -4 e 61 com 1
- reservar registrador r3 para input no processador
- instrução de "enter" no processador para pegar um valor e esperar (era output2)

- gerador de código binário

- suporte para várias chamadas da mesma função no mesmo escopo
- suporte para break em while
- suporte para float
- arrumar static void do semantic

## Contribuidores

Este projeto foi inicialmente desenvolvido por Marcos Aquino e Matheus Esteves, alunos da UNIFESP para a disciplina de Compiladores e, posteriormente, aprimorado por Marcos Aquino para a disciplina de Laboratório de Sistemas Computacionais: Compiladores. Agradecimentos ao professor e colegas por todo o suporte e aprendizado ao longo do desenvolvimento.

---

## Licença

Este projeto é de uso acadêmico e não possui uma licença específica.
