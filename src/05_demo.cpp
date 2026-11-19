#include "05_determinizacao.h"

#include <iostream>
#include <string>
#include <vector>

using namespace xuxu;

static int verificacoesFalhas = 0;

static void conferir(const bool condicao, const std::string& descricao) {
    std::cout << (condicao ? "  [ok]    " : "  [FALHA] ") << descricao << "\n";
    if (!condicao) ++verificacoesFalhas;
}

static bool compilarCiclo(const std::string& padrao, Afn& afn,
                          ResultadoDaDeterminizacao& det,
                          ResultadoDaMinimizacao& min) {
    const Resultado r = analisarExpressao(padrao);
    if (!r.ok) return false;
    afn = construirAfn(r.arvore);
    det = determinizar(afn);
    min = minimizar(det.afd);
    return true;
}

static void imprimirCicloDetalhado(const std::string& padrao,
                                   const std::string& cadeiaExemplo) {
    Afn afn;
    ResultadoDaDeterminizacao det;
    ResultadoDaMinimizacao min;
    if (!compilarCiclo(padrao, afn, det, min)) {
        std::cout << "  [FALHA] erro ao analisar padrao " << padrao << "\n";
        ++verificacoesFalhas;
        return;
    }

    std::cout << "\n  /" << padrao << "/  AFN " << afn.numEstados()
              << " (" << afn.numTransicoes() << " transicoes, "
              << afn.numVazias() << " vazias)"
              << " -> AFD " << det.estadosDoAfd
              << " -> minimo " << min.estadosDepois
              << "  (alfabeto: " << det.afd.alfabeto.size() << ")\n";

    std::cout << "    1. Subconjuntos que viraram estados do AFD:\n";
    for (std::size_t i = 0; i < det.subconjuntos.size(); ++i) {
        std::cout << "      D" << i << " = " << det.afd.nomes[i]
                  << (det.afd.ehFinal(i) ? " (aceitacao)" : "")
                  << (det.afd.temErro && det.afd.erro == i ? " (erro)" : "")
                  << "\n";
    }

    std::cout << "    2. Tabela de transicoes do AFD:\n"
              << det.afd.formatarTabela();

    std::cout << "    3. Refinamento da particao, rodada a rodada: ";
    for (std::size_t i = 0; i < min.blocosPorRodada.size(); ++i) {
        if (i > 0) std::cout << " -> ";
        std::cout << min.blocosPorRodada[i];
    }
    std::cout << " (ponto fixo)\n";

    std::cout << "    4. Tabela de transicoes do AFD minimizado:\n"
              << min.afd.formatarTabela();

    const bool rAfn = aceita(afn, cadeiaExemplo);
    const bool rAfd = det.afd.aceita(cadeiaExemplo);
    const bool rMin = min.afd.aceita(cadeiaExemplo);
    std::cout << "    5. Consulta para \"" << cadeiaExemplo << "\": "
              << "AFN=" << (rAfn ? "ACEITA" : "RECUSA")
              << ", AFD=" << (rAfd ? "ACEITA" : "RECUSA")
              << ", minimo=" << (rMin ? "ACEITA" : "RECUSA") << "\n";
}

static void medirPadrao(const std::string& padrao,
                        const std::string& descricao) {
    Afn afn;
    ResultadoDaDeterminizacao det;
    ResultadoDaMinimizacao min;
    if (!compilarCiclo(padrao, afn, det, min)) {
        std::cout << "  [FALHA] " << descricao << "\n";
        ++verificacoesFalhas;
        return;
    }
    std::cout << "  /" << padrao << "/  (" << descricao << ")\n"
              << "    AFN " << afn.numEstados()
              << " -> AFD " << det.estadosDoAfd
              << " -> minimo " << min.estadosDepois
              << "  | particao: ";
    for (std::size_t i = 0; i < min.blocosPorRodada.size(); ++i) {
        if (i > 0) std::cout << " -> ";
        std::cout << min.blocosPorRodada[i];
    }
    std::cout << "\n";
}

static void testarEquivalenciaExaustiva(const std::string& padrao,
                                        const std::size_t comprimentoMaximo) {
    Afn afn;
    ResultadoDaDeterminizacao det;
    ResultadoDaMinimizacao min;
    if (!compilarCiclo(padrao, afn, det, min)) {
        conferir(false, "compilacao de /" + padrao + "/");
        return;
    }
    const std::string alfabeto = alfabetoDoAfn(afn);
    const std::vector<std::string> cadeias =
        todasAsCadeias(alfabeto, comprimentoMaximo);

    const Discordancia d1 = compararSobre(afn, det.afd, cadeias);
    const Discordancia d2 = compararSobre(det.afd, min.afd, cadeias);

    std::size_t aceitas = 0;
    for (const std::string& c : cadeias) {
        if (min.afd.aceita(c)) ++aceitas;
    }

    const bool ok = !d1.houve && !d2.houve;
    std::string msg = "/" + padrao + "/  " + std::to_string(cadeias.size()) +
                      " cadeias ate comprimento " +
                      std::to_string(comprimentoMaximo) + " (" +
                      std::to_string(aceitas) + " aceitas, " +
                      std::to_string(cadeias.size() - aceitas) + " recusadas): ";
    if (d1.houve) {
        msg += "AFN != AFD em \"" + d1.cadeia + "\"";
    } else if (d2.houve) {
        msg += "AFD != minimo em \"" + d2.cadeia + "\"";
    } else {
        msg += "as tres maquinas concordam em todas";
    }
    conferir(ok, msg);
}

static std::string padraoDaExplosao(const std::size_t n) {
    std::string padrao = "(a|b)*a";
    for (std::size_t i = 1; i < n; ++i) {
        padrao += "(a|b)";
    }
    return padrao;
}

static void verificacoesGuiadas() {
    std::cout << "== Tarefa 1 e 2: Construcao de subconjuntos e minimizacao ==\n";

    imprimirCicloDetalhado("(a|b)*c", "ababac");
    imprimirCicloDetalhado("(a|b)*abb", "ababb");

    Afn afn;
    ResultadoDaDeterminizacao det;
    ResultadoDaMinimizacao min;

    compilarCiclo("(a|b)*c", afn, det, min);
    conferir(afn.numEstados() == 10, "(a|b)*c: Thompson produz 10 estados");
    conferir(afn.numTransicoes() == 12 && afn.numVazias() == 9,
             "(a|b)*c: 12 transicoes sendo 9 vazias");
    conferir(det.estadosDoAfd == 5,
             "(a|b)*c: determinizacao produz 5 estados (incluindo conjunto vazio)");
    conferir(min.estadosDepois == 3,
             "(a|b)*c: minimizacao reduz para 3 estados ({D0,D1,D2}, {D3}, {D4})");
    conferir(min.blocosPorRodada.size() >= 2 &&
                 min.blocosPorRodada.back() ==
                     min.blocosPorRodada[min.blocosPorRodada.size() - 2],
             "(a|b)*c: refinamento termina no ponto fixo");

    compilarCiclo("(a|b)*abb", afn, det, min);
    conferir(afn.numEstados() == 14, "(a|b)*abb: Thompson produz 14 estados");
    conferir(det.estadosDoAfd == 5, "(a|b)*abb: determinizacao produz 5 estados alcancaveis");
    conferir(min.estadosDepois == 4, "(a|b)*abb: minimizacao reduz para 4 estados");

    compilarCiclo("-?[0-9]+", afn, det, min);
    conferir(afn.numEstados() == 84 && det.estadosDoAfd == 23 && min.estadosDepois == 4,
             "-?[0-9]+: AFN 84 -> AFD 23 -> minimo 4");

    compilarCiclo("a.c", afn, det, min);
    conferir(afn.numEstados() == 6 && det.estadosDoAfd == 5 && min.estadosDepois == 5,
             "a.c: AFN 6 -> AFD 5 -> minimo 5 (coringa expandido sobre alfabeto)");

    std::cout << "\n== Verificacao de estados inacessiveis e completude (M5-Sec 1.6) ==\n";
    Afd afdManual;
    afdManual.alfabeto = {'0', '1'};
    const Estado eA = afdManual.novoEstado("A");
    const Estado eB = afdManual.novoEstado("B");
    const Estado eC = afdManual.novoEstado("C");
    const Estado eD = afdManual.novoEstado("D");
    const Estado eE = afdManual.novoEstado("E");
    const Estado eInacessivel = afdManual.novoEstado("X");
    afdManual.inicial = eA;
    afdManual.finais  = {eD, eE};
    afdManual.adicionarTransicao(eA, '0', eB);
    afdManual.adicionarTransicao(eA, '1', eC);
    afdManual.adicionarTransicao(eB, '0', eD);
    afdManual.adicionarTransicao(eB, '1', eE);
    afdManual.adicionarTransicao(eC, '0', eD);
    afdManual.adicionarTransicao(eC, '1', eE);
    afdManual.adicionarTransicao(eD, '0', eD);
    afdManual.adicionarTransicao(eD, '1', eD);
    afdManual.adicionarTransicao(eE, '0', eE);
    afdManual.adicionarTransicao(eE, '1', eE);
    afdManual.adicionarTransicao(eInacessivel, '0', eD);
    afdManual.adicionarTransicao(eInacessivel, '1', eE);

    const ResultadoDaMinimizacao minManual = minimizar(afdManual);
    conferir(estadosAlcancaveis(afdManual).size() == 5,
             "estado inacessivel X descartado antes do refinamento");
    conferir(minManual.estadosDepois == 3,
             "AFD {A, B, C, D, E} + inacessivel reduz para 3 blocos: {A}, {B,C}, {D,E}");
    conferir(minManual.blocosPorRodada == std::vector<std::size_t>{2, 3, 3},
             "sequencia de contagens do exemplo classico e 2 -> 3 -> 3");

    std::cout << "\n== Padroes lexicos da Linguagem Xuxu (exemplo01.lex) ==\n";
    medirPadrao("xuxuVerde|xuxuMaduro|xuxu|talvez|ouEntao|fudeu",
                "amostra de palavras reservadas Xuxu");
    medirPadrao("[0-9]+\\.[0-9]+", "decimal Xuxu");
    medirPadrao("[0-9]+", "inteiro Xuxu");
    medirPadrao("=", "atribuicao Xuxu");
    medirPadrao("[a-zA-Z_][a-zA-Z0-9_]*", "identificador Xuxu");

    std::cout << "\n== Tarefa 3: Equivalencia por comparacao exaustiva sobre cadeias ==\n";
    {
        compilarCiclo("(a|b)*c", afn, det, min);
        const std::vector<std::string> cadeias4 = todasAsCadeias("abc", 4);
        std::size_t aceitasAfn = 0, aceitasAfd = 0, aceitasMin = 0;
        for (const std::string& c : cadeias4) {
            if (aceita(afn, c))     ++aceitasAfn;
            if (det.afd.aceita(c))  ++aceitasAfd;
            if (min.afd.aceita(c))  ++aceitasMin;
        }
        conferir(cadeias4.size() == 121 && aceitasAfn == 15 &&
                     aceitasAfd == 15 && aceitasMin == 15 &&
                     !compararSobre(afn, det.afd, cadeias4).houve &&
                     !compararSobre(det.afd, min.afd, cadeias4).houve,
                 "/(a|b)*c/ ate comprimento 4: 121 cadeias, as tres maquinas aceitam as mesmas 15 e recusam 106");
    }
    testarEquivalenciaExaustiva("(a|b)*c", 6);
    testarEquivalenciaExaustiva("(a|b)*abb", 6);
    testarEquivalenciaExaustiva("a(b|c)*d", 5);
    testarEquivalenciaExaustiva("a.c", 4);
    testarEquivalenciaExaustiva("talvez|fudeu", 4);

    std::cout << "\n== Tarefa 3: Explosao de estados construida de proposito ==\n";
    std::cout << "  linguagem: o n-esimo simbolo contado do fim e um 'a'\n\n";
    std::size_t ultimoMinimo = 0;
    for (std::size_t n = 1; n <= 8; ++n) {
        const std::string padrao = padraoDaExplosao(n);
        compilarCiclo(padrao, afn, det, min);
        ultimoMinimo = min.estadosDepois;
        std::cout << "  n = " << n
                  << "  AFN " << afn.numEstados() << " estados"
                  << "  ->  AFD " << det.estadosDoAfd
                  << "  ->  minimo " << min.estadosDepois << "\n";
    }
    std::cout << "\n  o minimo acompanha o AFD (2^n estados): a explosao e da LINGUAGEM, nao do algoritmo.\n";
    conferir(ultimoMinimo >= 256,
             "explosao em n = 8 atinge pelo menos 2^8 = 256 estados no AFD minimo");
}

int main(int argc, char** argv) {
    if (argc > 1) {
        const std::string arg1 = argv[1];
        if (arg1 != "determinizar" && arg1 != "afd") {
            const std::string padrao = arg1;
            const std::string cadeia = argc > 2 ? argv[2] : "";
            imprimirCicloDetalhado(padrao, cadeia);
            return verificacoesFalhas > 0 ? 1 : 0;
        }
    }

    verificacoesGuiadas();
    return verificacoesFalhas > 0 ? 1 : 0;
}
