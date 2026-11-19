#ifndef XUXU_05_DETERMINIZACAO_H
#define XUXU_05_DETERMINIZACAO_H

#include "04_afn.h"

#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace xuxu {

using Estado = std::size_t;

struct Afd {
    std::vector<std::map<char, Estado>> transicoes;
    Estado inicial = 0;
    std::set<Estado> finais;
    std::set<char> alfabeto;
    std::vector<std::string> nomes;
    Estado erro = 0;
    bool temErro = false;

    Estado novoEstado(const std::string& nome = "") {
        const Estado id = transicoes.size();
        transicoes.emplace_back();
        nomes.push_back(nome.empty() ? "D" + std::to_string(id) : nome);
        return id;
    }

    void adicionarTransicao(const Estado origem, const char simbolo,
                            const Estado destino) {
        transicoes.at(origem)[simbolo] = destino;
    }

    std::size_t numEstados() const { return transicoes.size(); }
    std::size_t numTransicoes() const;
    bool ehFinal(const Estado estado) const { return finais.contains(estado); }
    bool aceita(const std::string& cadeia) const;
    std::string formatarTabela() const;
};

struct ResultadoDaDeterminizacao {
    Afd afd;
    std::size_t estadosDoAfn = 0;
    std::size_t estadosDoAfd = 0;
    std::vector<std::vector<Estado>> subconjuntos;
};

struct ResultadoDaMinimizacao {
    Afd afd;
    std::size_t estadosAntes  = 0;
    std::size_t estadosDepois = 0;
    std::vector<std::size_t> blocosPorRodada;
    std::vector<std::vector<Estado>> grupos;
};

struct Discordancia {
    bool houve = false;
    std::string cadeia;
    bool primeiraAceitou = false;
    bool segundaAceitou  = false;
};

std::string alfabetoDoAfn(const Afn& afn);

std::vector<Estado> mover(const Afn& afn, const std::vector<Estado>& estados,
                          char simbolo);

std::vector<Estado> moverEFechar(const Afn& afn,
                                 const std::vector<Estado>& estados,
                                 char simbolo);

ResultadoDaDeterminizacao determinizar(const Afn& afn,
                                       const std::string& alfabeto);
ResultadoDaDeterminizacao determinizar(const Afn& afn);

void completarAfd(Afd& afd);

std::set<Estado> estadosAlcancaveis(const Afd& afd);

ResultadoDaMinimizacao minimizar(const Afd& afd);

bool aceita(const Afd& afd, const std::string& cadeia);

std::vector<std::string> todasAsCadeias(const std::string& alfabeto,
                                        std::size_t comprimentoMaximo);

Discordancia compararSobre(const Afd& primeira, const Afd& segunda,
                           const std::vector<std::string>& cadeias);

Discordancia compararSobre(const Afn& afn, const Afd& afd,
                           const std::vector<std::string>& cadeias);

}

#endif
