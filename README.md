# Linguagem Xuxu — Compiladores e Linguagens Formais e Autômatos

Integrantes: David Lino, Enzo Brumate, Maria Ladeia, Matheus Floresti, Henrique Garcia, Leonardo Toledo

Implementação em C++20 do pipeline de reconhecimento léxico da **Linguagem Xuxu**: leitura e redução de expressões regulares para árvore sintática (Módulo 2), autômato finito determinístico manual (Módulo 3), construção de Thompson e simulação de AFN (Módulo 4) e determinização por construção de subconjuntos e minimização por refinamento de partições (Módulo 5).

---

## Como compilar e rodar todos os testes (comando único)

Para compilar e executar os testes de todas as partes (incluindo as Partes 1 e 2 desta entrega) com um único comando:

```bash
make run
```

Para rodar especificamente os testes das Partes 1 (Módulo 4 — Thompson e AFN) e 2 (Módulo 5 — Determinização e Minimização):

```bash
make afn determinizar
```

*(No Windows sem `make`, pode-se executar diretamente em PowerShell:)*

```powershell
g++ -std=c++20 -Wall -Wextra -Werror -o src/04_demo.exe src/04_demo.cpp src/04_afn.cpp src/02_regex.cpp; g++ -std=c++20 -Wall -Wextra -Werror -o src/05_demo.exe src/05_demo.cpp src/05_determinizacao.cpp src/04_afn.cpp src/02_regex.cpp; .\src\04_demo.exe afn; .\src\05_demo.exe determinizar
```

Também é possível inspecionar qualquer expressão regular e cadeia diretamente pela linha de comando:

```bash
./src/05_demo "(a|b)*c" "ababac"
```

---

## Onde está cada parte no repositório

| Parte / Componente | Arquivos | O que implementa |
| --- | --- | --- |
| **Árvore sintática e reduções (Módulo 2)** | [`src/02_regex.h`](src/02_regex.h), [`src/02_regex.cpp`](src/02_regex.cpp), [`src/02_demo.cpp`](src/02_demo.cpp) | Leitor de padrões (`analisarExpressao`) que reduz `+`, `?`, `[...]` e `(...)` ao núcleo mínimo (`Simbolo`, `Qualquer`, `Vazio`, `Concatenacao`, `Alternancia`, `Fecho`). |
| **Construção de Thompson (Módulo 4 — Parte 1)** | [`src/04_afn.h`](src/04_afn.h), [`src/04_afn.cpp`](src/04_afn.cpp) | `construirAfn(arvore)`: transforma a árvore sintática num AFN com transições vazias ($\varepsilon$) para todos os operadores reconhecidos pelo projeto. |
| **Fecho vazio, movimento e simulação do AFN (Módulo 4 — Parte 1)** | [`src/04_afn.h`](src/04_afn.h), [`src/04_afn.cpp`](src/04_afn.cpp), [`src/05_determinizacao.cpp`](src/05_determinizacao.cpp) | `fechoVazio`, `conjuntoInicial`, `mover`, `moverEFechar`, `simular` e `aceita(afn, cadeia)`. |
| **Determinização — Construção de subconjuntos (Módulo 5 — Parte 2)** | [`src/05_determinizacao.h`](src/05_determinizacao.h), [`src/05_determinizacao.cpp`](src/05_determinizacao.cpp) | `alfabetoDoAfn`, `determinizar`: constrói o AFD guardando em `subconjuntos` o conjunto de estados do AFN que cada estado do AFD representa (incluindo o conjunto vazio `{}` como estado de erro quando alcançável). |
| **Minimização — Refinamento de partições (Módulo 5 — Parte 2)** | [`src/05_determinizacao.h`](src/05_determinizacao.h), [`src/05_determinizacao.cpp`](src/05_determinizacao.cpp) | `completarAfd`, `estadosAlcancaveis` (descarte de inacessíveis via BFS), `particaoInicial`, `assinatura`, `refinarParticao` e `minimizar` até o ponto fixo. |
| **Testes e baterias de casos (Partes 1 e 2)** | [`src/04_demo.cpp`](src/04_demo.cpp), [`exemplos/04_casos.txt`](exemplos/04_casos.txt), [`src/05_demo.cpp`](src/05_demo.cpp) | Verificações da tabela de Thompson, bateria de 44 casos conferidos à mão (`04_casos.txt`), comparação exaustiva das 3 máquinas (`todasAsCadeias` e `compararSobre` nos dois saltos) e medição da explosão de estados. |

---

## Conferência com `(a|b)*c` sobre `{a, b, c}`

- **Construção de Thompson:** `10` estados, `12` transições e `9` transições vazias. Aceita `ababac` e rejeita `abab`.
- **Determinização:** `5` estados alcançáveis (`D0 = {0,2,4,6,7,8}`, `D1 = {0,1,2,4,5,7,8}`, `D2 = {0,2,3,4,5,7,8}`, `D3 = {9}` de aceitação e `D4 = {}` que representa o conjunto vazio / estado de erro).
- **Minimização:** refinamento `2 -> 3 -> 3` (ponto fixo), reduzindo de `5` para **`3` estados** completos (`[D0,D1,D2]`, `[D3]` e o estado de erro `[D4]`).
- **Bateria exaustiva até comprimento 4:** $1 + 3 + 9 + 27 + 81 = 121$ cadeias sobre `{a, b, c}`. As três máquinas (AFN, AFD e AFD mínimo) concordam em todas: **aceitam as mesmas 15 cadeias** e **rejeitam as mesmas 106**. Até comprimento 6 ($5 + 3 - 2 = 6$), concordam em todas as `1093` cadeias (`63` aceitas e `1030` recusadas).

---

## Medição nos padrões léxicos da Linguagem Xuxu (`exemplos/exemplo01.lex`)

| Classe léxica | Padrão | Estados AFN | Estados AFD | Estados AFD Mínimo | Refinamento da partição |
| --- | --- | ---: | ---: | ---: | --- |
| Palavras reservadas (amostra) | `xuxuVerde\|xuxuMaduro\|xuxu\|talvez\|ouEntao\|fudeu` | 92 | 35 | **30** | `2 -> 6 -> 12 -> 18 -> 24 -> 28 -> 30 -> 30` |
| Decimal | `[0-9]+\.[0-9]+` | 158 | 43 | **5** | `2 -> 3 -> 4 -> 5 -> 5` |
| Inteiro | `[0-9]+` | 78 | 21 | **2** | `2 -> 2` |
| Atribuição | `=` | 2 | 3 | **3** | `2 -> 3 -> 3` |
| Identificador | `[a-zA-Z_][a-zA-Z0-9_]*` | 462 | 118 | **3** | `2 -> 3 -> 3` |

---

## Explosão de estados construída de propósito

Linguagem: **o $n$-ésimo símbolo contado a partir do fim é `a`**, sobre o alfabeto `{a, b}`, descrita pela família de padrões:

$$\text{padrão}(n) = (a|b)^*a\underbrace{(a|b)(a|b)\cdots(a|b)}_{n-1 \text{ vezes}}$$

Para lembrar os últimos $n$ símbolos lidos de forma determinística, qualquer AFD precisa distinguir pelo menos $2^n$ sufixos. Abaixo estão as contagens impressas por `src/05_demo`:

| $n$ | Padrão | Estados AFN (Thompson) | Estados AFD (Determinizado) | Estados AFD Mínimo |
| ---: | --- | ---: | ---: | ---: |
| 1 | `(a\|b)*a` | 10 | 3 | **2** ($2^1$) |
| 2 | `(a\|b)*a(a\|b)` | 16 | 5 | **4** ($2^2$) |
| 3 | `(a\|b)*a(a\|b)(a\|b)` | 22 | 9 | **8** ($2^3$) |
| 4 | `(a\|b)*a(a\|b)(a\|b)(a\|b)` | 28 | 17 | **16** ($2^4$) |
| 5 | `(a\|b)*a(a\|b)^4` | 34 | 33 | **32** ($2^5$) |
| 6 | `(a\|b)*a(a\|b)^5` | 40 | 65 | **64** ($2^6$) |
| 7 | `(a\|b)*a(a\|b)^6` | 46 | 129 | **128** ($2^7$) |
| 8 | `(a\|b)*a(a\|b)^7` | 52 | 257 | **256** ($2^8$) |

Enquanto o AFN de Thompson cresce linearmente ($6n + 4$ estados), o AFD determinizado cresce exponencialmente ($2^n + 1$ estados) e o AFD mínimo mantém exatamente $2^n$ estados — comprovando que a explosão vem da própria linguagem, e não de desperdício da construção de subconjuntos.
