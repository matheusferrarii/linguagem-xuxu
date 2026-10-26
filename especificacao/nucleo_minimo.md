# O núcleo mínimo de operadores e as reduções

Tarefa 1 do módulo 2. Critério de inclusão no núcleo: **a impossibilidade de
reduzir**. Um operador que se exprime pela composição de outros é conveniência
de quem escreve o padrão, não capacidade nova do sistema.

O domínio dos padrões descritos aqui é o dos próprios tokens da Xuxu — os
padrões definem como reconhecer cada classe léxica listada em
[`classes_lexicas.md`](classes_lexicas.md), sobre o alfabeto Σ definido em
[`alfabeto.md`](alfabeto.md).

## O núcleo

Três operadores e duas folhas.

| Construção   | Papel                                       |
| ------------ | -------------------------------------------- |
| concatenação | núcleo — uma coisa seguida de outra          |
| alternância  | núcleo — uma coisa ou outra                  |
| fecho        | núcleo — zero ou mais repetições             |
| símbolo      | folha — um símbolo literal do alfabeto       |
| cadeia vazia | folha — produzida pela redução do opcional   |

Tudo o mais que se escreve num padrão é reduzido a isto durante a leitura.

## As reduções, em pares

| Quem escreve o padrão | A árvore recebe            |
| ---------------------- | --------------------------- |
| `x+`                    | `concat(x, fecho(x))`       |
| `x?`                    | `alt(x, vazio)`              |
| `[abc]`                 | `alt(alt('a','b'),'c')`      |
| `[a-c]`                 | `alt(alt('a','b'),'c')`      |
| `(x)`                   | `x` — o grupo não sobrevive à leitura |
| `\.`                    | `'.'` — símbolo literal      |

O grupo não vira nó: parênteses existem para quem escreve dizer onde a
precedência muda, e uma vez que a árvore está construída, a estrutura **é** a
precedência. A árvore de `(a)b` e a de `ab` são a mesma.

## A exceção: o coringa

O coringa `.` **não** é reduzido, embora em princípio pudesse ser — é a
alternância de todos os símbolos do alfabeto. Σ tem várias dezenas de
símbolos (ver `alfabeto.md`), e essa expansão produziria uma árvore grande
por ocorrência só para dizer o que uma folha já diz. Mantivemos o coringa
como folha própria por razão de tamanho, não de expressividade.

## Padrões das classes léxicas da Xuxu

Aplicando o núcleo acima às categorias de `classes_lexicas.md`:

| Classe léxica | Padrão                     |
| -------------- | --------------------------- |
| identificador   | `[a-zA-Z_][a-zA-Z0-9_]*`   |
| inteiro         | `[0-9]+`                    |
| decimal         | `[0-9]+\.[0-9]+`            |

Essas três são as primeiras implementadas no leitor de padrões
(`src/02_regex.h` e `src/02_regex.cpp`). As demais classes (`string`,
`caractere`, `operador_aritmetico`, etc.) serão adicionadas nas próximas
entregas — o núcleo mínimo já é suficiente para descrevê-las, falta só
escrever o padrão de cada uma.

## Descartado

**Quantificador contado** (`x{3,5}`). É redutível — expande em concatenações
e opcionais — então caberia no critério. Ficou de fora por não ser
necessário para nenhuma classe léxica da Xuxu até agora. Se for necessário,
volta como redução, nunca como operador de núcleo.
