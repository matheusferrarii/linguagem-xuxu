#include "04_afn.h"

namespace xuxu {

namespace {

struct Bloco {
    std::size_t entrada;
    std::size_t saida;
};

class Construtor {
public:
    Construtor(const Arvore& arvore, Afn& afn) : arvore_(arvore), afn_(afn) {}

    Bloco construir(const std::size_t indice) {
        const No& no = arvore_.nos[indice];
        switch (no.tipo) {
            case TipoDeNo::Simbolo:
                return folha(static_cast<unsigned char>(no.simbolo));
            case TipoDeNo::Qualquer:
                return folha(kQualquer);
            case TipoDeNo::Vazio:
                return folha(kEpsilon);
            case TipoDeNo::Concatenacao: {
                const Bloco a = construir(no.esquerda);
                const Bloco b = construir(no.direita);
                ligar(a.saida, b.entrada, kEpsilon);
                return {a.entrada, b.saida};
            }
            case TipoDeNo::Alternancia: {
                const Bloco a = construir(no.esquerda);
                const Bloco b = construir(no.direita);
                const std::size_t entrada = novoEstado();
                const std::size_t saida   = novoEstado();
                ligar(entrada, a.entrada, kEpsilon);
                ligar(entrada, b.entrada, kEpsilon);
                ligar(a.saida, saida, kEpsilon);
                ligar(b.saida, saida, kEpsilon);
                return {entrada, saida};
            }
            case TipoDeNo::Fecho: {
                const Bloco a = construir(no.esquerda);
                const std::size_t entrada = novoEstado();
                const std::size_t saida   = novoEstado();
                ligar(entrada, a.entrada, kEpsilon);
                ligar(entrada, saida, kEpsilon);
                ligar(a.saida, a.entrada, kEpsilon);
                ligar(a.saida, saida, kEpsilon);
                return {entrada, saida};
            }
        }
        return {0, 0};
    }

private:
    std::size_t novoEstado() {
        afn_.saindo.emplace_back();
        return afn_.saindo.size() - 1;
    }

    void ligar(const std::size_t origem, const std::size_t destino,
               const int rotulo) {
        afn_.saindo[origem].push_back({destino, rotulo});
    }

    Bloco folha(const int rotulo) {
        const std::size_t entrada = novoEstado();
        const std::size_t saida   = novoEstado();
        ligar(entrada, saida, rotulo);
        return {entrada, saida};
    }

    const Arvore& arvore_;
    Afn& afn_;
};

void fechoVazio(const Afn& afn, std::vector<std::size_t>& ativos,
                std::vector<char>& marcas) {
    std::vector<std::size_t> pilha(ativos.begin(), ativos.end());
    while (!pilha.empty()) {
        const std::size_t estado = pilha.back();
        pilha.pop_back();
        for (const Transicao& t : afn.saindo[estado]) {
            if (t.rotulo == kEpsilon && !marcas[t.destino]) {
                marcas[t.destino] = 1;
                ativos.push_back(t.destino);
                pilha.push_back(t.destino);
            }
        }
    }
}

}

std::size_t Afn::numTransicoes() const {
    std::size_t total = 0;
    for (const auto& lista : saindo) total += lista.size();
    return total;
}

std::size_t Afn::numVazias() const {
    std::size_t total = 0;
    for (const auto& lista : saindo) {
        for (const Transicao& t : lista) {
            if (t.rotulo == kEpsilon) ++total;
        }
    }
    return total;
}

Afn construirAfn(const Arvore& arvore) {
    Afn afn;
    if (arvore.vazia()) {
        afn.saindo.emplace_back();
        return afn;
    }
    Construtor construtor(arvore, afn);
    const Bloco bloco = construtor.construir(arvore.raiz);
    afn.inicial   = bloco.entrada;
    afn.aceitacao = bloco.saida;
    return afn;
}

std::vector<std::size_t> conjuntoInicial(const Afn& afn) {
    std::vector<char> marcas(afn.numEstados(), 0);
    std::vector<std::size_t> ativos{afn.inicial};
    marcas[afn.inicial] = 1;
    fechoVazio(afn, ativos, marcas);
    return ativos;
}

Simulacao simular(const Afn& afn, const std::string& cadeia) {
    Simulacao simulacao;
    std::vector<std::size_t> ativos = conjuntoInicial(afn);
    simulacao.passos.push_back(PassoDaSimulacao{'\0', ativos});
    simulacao.maiorConjuntoAtivo = ativos.size();

    for (const char c : cadeia) {
        if (ativos.empty()) {
            break;
        }
        const int simbolo = static_cast<unsigned char>(c);
        std::vector<char> marcas(afn.numEstados(), 0);
        std::vector<std::size_t> novos;
        for (const std::size_t estado : ativos) {
            for (const Transicao& t : afn.saindo[estado]) {
                if ((t.rotulo == simbolo || t.rotulo == kQualquer) &&
                    !marcas[t.destino]) {
                    marcas[t.destino] = 1;
                    novos.push_back(t.destino);
                }
            }
        }
        fechoVazio(afn, novos, marcas);
        ativos = std::move(novos);
        simulacao.passos.push_back(PassoDaSimulacao{c, ativos});
        if (ativos.size() > simulacao.maiorConjuntoAtivo) {
            simulacao.maiorConjuntoAtivo = ativos.size();
        }
    }

    for (const std::size_t estado : ativos) {
        if (estado == afn.aceitacao) {
            simulacao.aceitou = true;
            break;
        }
    }
    return simulacao;
}

std::string formatarSimulacao(const std::string& cadeia, const Simulacao& simulacao) {
    std::string texto = "  pattern sobre \"" + cadeia + "\"\n";
    for (const PassoDaSimulacao& passo : simulacao.passos) {
        texto += "    ";
        if (passo.simboloLido == '\0') {
            texto += "inicio ";
        } else {
            texto += "apos '";
            texto += passo.simboloLido;
            texto += "' ";
        }
        texto += "-> { ";
        for (std::size_t i = 0; i < passo.ativos.size(); ++i) {
            if (i > 0) {
                texto += ", ";
            }
            texto += std::to_string(passo.ativos[i]);
        }
        texto += " }\n";
    }
    texto += std::string("  resultado: ") + (simulacao.aceitou ? "ACEITA" : "RECUSA");
    texto += " | maior conjunto ativo: " + std::to_string(simulacao.maiorConjuntoAtivo) + "\n";
    return texto;
}

bool aceita(const Afn& afn, const std::string& cadeia) {
    return simular(afn, cadeia).aceitou;
}

}
