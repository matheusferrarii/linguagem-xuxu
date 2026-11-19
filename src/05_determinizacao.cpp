#include "05_determinizacao.h"

#include <algorithm>
#include <queue>
#include <stdexcept>
#include <utility>

namespace xuxu {

namespace {

bool contemFinal(const Afn& afn, const std::vector<Estado>& estados) {
    for (const Estado estado : estados) {
        if (estado == afn.aceitacao) {
            return true;
        }
    }
    return false;
}

std::string formatarConjunto(const std::vector<Estado>& conjunto) {
    if (conjunto.empty()) {
        return "{}";
    }
    std::string nome = "{";
    for (std::size_t i = 0; i < conjunto.size(); ++i) {
        if (i > 0) {
            nome += ",";
        }
        nome += std::to_string(conjunto[i]);
    }
    nome += "}";
    return nome;
}

using Grupo    = std::set<Estado>;
using Particao = std::vector<Grupo>;

std::size_t grupoDe(const Particao& particao, const Estado estado) {
    for (std::size_t i = 0; i < particao.size(); ++i) {
        if (particao[i].contains(estado)) {
            return i;
        }
    }
    throw std::logic_error("Estado nao encontrado na particao");
}

std::vector<std::size_t> assinatura(const Afd& afd, const Particao& particao,
                                    const Estado estado) {
    std::vector<std::size_t> resultado;
    for (const char simbolo : afd.alfabeto) {
        const Estado destino = afd.transicoes.at(estado).at(simbolo);
        resultado.push_back(grupoDe(particao, destino));
    }
    return resultado;
}

Particao refinarParticao(const Afd& afd, const Particao& atual) {
    Particao nova;
    for (const Grupo& grupo : atual) {
        std::map<std::vector<std::size_t>, Grupo> subdivisoes;
        for (const Estado estado : grupo) {
            const auto chave = assinatura(afd, atual, estado);
            subdivisoes[chave].insert(estado);
        }
        for (const auto& [chave, subgrupo] : subdivisoes) {
            nova.push_back(subgrupo);
        }
    }
    return nova;
}

Particao particaoInicial(const Afd& afd, const std::set<Estado>& alcancaveis) {
    Grupo finais;
    Grupo naoFinais;
    for (const Estado estado : alcancaveis) {
        if (afd.finais.contains(estado)) {
            finais.insert(estado);
        } else {
            naoFinais.insert(estado);
        }
    }
    Particao resultado;
    if (!naoFinais.empty()) resultado.push_back(naoFinais);
    if (!finais.empty())    resultado.push_back(finais);
    return resultado;
}

}

std::size_t Afd::numTransicoes() const {
    std::size_t total = 0;
    for (const auto& mapa : transicoes) {
        total += mapa.size();
    }
    return total;
}

bool Afd::aceita(const std::string& cadeia) const {
    if (transicoes.empty()) {
        return false;
    }
    Estado atual = inicial;
    for (const char simbolo : cadeia) {
        const auto it = transicoes.at(atual).find(simbolo);
        if (it == transicoes.at(atual).end()) {
            return false;
        }
        atual = it->second;
    }
    return finais.contains(atual);
}

std::string Afd::formatarTabela() const {
    std::string saida;
    saida += "    estado      ";
    for (const char c : alfabeto) {
        saida += " | '";
        saida += c;
        saida += "'";
    }
    saida += " | aceita\n";

    for (Estado s = 0; s < transicoes.size(); ++s) {
        std::string rotulo = (s == inicial ? "-> " : "   ");
        rotulo += "D" + std::to_string(s);
        if (s < nomes.size() && nomes[s] != ("D" + std::to_string(s))) {
            rotulo += " " + nomes[s];
        }
        while (rotulo.size() < 16) rotulo += ' ';
        saida += rotulo;
        for (const char c : alfabeto) {
            saida += " | ";
            const auto it = transicoes[s].find(c);
            if (it != transicoes[s].end()) {
                std::string dest = "D" + std::to_string(it->second);
                while (dest.size() < 3) dest += ' ';
                saida += dest;
            } else {
                saida += "-  ";
            }
        }
        saida += " | ";
        saida += finais.contains(s) ? "sim" : "nao";
        saida += "\n";
    }
    return saida;
}

std::string alfabetoDoAfn(const Afn& afn) {
    std::vector<char> vistos(256, 0);
    std::string alfabeto;
    for (const auto& lista : afn.saindo) {
        for (const Transicao& t : lista) {
            if (t.rotulo == kEpsilon || t.rotulo == kQualquer) {
                continue;
            }
            const std::size_t byte = static_cast<unsigned char>(t.rotulo);
            if (!vistos[byte]) {
                vistos[byte] = 1;
                alfabeto += static_cast<char>(t.rotulo);
            }
        }
    }
    std::sort(alfabeto.begin(), alfabeto.end());
    return alfabeto;
}

std::vector<Estado> mover(const Afn& afn, const std::vector<Estado>& estados,
                          const char simbolo) {
    const int codigo = static_cast<unsigned char>(simbolo);
    std::vector<Estado> resultado;
    for (const Estado estado : estados) {
        for (const Transicao& t : afn.saindo.at(estado)) {
            if (t.rotulo == codigo || t.rotulo == kQualquer) {
                resultado.push_back(t.destino);
            }
        }
    }
    std::sort(resultado.begin(), resultado.end());
    resultado.erase(std::unique(resultado.begin(), resultado.end()),
                    resultado.end());
    return resultado;
}

std::vector<Estado> moverEFechar(const Afn& afn,
                                 const std::vector<Estado>& estados,
                                 const char simbolo) {
    std::vector<Estado> resultado = fechoVazio(afn, mover(afn, estados, simbolo));
    std::sort(resultado.begin(), resultado.end());
    resultado.erase(std::unique(resultado.begin(), resultado.end()),
                    resultado.end());
    return resultado;
}

ResultadoDaDeterminizacao determinizar(const Afn& afn,
                                       const std::string& alfabeto) {
    ResultadoDaDeterminizacao resultado;
    resultado.estadosDoAfn = afn.numEstados();

    Afd& afd = resultado.afd;
    for (const char c : alfabeto) {
        afd.alfabeto.insert(c);
    }

    using Conjunto = std::vector<Estado>;
    std::map<Conjunto, Estado> catalogo;
    std::vector<Conjunto> pendentes;

    Conjunto inicial = conjuntoInicial(afn);
    std::sort(inicial.begin(), inicial.end());
    inicial.erase(std::unique(inicial.begin(), inicial.end()), inicial.end());

    const Estado idInicial = afd.novoEstado(formatarConjunto(inicial));
    catalogo[inicial] = idInicial;
    afd.inicial = idInicial;
    resultado.subconjuntos.push_back(inicial);
    pendentes.push_back(inicial);

    if (contemFinal(afn, inicial)) {
        afd.finais.insert(idInicial);
    }

    while (!pendentes.empty()) {
        const Conjunto atual = pendentes.back();
        pendentes.pop_back();

        const Estado origem = catalogo.at(atual);

        for (const char simbolo : afd.alfabeto) {
            const Conjunto destino = moverEFechar(afn, atual, simbolo);

            const auto encontrado = catalogo.find(destino);
            Estado idDestino = 0;

            if (encontrado == catalogo.end()) {
                idDestino = afd.novoEstado(formatarConjunto(destino));
                catalogo[destino] = idDestino;
                resultado.subconjuntos.push_back(destino);
                pendentes.push_back(destino);

                if (destino.empty()) {
                    afd.erro    = idDestino;
                    afd.temErro = true;
                }
                if (contemFinal(afn, destino)) {
                    afd.finais.insert(idDestino);
                }
            } else {
                idDestino = encontrado->second;
            }

            afd.adicionarTransicao(origem, simbolo, idDestino);
        }
    }

    resultado.estadosDoAfd = afd.numEstados();
    return resultado;
}

ResultadoDaDeterminizacao determinizar(const Afn& afn) {
    return determinizar(afn, alfabetoDoAfn(afn));
}

void completarAfd(Afd& afd) {
    bool faltaTransicao = false;
    for (Estado estado = 0; estado < afd.transicoes.size(); ++estado) {
        for (const char simbolo : afd.alfabeto) {
            if (!afd.transicoes[estado].contains(simbolo)) {
                faltaTransicao = true;
            }
        }
    }
    if (!faltaTransicao) {
        return;
    }

    const Estado erro = afd.novoEstado("{}");
    afd.erro    = erro;
    afd.temErro = true;

    for (Estado estado = 0; estado <= erro; ++estado) {
        for (const char simbolo : afd.alfabeto) {
            if (!afd.transicoes[estado].contains(simbolo)) {
                afd.adicionarTransicao(estado, simbolo, erro);
            }
        }
    }
}

std::set<Estado> estadosAlcancaveis(const Afd& afd) {
    std::set<Estado> visitados;
    if (afd.transicoes.empty()) {
        return visitados;
    }
    std::queue<Estado> fila;
    visitados.insert(afd.inicial);
    fila.push(afd.inicial);

    while (!fila.empty()) {
        const Estado atual = fila.front();
        fila.pop();
        for (const auto& [simbolo, destino] : afd.transicoes.at(atual)) {
            if (visitados.insert(destino).second) {
                fila.push(destino);
            }
        }
    }
    return visitados;
}

ResultadoDaMinimizacao minimizar(const Afd& original) {
    ResultadoDaMinimizacao resultado;
    resultado.estadosAntes = original.numEstados();

    Afd afd = original;
    completarAfd(afd);

    const std::set<Estado> alcancaveis = estadosAlcancaveis(afd);
    Particao particao = particaoInicial(afd, alcancaveis);
    resultado.blocosPorRodada.push_back(particao.size());

    while (true) {
        Particao nova = refinarParticao(afd, particao);
        resultado.blocosPorRodada.push_back(nova.size());
        if (nova.size() == particao.size()) {
            break;
        }
        particao = std::move(nova);
    }

    Afd& minimo = resultado.afd;
    minimo.alfabeto = afd.alfabeto;

    std::vector<Estado> representante(afd.transicoes.size(), 0);

    for (const Grupo& grupo : particao) {
        std::vector<Estado> elementos(grupo.begin(), grupo.end());
        resultado.grupos.push_back(elementos);

        std::string nome = "[";
        for (std::size_t i = 0; i < elementos.size(); ++i) {
            if (i > 0) nome += ",";
            nome += "D" + std::to_string(elementos[i]);
        }
        nome += "]";

        const Estado novoId = minimo.novoEstado(nome);
        for (const Estado estado : grupo) {
            representante[estado] = novoId;
        }
    }

    if (!particao.empty()) {
        minimo.inicial = representante[afd.inicial];
    }

    for (const Estado estado : afd.finais) {
        if (alcancaveis.contains(estado)) {
            minimo.finais.insert(representante[estado]);
        }
    }

    if (afd.temErro && alcancaveis.contains(afd.erro)) {
        minimo.erro    = representante[afd.erro];
        minimo.temErro = true;
    }

    for (const Grupo& grupo : particao) {
        const Estado origemAntiga = *grupo.begin();
        const Estado origemNova   = representante[origemAntiga];
        for (const char simbolo : afd.alfabeto) {
            const Estado destinoAntigo =
                afd.transicoes[origemAntiga].at(simbolo);
            minimo.adicionarTransicao(origemNova, simbolo,
                                      representante[destinoAntigo]);
        }
    }

    resultado.estadosDepois = minimo.numEstados();
    return resultado;
}

bool aceita(const Afd& afd, const std::string& cadeia) {
    return afd.aceita(cadeia);
}

std::vector<std::string> todasAsCadeias(const std::string& alfabeto,
                                        const std::size_t comprimentoMaximo) {
    std::vector<std::string> cadeias{std::string{}};
    std::vector<std::string> nivel{std::string{}};
    for (std::size_t comp = 0; comp < comprimentoMaximo; ++comp) {
        std::vector<std::string> proximo;
        for (const std::string& base : nivel) {
            for (const char simbolo : alfabeto) {
                proximo.push_back(base + simbolo);
            }
        }
        cadeias.insert(cadeias.end(), proximo.begin(), proximo.end());
        nivel = std::move(proximo);
    }
    return cadeias;
}

Discordancia compararSobre(const Afd& primeira, const Afd& segunda,
                           const std::vector<std::string>& cadeias) {
    Discordancia discordancia;
    for (const std::string& cadeia : cadeias) {
        const bool a = primeira.aceita(cadeia);
        const bool b = segunda.aceita(cadeia);
        if (a != b) {
            discordancia.houve           = true;
            discordancia.cadeia          = cadeia;
            discordancia.primeiraAceitou = a;
            discordancia.segundaAceitou  = b;
            return discordancia;
        }
    }
    return discordancia;
}

Discordancia compararSobre(const Afn& afn, const Afd& afd,
                           const std::vector<std::string>& cadeias) {
    Discordancia discordancia;
    for (const std::string& cadeia : cadeias) {
        const bool a = aceita(afn, cadeia);
        const bool b = afd.aceita(cadeia);
        if (a != b) {
            discordancia.houve           = true;
            discordancia.cadeia          = cadeia;
            discordancia.primeiraAceitou = a;
            discordancia.segundaAceitou  = b;
            return discordancia;
        }
    }
    return discordancia;
}

}
