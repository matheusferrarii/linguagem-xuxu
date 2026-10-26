#include "02_regex.h"

#include <sstream>

namespace xuxu {

namespace {

class Analisador {
public:
    explicit Analisador(const std::string& texto) : texto_(texto) {}

    Resultado analisar() {
        Resultado resultado;
        const std::size_t raiz = alternancia();
        if (falhou_) {
            resultado.ok = false;
            resultado.erro = erro_;
            return resultado;
        }
        if (posicao_ != texto_.size()) {
            return falhar("simbolo inesperado apos o fim da expressao");
        }
        resultado.ok = true;
        resultado.arvore.nos  = nos_;
        resultado.arvore.raiz = raiz;
        return resultado;
    }

private:
    std::size_t novoFolha(const TipoDeNo tipo, const char simbolo) {
        No no;
        no.tipo    = tipo;
        no.simbolo = simbolo;
        nos_.push_back(no);
        return nos_.size() - 1;
    }

    std::size_t novoBinario(const TipoDeNo tipo,
                            const std::size_t esquerda,
                            const std::size_t direita) {
        No no;
        no.tipo     = tipo;
        no.esquerda = esquerda;
        no.direita  = direita;
        nos_.push_back(no);
        return nos_.size() - 1;
    }

    std::size_t novoFecho(const std::size_t filho) {
        No no;
        no.tipo     = TipoDeNo::Fecho;
        no.esquerda = filho;
        nos_.push_back(no);
        return nos_.size() - 1;
    }

    std::size_t clonar(const std::size_t origem) {
        const No& modelo = nos_[origem];
        No copia;
        copia.tipo    = modelo.tipo;
        copia.simbolo = modelo.simbolo;
        const std::size_t esquerdaOriginal = modelo.esquerda;
        const std::size_t direitaOriginal  = modelo.direita;
        copia.esquerda = esquerdaOriginal == kSemFilho
                             ? kSemFilho
                             : clonar(esquerdaOriginal);
        copia.direita  = direitaOriginal == kSemFilho
                             ? kSemFilho
                             : clonar(direitaOriginal);
        nos_.push_back(copia);
        return nos_.size() - 1;
    }

    bool fim() const { return posicao_ >= texto_.size(); }
    char atual() const { return texto_[posicao_]; }

    Resultado falhar(const std::string& mensagem) {
        Resultado resultado;
        resultado.ok            = false;
        resultado.erro.posicao  = posicao_;
        resultado.erro.mensagem = mensagem;
        return resultado;
    }

    std::size_t erroEm(const std::string& mensagem) {
        if (!falhou_) {
            falhou_          = true;
            erro_.posicao    = posicao_;
            erro_.mensagem   = mensagem;
        }
        return kSemFilho;
    }

    std::size_t alternancia() {
        std::size_t esquerda = concatenacao();
        if (falhou_) return kSemFilho;
        while (!fim() && atual() == '|') {
            ++posicao_;
            const std::size_t direita = concatenacao();
            if (falhou_) return kSemFilho;
            esquerda = novoBinario(TipoDeNo::Alternancia, esquerda, direita);
        }
        return esquerda;
    }

    std::size_t concatenacao() {
        if (fim() || atual() == '|' || atual() == ')') {
            return erroEm("esperava uma expressao aqui");
        }
        std::size_t esquerda = repeticao();
        if (falhou_) return kSemFilho;
        while (!fim() && atual() != '|' && atual() != ')') {
            const std::size_t direita = repeticao();
            if (falhou_) return kSemFilho;
            esquerda = novoBinario(TipoDeNo::Concatenacao, esquerda, direita);
        }
        return esquerda;
    }

    std::size_t repeticao() {
        std::size_t no = atomo();
        if (falhou_) return kSemFilho;
        while (!fim() && (atual() == '*' || atual() == '+' || atual() == '?')) {
            const char sufixo = atual();
            ++posicao_;
            if (sufixo == '*') {
                no = novoFecho(no);
            } else if (sufixo == '+') {
                const std::size_t copia = clonar(no);
                no = novoBinario(TipoDeNo::Concatenacao, no, novoFecho(copia));
            } else {
                no = novoBinario(TipoDeNo::Alternancia, no,
                                 novoFolha(TipoDeNo::Vazio, '\0'));
            }
        }
        return no;
    }

    std::size_t atomo() {
        if (fim()) return erroEm("expressao terminou antes do esperado");

        const char simbolo = atual();

        if (simbolo == '(') {
            ++posicao_;
            const std::size_t interno = alternancia();
            if (falhou_) return kSemFilho;
            if (fim() || atual() != ')') {
                return erroEm("falta o fecha-parenteses do grupo");
            }
            ++posicao_;
            return interno;
        }

        if (simbolo == '[') return classe();

        if (simbolo == '\\') {
            ++posicao_;
            if (fim()) return erroEm("expressao terminou apos a barra invertida");
            const char literal = atual();
            ++posicao_;
            return novoFolha(TipoDeNo::Simbolo, literal);
        }

        if (simbolo == '.') {
            ++posicao_;
            return novoFolha(TipoDeNo::Qualquer, '\0');
        }

        if (simbolo == '*' || simbolo == '+' || simbolo == '?' || simbolo == ')') {
            return erroEm("operador sem expressao a que se aplicar");
        }

        ++posicao_;
        return novoFolha(TipoDeNo::Simbolo, simbolo);
    }

    std::size_t classe() {
        ++posicao_;
        if (fim() || atual() == ']') return erroEm("classe de caracteres vazia");

        std::vector<char> simbolos;

        while (!fim() && atual() != ']') {
            char c = atual();

            if (c == '\\') {
                ++posicao_;
                if (fim()) return erroEm("expressao terminou dentro de classe de caracteres");
                simbolos.push_back(atual());
                ++posicao_;
                continue;
            }

            ++posicao_;

            if (!fim() && atual() == '-') {
                const std::size_t proximo = posicao_ + 1;
                if (proximo < texto_.size() && texto_[proximo] != ']') {
                    ++posicao_;
                    const char fim_ch = atual();
                    ++posicao_;
                    if (static_cast<unsigned char>(c) >
                        static_cast<unsigned char>(fim_ch)) {
                        return erroEm("intervalo invertido na classe de caracteres");
                    }
                    for (char code = c; code <= fim_ch; ++code) {
                        simbolos.push_back(code);
                    }
                    continue;
                }
                simbolos.push_back(c);
                simbolos.push_back('-');
                ++posicao_;
                continue;
            }

            simbolos.push_back(c);
        }

        if (fim()) return erroEm("classe de caracteres nao fechada");
        ++posicao_;

        if (simbolos.empty()) return erroEm("classe de caracteres vazia");

        std::size_t raiz = novoFolha(TipoDeNo::Simbolo, simbolos[0]);
        for (std::size_t i = 1; i < simbolos.size(); ++i) {
            raiz = novoBinario(TipoDeNo::Alternancia, raiz,
                               novoFolha(TipoDeNo::Simbolo, simbolos[i]));
        }
        return raiz;
    }

    const std::string& texto_;
    std::size_t posicao_ = 0;
    std::vector<No> nos_;
    bool falhou_       = false;
    ErroDeSintaxe erro_;
};

void escreverPrefixa(const Arvore& arvore, std::size_t idx, std::string& saida) {
    if (idx == kSemFilho) { saida += "<sem-filho>"; return; }
    const No& no = arvore.nos[idx];
    switch (no.tipo) {
        case TipoDeNo::Simbolo: {
            saida += "s:";
            if (no.simbolo == '\'' || no.simbolo == '\\') saida += '\\';
            saida += '\'';
            saida += no.simbolo;
            saida += '\'';
            break;
        }
        case TipoDeNo::Qualquer:
            saida += "qualquer";
            break;
        case TipoDeNo::Vazio:
            saida += "vazio";
            break;
        case TipoDeNo::Fecho:
            saida += "fecho(";
            escreverPrefixa(arvore, no.esquerda, saida);
            saida += ")";
            break;
        case TipoDeNo::Concatenacao:
            saida += "concat(";
            escreverPrefixa(arvore, no.esquerda, saida);
            saida += ",";
            escreverPrefixa(arvore, no.direita, saida);
            saida += ")";
            break;
        case TipoDeNo::Alternancia:
            saida += "alt(";
            escreverPrefixa(arvore, no.esquerda, saida);
            saida += ",";
            escreverPrefixa(arvore, no.direita, saida);
            saida += ")";
            break;
    }
}

}

Resultado analisarExpressao(const std::string& expressao) {
    return Analisador(expressao).analisar();
}

std::string formatarArvore(const Arvore& arvore) {
    if (arvore.vazia()) return "(arvore vazia)";
    std::string saida;
    escreverPrefixa(arvore, arvore.raiz, saida);
    return saida;
}

std::string formatarErro(const std::string& expressao,
                          const ErroDeSintaxe& erro) {
    std::string saida = "  " + expressao + "\n";
    saida += "  ";
    for (std::size_t i = 0; i < erro.posicao && i < expressao.size(); ++i) {
        saida += ' ';
    }
    saida += "^ ";
    saida += erro.mensagem;
    saida += " (posicao " + std::to_string(erro.posicao) + ")";
    return saida;
}

std::size_t tamanho(const Arvore& arvore) {
    return arvore.nos.size();
}

}
