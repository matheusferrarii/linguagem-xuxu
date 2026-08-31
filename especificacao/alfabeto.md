# Alfabeto do Xuxu — Σ

Σ é o conjunto de todos os símbolos que podem aparecer num arquivo `.xuxu` ou num arquivo `.lex`.

```
Σ = { a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z,
      A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
      !, @, #, $, %, &, *, |, (, ), ', ", -, =, +, \, /, ?, _, <, >, ., :, ;, {, }, [, ] }
```

**Tamanho:** Σ tem 75 símbolos.

**Por que esse recorte:**
- Letras latinas minúsculas e maiúsculas — identificadores e palavras reservadas da Xuxu.
- Dígitos — literais inteiros e decimais.
- Sinais de pontuação e operadores — delimitadores, operadores aritméticos, relacionais e lógicos.
- As aspas simples e duplas — literais de cadeia e caractere.
- A barra invertida `\` — escapes dentro de padrões e literais.

**Exigências formais satisfeitas:**
- Finito: 75 elementos enumerados acima.
- Não vazio: condição trivialmente satisfeita.

O coringa `.` nas expressões regulares dos padrões denota a alternância de todos os 75 símbolos. Expandir essa alternância manualmente produziria 75 nós por ocorrência; por isso o coringa permanece como folha própria na árvore — razão de tamanho, não de expressividade. (Ver [`nucleo_minimo.md`](nucleo_minimo.md), seção "A exceção: o coringa".)