#ifndef XUXU_02_REGEX_H
#define XUXU_02_REGEX_H

#include <cstddef>
#include <string>
#include <vector>

namespace xuxu {

inline constexpr std::size_t kSemFilho = static_cast<std::size_t>(-1);

enum class TipoDeNo {
    Simbolo,
    Qualquer,
    Vazio,
    Concatenacao,
    Alternancia,
    Fecho,
};

struct No {
    TipoDeNo tipo = TipoDeNo::Vazio;
    char simbolo = '\0';
    std::size_t esquerda = kSemFilho;
    std::size_t direita  = kSemFilho;
};

struct Arvore {
    std::vector<No> nos;
    std::size_t raiz = kSemFilho;

    bool vazia() const { return raiz == kSemFilho; }
};

struct ErroDeSintaxe {
    std::size_t posicao = 0;
    std::string mensagem;
};

struct Resultado {
    bool ok = false;
    Arvore arvore;
    ErroDeSintaxe erro;
};

Resultado analisarExpressao(const std::string& expressao);

std::string formatarArvore(const Arvore& arvore);

std::string formatarErro(const std::string& expressao, const ErroDeSintaxe& erro);

std::size_t tamanho(const Arvore& arvore);

}

#endif
