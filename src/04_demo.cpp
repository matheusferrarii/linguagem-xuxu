#include "04_afn.h"

#include <fstream>
#include <iostream>
#include <string>

using namespace xuxu;

static int verificacoesFalhas = 0;

static void conferir(const bool condicao, const std::string& descricao) {
    std::cout << (condicao ? "  [ok]    " : "  [FALHA] ") << descricao << "\n";
    if (!condicao) ++verificacoesFalhas;
}

static bool compilar(const std::string& padrao, Afn& afn) {
    const Resultado r = analisarExpressao(padrao);
    if (!r.ok) return false;
    afn = construirAfn(r.arvore);
    return true;
}

static bool testaCadeia(const std::string& padrao, const std::string& cadeia) {
    Afn afn;
    return compilar(padrao, afn) && aceita(afn, cadeia);
}

static void verificacoes() {
    std::cout << "== Tarefa 1: construcao de Thompson ==\n";
    Afn afn;
    compilar("(a|b)*c", afn);
    conferir(afn.numEstados() == 10, "(a|b)*c tem 10 estados");
    conferir(afn.numTransicoes() == 12, "(a|b)*c tem 12 transicoes");
    conferir(afn.numVazias() == 9, "(a|b)*c tem 9 transicoes vazias");

    std::cout << "== Tarefa 1: fecho ==\n";
    conferir(testaCadeia("a*", ""),    "a* aceita a cadeia vazia");
    conferir(testaCadeia("a*", "a"),   "a* aceita uma repeticao");
    conferir(testaCadeia("a*", "aaa"), "a* aceita tres repeticoes");
    conferir(!testaCadeia("a*", "ab"), "a* recusa ab");

    std::cout << "== Tarefa 2: simulacao ==\n";
    compilar("a|b", afn);
    conferir(conjuntoInicial(afn).size() > 1,
             "conjunto inicial de a|b tem mais de um estado");
    conferir(testaCadeia("a**", "aaa") && !testaCadeia("a**", "b"),
             "a** termina (sem laco no fecho vazio)");

    std::cout << "== Demonstracao do traco da simulacao ==\n";
    compilar("(a|b)*c", afn);
    std::cout << "/(a|b)*c/ -> " << afn.numEstados() << " estados, "
              << afn.numTransicoes() << " transicoes ("
              << afn.numVazias() << " vazias)\n";
    const Simulacao sim = simular(afn, "abc");
    std::cout << formatarSimulacao("abc", sim);
    conferir(sim.aceitou, "(a|b)*c aceita abc");
}

static int rodarCasos(const std::string& caminho) {
    std::ifstream arquivo(caminho);
    if (!arquivo) {
        std::cerr << "nao foi possivel abrir " << caminho << "\n";
        return 2;
    }
    std::cout << "== Tarefa 3: bateria " << caminho << " ==\n";
    std::string linha;
    int numero = 0, total = 0, falhas = 0;
    while (std::getline(arquivo, linha)) {
        ++numero;
        if (!linha.empty() && linha.back() == '\r') linha.pop_back();
        if (linha.empty() || linha[0] == '#') continue;

        const std::size_t t1 = linha.find('\t');
        const std::size_t t2 = t1 == std::string::npos
                                   ? std::string::npos
                                   : linha.find('\t', t1 + 1);
        if (t2 == std::string::npos) {
            std::cout << "  " << caminho << ":" << numero
                      << ": linha mal formada\n";
            ++falhas;
            continue;
        }
        const std::string padrao   = linha.substr(0, t1);
        const std::string cadeia   = linha.substr(t1 + 1, t2 - t1 - 1);
        const std::string esperado = linha.substr(t2 + 1);

        ++total;
        Afn afn;
        std::string obtido = "erro-de-padrao";
        if (compilar(padrao, afn)) {
            obtido = aceita(afn, cadeia) ? "aceita" : "recusa";
        }
        if (obtido != esperado) {
            ++falhas;
            std::cout << "  " << caminho << ":" << numero
                      << ": esperado '" << esperado << "', obtido '"
                      << obtido << "' (padrao '" << padrao
                      << "', cadeia '" << cadeia << "')\n";
        }
    }
    std::cout << "  casos: " << total << ", falhas: " << falhas << "\n";
    return falhas > 0 ? 1 : 0;
}

int main(int argc, char** argv) {
    const std::string comando = argc > 1 ? argv[1] : "afn";
    if (comando != "afn") {
        std::cerr << "uso: 04_demo afn [arquivo_de_casos]\n";
        return 2;
    }
    const std::string caminho = argc > 2 ? argv[2] : "exemplos/04_casos.txt";
    verificacoes();
    const int codigo = rodarCasos(caminho);
    if (codigo != 0) return codigo;
    return verificacoesFalhas > 0 ? 1 : 0;
}
