#include "02_regex.h"

#include <iostream>
#include <string>
#include <vector>

using namespace xuxu;

static void secao(const std::string& titulo) {
    std::cout << "\n" << std::string(60, '=') << "\n"
              << "  " << titulo << "\n"
              << std::string(60, '=') << "\n";
}

static void testarOk(const std::string& expressao,
                     const std::string& descricao) {
    const Resultado r = analisarExpressao(expressao);
    std::cout << "\n  " << descricao << "\n"
              << "  entrada : " << expressao << "\n";
    if (r.ok) {
        std::cout << "  arvore  : " << formatarArvore(r.arvore) << "\n"
                  << "  nos     : " << tamanho(r.arvore) << "\n";
    } else {
        std::cout << "  [INESPERADO] erro:\n"
                  << formatarErro(expressao, r.erro) << "\n";
    }
}

static void testarErro(const std::string& expressao,
                       const std::string& descricao) {
    const Resultado r = analisarExpressao(expressao);
    std::cout << "\n  " << descricao << "\n";
    if (!r.ok) {
        std::cout << "  recusado corretamente:\n"
                  << formatarErro(expressao, r.erro) << "\n";
    } else {
        std::cout << "  [INESPERADO] aceito: "
                  << formatarArvore(r.arvore) << "\n";
    }
}

static void testarConvergencia(const std::string& expr1,
                               const std::string& expr2,
                               const std::string& descricao) {
    const Resultado r1 = analisarExpressao(expr1);
    const Resultado r2 = analisarExpressao(expr2);
    std::cout << "\n  " << descricao << "\n"
              << "  expr 1: " << expr1 << "\n"
              << "  expr 2: " << expr2 << "\n";
    if (!r1.ok || !r2.ok) {
        std::cout << "  [ERRO DE PARSE]\n";
        return;
    }
    const std::string a1 = formatarArvore(r1.arvore);
    const std::string a2 = formatarArvore(r2.arvore);
    if (a1 == a2) {
        std::cout << "  convergem: " << a1 << "\n";
    } else {
        std::cout << "  [NAO CONVERGEM]\n"
                  << "    arvore 1: " << a1 << "\n"
                  << "    arvore 2: " << a2 << "\n";
    }
}

int main() {
    secao("1. Padroes do exemplo01.lex reduzidos ao nucleo");

    testarOk("[a-zA-Z_][a-zA-Z0-9_]*", "identificador");
    testarOk("[0-9]+",                  "inteiro (fecho positivo)");
    testarOk("[0-9]+\\.[0-9]+",         "decimal (ponto escapado)");
    testarOk("=",                       "atribuicao");

    testarOk("xuxuVerde|xuxuMaduro|xuxu", "palavras reservadas (amostra)");

    secao("2. Convergencia de notacoes (M2-T2)");

    testarConvergencia("a+",   "aa*",  "fecho positivo x concatenacao com fecho");
    testarConvergencia("[ab]", "a|b",  "classe de dois simbolos x alternancia");
    testarConvergencia("[a-c]", "a|b|c", "faixa x enumeracao");
    testarConvergencia("(a)b", "ab",   "grupo redundante x sem grupo");

    secao("3. Custo das reducoes (numero de nos)");

    struct { const char* expr; const char* desc; } custos[] = {
        { "a",                 "um simbolo literal" },
        { "a*",                "fecho" },
        { "a+",                "fecho positivo (clona a subarvore)" },
        { "a?",                "opcional" },
        { "[abc]",             "classe de tres simbolos" },
        { "[a-z]",             "faixa de 26 simbolos" },
        { "[a-zA-Z_]",         "primeira parte do identificador" },
        { "[a-zA-Z0-9_]*",     "segunda parte do identificador" },
        { "[a-zA-Z_][a-zA-Z0-9_]*", "identificador completo" },
    };
    for (const auto& c : custos) {
        const Resultado r = analisarExpressao(c.expr);
        std::cout << "\n  " << c.desc << "\n"
                  << "  entrada : " << c.expr << "\n";
        if (r.ok) {
            std::cout << "  nos     : " << tamanho(r.arvore) << "\n";
        } else {
            std::cout << "  [ERRO]\n";
        }
    }

    secao("4. Recusa com posicao (M2-T4)");

    testarErro("a(",     "parentese sem fechamento");
    testarErro("*ab",    "operador de repeticao sem expressao anterior");
    testarErro("ab)c",   "fecha-parenteses sem abertura");
    testarErro("a\\",    "barra invertida sem simbolo");
    testarErro("[]",     "classe de caracteres vazia");
    testarErro("|abc",   "alternancia sem lado esquerdo");

    std::cout << "\n" << std::string(60, '=') << "\n"
              << "  Concluido sem erros fatais.\n"
              << std::string(60, '=') << "\n";
    return 0;
}
