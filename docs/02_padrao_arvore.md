David Lino, Enzo Brumate, Maria Ladeia, Matheus Floresti, Henrique Garcia, Leonardo Toledo

# Do padrão à árvore, e da árvore de volta ao padrão — Linguagem Xuxu

Base: os exemplos em `docs/*.xuxu` e `exemplos/entrada01.txt`; as classes de
`especificacao/classes_lexicas.md`; o núcleo e as reduções de
`especificacao/nucleo_minimo.md`. As formas lineares e as contagens abaixo
foram conferidas rodando o leitor de padrões (`src/02_regex.cpp`), que imprime
a árvore reduzida na mesma notação usada aqui (`s:'x'`, `vazio`, `qualquer`,
`concat`, `alt`, `fecho`).

Abreviações usadas na coluna de forma linear (só para não repetir subárvores):

- `Dig` = `alt(alt(alt(alt(alt(alt(alt(alt(alt(s:'0',s:'1'),s:'2'),s:'3'),s:'4'),s:'5'),s:'6'),s:'7'),s:'8'),s:'9')` — 19 nós (10 folhas + 9 alternâncias)
- `Ini` = alternância dos 53 símbolos de `[a-zA-Z_]`, juntados dois a dois — 105 nós (53 folhas + 52 alternâncias)
- `Cont` = alternância dos 63 símbolos de `[a-zA-Z0-9_]`, juntados dois a dois — 125 nós (63 folhas + 62 alternâncias)

Regra de contagem de uma classe: `n` símbolos → `n` folhas + `n−1` alternâncias = `2n−1` nós.

---

## 1. Tabela de peças

| Peça | Trecho real | Padrão com açúcar | Padrão no núcleo | Forma linear da árvore reduzida | Caracteres | Nós |
|---|---|---|---|---|---|---|
| **número** | `7.5` (`xuxuMaduro media = 7.5`) | `[0-9]+(\.[0-9]+)?` | `d·d*·(.·d·d* \| ε)`, com `d = 0\|1\|2\|3\|4\|5\|6\|7\|8\|9` | `concat( concat(Dig, fecho(Dig)), alt( concat(s:'.', concat(Dig, fecho(Dig))), vazio ) )` | 17 | **85** |
| **identificador** | `media`, `contador` | `[a-zA-Z_][a-zA-Z0-9_]*` | `i·c*`, com `i = a\|…\|z\|A\|…\|Z\|_` e `c = a\|…\|z\|A\|…\|Z\|0\|…\|9\|_` | `concat( Ini, fecho(Cont) )` | 22 | **232** |
| **palavra reservada** | `talvez`, `fudeu` (`condicao.xuxu`); `chega`, `pula` (`forXuxu.xuxu`) | `talvez\|fudeu\|chega\|pula` | `t·a·l·v·e·z \| f·u·d·e·u \| c·h·e·g·a \| p·u·l·a` | `alt(alt(alt(concat(concat(concat(concat(concat(s:'t',s:'a'),s:'l'),s:'v'),s:'e'),s:'z'),concat(concat(concat(concat(s:'f',s:'u'),s:'d'),s:'e'),s:'u')),concat(concat(concat(concat(s:'c',s:'h'),s:'e'),s:'g'),s:'a')),concat(concat(concat(s:'p',s:'u'),s:'l'),s:'a'))` | 23 | **39** |
| **texto entre aspas** | `"Aprovado"` (`condicao.xuxu`), `"Chuchu"` (`concatenacao.xuxu`) | `".*"` | `"·(.)*·"` (o coringa `.` fica como folha própria — ver `nucleo_minimo.md`) | `concat(concat(s:'"',fecho(qualquer)),s:'"')` | 4 | **6** |
| **sinal** | `<` (`contador < 5`), `+` (`contador + 1`), `=` (`nota = 6`) | `<\|>\|=\|\+` | `<\|>\|=\|+` (já está no núcleo) | `alt(alt(alt(s:'<',s:'>'),s:'='),s:'+')` | 8 | **7** |

De onde saem os **85** do número: a classe `[0-9]` vale 19 (`Dig`). O fecho
positivo `[0-9]+` duplica a subárvore — `19 + 19` — e soma o nó do fecho e o da
concatenação: 40. A parte `\.[0-9]+` é `concat(s:'.', 40)` = 42. O opcional
`(…)?` vira `alt(42, vazio)` = 44. A concatenação da raiz junta `40 + 44 + 1` = **85**.

De onde saem os **232** do identificador: `[a-zA-Z_]` tem 53 símbolos → 105 nós;
`[a-zA-Z0-9_]` tem 63 símbolos → 125 nós; o fecho acrescenta 1 e a concatenação
acrescenta 1: `105 + 126 + 1` = **232**. É a linha que mais cresce, e o texto do
padrão (22 caracteres) não dá nenhum sinal disso. Se o custo pesar num módulo
seguinte, a decisão barata é de notação: restringir o identificador a
minúsculas (`[a-z][a-z0-9_]*`, 126 nós).

### Caminho de volta (Passo 5)

Percorrendo a árvore do inteiro `concat(Dig, fecho(Dig))` e escrevendo só com os
três operadores:

`(0|1|2|3|4|5|6|7|8|9)(0|1|2|3|4|5|6|7|8|9)*`

Descreve as mesmas cadeias que `[0-9]+`: 43 caracteres contra 6. Reconstruindo a
árvore dessa expressão e reduzindo de novo, chega-se à mesma forma linear e aos
mesmos 40 nós — a volta fecha.

---

## 2. Cobertura mínima

- **Fecho:** as peças *número*, *identificador* e *texto entre aspas* usam fecho.
- **Opcional:** a peça *número* usa opcional — `(\.[0-9]+)?` reduz para
  `alt(concat(s:'.', …), vazio)`, e é daí que sai a folha `vazio` da árvore.

---

## 3. Um par que converge

Peça *inteiro* (`nota = 6` em `condicao.xuxu`), escrita de duas maneiras:

| Escrita | Forma linear | Nós |
|---|---|---|
| `[0-9]+` | `concat( Dig, fecho(Dig) )` | 40 |
| `[0-9][0-9]*` | `concat( Dig, fecho(Dig) )` | 40 |

As duas formas lineares são o mesmo texto, caractere por caractere (conferido no
leitor de padrões). A redução é consistente nesse par: `x+` vira
`concat(x, fecho(x))`, que é exatamente `x` seguido de `x*`.

---

## 4. Um par que não converge

Duas expressões que denotam a **mesma** linguagem — todas as cadeias formadas por
`a` e `b`, inclusive a vazia — e cujas árvores são diferentes:

| Escrita | Forma linear | Nós |
|---|---|---|
| `(a\|b)*` | `fecho(alt(s:'a',s:'b'))` | 4 |
| `(a*b*)*` | `fecho(concat(fecho(s:'a'),fecho(s:'b')))` | 6 |

**Demonstração de que denotam o mesmo conjunto.** `(a|b)*` é, por definição,
toda sequência de zero ou mais símbolos, cada um `a` ou `b` — ou seja, `{a,b}*`.
Em `(a*b*)*`, cada repetição do fecho externo produz um bloco `aᶦbʲ` (com `i, j ≥ 0`),
e concatenar qualquer número desses blocos alcança qualquer sequência de `a`s e
`b`s: por exemplo `abba` = `(a)(b b)(a)`, `ε` = zero blocos, `ba` = `(b)(a)`.
Logo também é `{a,b}*`.

**O que isso diz da comparação de árvores.** Comparar duas árvores reduzidas só
detecta a igualdade que veio da redução; não decide equivalência de linguagens.
Aqui as árvores diferem em rótulo e em tamanho, e mesmo assim as linguagens são
iguais. Para responder à pergunta maior seria preciso converter cada expressão
em autômato, minimizar e comparar os dois mínimos.

---

## 5. Dois requisitos, um de cada lado

**Parece sair da classe e não sai — o horário do `agenda`.** Em
`alarme("08:00")` (`crom.xuxu`), o horário vai de `00:00` a `23:59`. Parece
exigir aritmética (comparar com 24 e com 60), mas não exige:

`([01][0-9]|2[0-3]):[0-5][0-9]`

descreve exatamente esse conjunto. A árvore reduzida tem 67 nós (conferido no
leitor). O que a máquina precisa lembrar é apenas em que trecho do horário ela
está, e esse número tem teto fixo: 5 posições.

**Sai mesmo — identificador declarado antes do uso.** Em Xuxu, um nome usado
numa expressão (`feirante(item)`, `contador = contador + 1`) tem de ter sido
declarado antes com `xuxu` / `xuxuVerde` / `xuxuMaduro`. Para verificar isso a
máquina precisaria lembrar **todos** os nomes já declarados, e um programa pode
declarar quantas variáveis quiser — a quantidade não tem teto. O requisito fica
fora da classe regular e fora deste módulo: é verificação para depois de o texto
virar estrutura (tabela de símbolos). É o mesmo tipo de pergunta que
`docs/01_recorte.md` levanta sobre a sobreposição entre `palavra_reservada` e
`identificador`, resolvida por interseção de autômatos.

---

## 6. Quatro recusas com posição

Mensagens e posições produzidas pelo leitor de padrões (`src/02_regex.cpp`).
A posição é o índice, contado de 0, do ponto em que a falta se constata.

| Malformação | Entrada de exemplo | Mensagem | Posição |
|---|---|---|---|
| Grupo que não fecha | `(talvez` | `falta o fecha-parenteses do grupo` | 7 (fim do texto) |
| Repetição sem operando | `+[0-9]` | `operador sem expressao a que se aplicar` | 0 (o `+`) |
| Classe sem colchete final | `[a-z` | `classe de caracteres nao fechada` | 4 (fim do texto) |
| Símbolo sobrando depois do fim | `abc)` | `simbolo inesperado apos o fim da expressao` | 3 (o `)`) |

---

## Como conferimos, antes de entregar

| Verificação | Resultado |
|---|---|
| Contagem de nós de cada linha | **Confirma** — o leitor de padrões imprime o mesmo número para as cinco peças (85, 232, 39, 6, 7). |
| Convergência do par do item 3 | **Confirma** — `[0-9]+` e `[0-9][0-9]*` dão a mesma forma linear e 40 nós. |
| Leitura de volta do Passo 5 | **Confirma** — `(0\|…\|9)(0\|…\|9)*` reconstruída e reduzida de novo dá a mesma forma linear, 40 nós. |
