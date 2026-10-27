#ifndef XUXU_04_AFN_H
#define XUXU_04_AFN_H

#include "02_regex.h"

#include <cstddef>
#include <string>
#include <vector>

namespace xuxu {

inline constexpr int kEpsilon  = -1;
inline constexpr int kQualquer = -2;

struct Transicao {
    std::size_t destino = 0;
    int rotulo = kEpsilon;
};

struct Afn {
    std::vector<std::vector<Transicao>> saindo;
    std::size_t inicial   = 0;
    std::size_t aceitacao = 0;

    std::size_t numEstados() const { return saindo.size(); }
    std::size_t numTransicoes() const;
    std::size_t numVazias() const;
};

struct PassoDaSimulacao {
    char simboloLido = '\0';
    std::vector<std::size_t> ativos;
};

struct Simulacao {
    bool aceitou = false;
    std::vector<PassoDaSimulacao> passos;
    std::size_t maiorConjuntoAtivo = 0;
};

Afn construirAfn(const Arvore& arvore);

std::vector<std::size_t> conjuntoInicial(const Afn& afn);

Simulacao simular(const Afn& afn, const std::string& cadeia);

std::string formatarSimulacao(const std::string& cadeia, const Simulacao& simulacao);

bool aceita(const Afn& afn, const std::string& cadeia);

}

#endif
