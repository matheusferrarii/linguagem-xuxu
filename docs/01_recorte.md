# O recorte da Xuxu — decisões fixadas no primeiro módulo

Registro das três decisões que a Tarefa 1 pede. Cada uma vem com a alternativa
descartada, porque é a comparação que torna a decisão compreensível quando
ela precisar ser revisitada.

**Domínio:** os padrões da Xuxu falam sobre o código-fonte da própria
linguagem Xuxu. Quem escreve os padrões é quem constrói o compilador da
Xuxu; a entrada processada é um arquivo `.xuxu`.

## Que classe de padrões o sistema aceita

**Decisão:** expressões regulares com concatenação, alternância (`|`), fecho
(`*`), fecho positivo (`+`), opcional (`?`), classe de caracteres (`[...]`),
coringa (`.`) e agrupamento por parênteses.

**Núcleo mínimo:** concatenação, alternância e fecho — detalhado, com a
tabela de reduções, em [`especificacao/nucleo_minimo.md`](../especificacao/nucleo_minimo.md).
Os outros operadores são conveniência de escrita, reduzidos ao núcleo logo
depois da leitura.

**Descartado:** grupos de captura e retrovisores (*backreferences*), pela
mesma razão do material de referência — retrovisor sai da classe das
linguagens regulares e não pode ser compilado para autômato finito.

## Que forma tem a descrição escrita pelo usuário

**Decisão:** um arquivo de especificação (extensão `.lex`) é uma sequência
de declarações `pattern NOME = /regex/;`, uma por classe léxica. Não há
bloco de regras condicionais: a classificação de cada trecho do arquivo
`.xuxu` de entrada é automática, por casamento mais longo entre os padrões
declarados, com a ordem de declaração como critério de desempate.

A gramática completa está em [`01_gramatica.txt`](01_gramatica.txt), e o
exemplo canônico em [`exemplos/exemplo01.lex`](../exemplos/exemplo01.lex).

**Descartado:** um bloco `rule { on X(x) where ... => emit(...) }`, como o
do material de referência. Lá ele existe porque o sistema busca padrões
condicionalmente sobre um domínio externo (e-mails, números acima de um
limite). Aqui o objetivo é só reconhecer e classificar os tokens do próprio
código-fonte da Xuxu — não há condição a avaliar, então o bloco de regras
não paga o que custa: seria uma camada inteira (linguagem de condições,
bytecode, avaliador) sem uso real neste recorte.

## O que o sistema produz

**Decisão:** a lista de tokens reconhecidos no arquivo `.xuxu` de entrada,
na ordem em que aparecem, cada um com `(classe, valor, posição inicial)`.

**Descartado:** compilar para um objeto de máquina (autômatos + bytecode +
máquina virtual), como no material de referência. Essa camada faz sentido
para uma ferramenta de busca de propósito geral sobre entrada externa; para
o léxico da própria Xuxu, a lista de tokens classificados já é o produto
final que o resto do compilador (quando existir) vai consumir.
