# O recorte da Xuxu — decisões fixadas no primeiro módulo

Registro das três decisões que a Tarefa 1 pede. Cada uma vem com a alternativa
descartada, porque é a comparação que torna a decisão compreensível quando
ela precisar ser revisitada.

**Domínio:** o arquivo `.lex` é a **interface do sistema** — quem adota a
linguagem escreve os padrões do vocabulário que quer usar. Trocando esse
arquivo, o mesmo motor reconhece programas escritos em outra língua, ou com
duas grafias convivendo. Quem constrói o compilador define o motor; quem usa
a linguagem define o vocabulário.

*Alternativa descartada:* manter os padrões como peça interna, visível apenas
ao construtor do compilador. Descartada porque elimina o diferencial da
proposta: a separação entre motor e vocabulário é o que torna o sistema
reutilizável e o que produz a pergunta teórica do item seguinte.

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
de entrada é automática, por casamento mais longo entre os padrões
declarados, com a ordem de declaração como critério de desempate.

A gramática completa está em [`01_gramatica.txt`](01_gramatica.txt), e o
exemplo canônico em [`exemplos/exemplo01.lex`](../exemplos/exemplo01.lex).

**Descartado:** um bloco `rule { on X(x) where ... => emit(...) }`, como o
do material de referência. Lá ele existe porque o sistema busca padrões
condicionalmente sobre um domínio externo (e-mails, números acima de um
limite). Aqui o objetivo é reconhecer e classificar os tokens da linguagem —
não há condição a avaliar, então o bloco de regras não paga o que custa.

## O que o sistema produz

**Decisão:** o produto primário é o **arquivo de tabelas de transição do
autômato** (extensão `.aut`), gerado a partir das expressões regulares
declaradas no `.lex`. Um segundo componente lê esse arquivo para reconhecer
tokens numa entrada qualquer — a separação entre "compilar os padrões" e
"usar os padrões compilados" é o que permite os módulos futuros (geração de
código, ambientes de execução) consumirem algo concreto.

**Consequência teórica que o produto torna possível:** se `pare` é palavra
reservada e o padrão de identificador aceita qualquer sequência de letras,
existe cadeia que pertence às duas classes. Com o vocabulário vindo de fora,
ninguém garante de antemão que isso não acontece. Descobrir se duas classes
se sobrepõem é construir a máquina que reconhece o que ambas aceitam e
perguntar se ela aceita alguma coisa — **interseção de autômatos**, decidida
antes de qualquer programa ser lido.

**Descartado (lista de tokens):** produzir diretamente a lista de tokens
reconhecidos num arquivo de entrada, sem gravar o autômato. Descartado
porque sem objeto gravado os módulos futuros — geração de código e ambientes
de execução — ficam sem o que consumir no sistema.
