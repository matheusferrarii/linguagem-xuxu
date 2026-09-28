#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <cctype>

class AFD {
private:
    // Q: conjunto de estados
    std::unordered_set<std::string> estados;

    // Σ: alfabeto
    std::unordered_set<char> alfabeto;

    // δ: função de transição
    std::unordered_map<
        std::string,
        std::unordered_map<char, std::string>
    > transicoes;

    // q₀: estado inicial
    std::string estadoInicial;

    // F: conjunto de estados finais
    std::unordered_set<std::string> estadosFinais;

public:

    void adicionarEstado(const std::string& estado) {
        estados.insert(estado);
    }

    void adicionarSimbolo(char simbolo) {
        alfabeto.insert(simbolo);
    }

    void definirEstadoInicial(const std::string& estado) {
        if (!estados.contains(estado)) {
            throw std::invalid_argument(
                "O estado inicial deve pertencer a Q."
            );
        }

        estadoInicial = estado;
    }

    void adicionarEstadoFinal(const std::string& estado) {
        if (!estados.contains(estado)) {
            throw std::invalid_argument(
                "O estado final deve pertencer a Q."
            );
        }

        estadosFinais.insert(estado);
    }

    void adicionarTransicao(
        const std::string& estado,
        char simbolo,
        const std::string& proximoEstado
    ) {
        if (!estados.contains(estado)) {
            throw std::invalid_argument(
                "Estado de origem inexistente."
            );
        }

        if (!estados.contains(proximoEstado)) {
            throw std::invalid_argument(
                "Estado de destino inexistente."
            );
        }

        if (!alfabeto.contains(simbolo)) {
            throw std::invalid_argument(
                "Simbolo inexistente no alfabeto."
            );
        }

        transicoes[estado][simbolo] = proximoEstado;
    }

    bool aceita(const std::string& palavra) const {

        std::string estado = estadoInicial;

        for (char simbolo : palavra) {

            // Verifica se o símbolo pertence a Σ.
            if (!alfabeto.contains(simbolo)) {
                return false;
            }

            // Procura δ(estado, símbolo).
            auto estadoIt = transicoes.find(estado);

            if (estadoIt == transicoes.end()) {
                return false;
            }

            auto simboloIt = estadoIt->second.find(simbolo);

            if (simboloIt == estadoIt->second.end()) {
                return false;
            }

            // q ← δ(q, símbolo)
            estado = simboloIt->second;
        }

        // w é aceita se q ∈ F.
        return estadosFinais.contains(estado);
    }
};

bool testarCaso(
    const AFD& afd,
    const std::string& palavra,
    bool esperadoAceita,
    const std::string& descricao
) {
    const bool aceita = afd.aceita(palavra);
    const bool ok = (aceita == esperadoAceita);

    std::cout << (ok ? "[OK]     " : "[FALHOU] ")
              << descricao
              << " -> esperado " << (esperadoAceita ? "ACEITA" : "REJEITADA")
              << ", obtido " << (aceita ? "ACEITA" : "REJEITADA")
              << '\n';

    return ok;
}


std::string lerArquivo(const std::string& nomeArquivo) {

    std::ifstream arquivo(nomeArquivo);

    if (!arquivo.is_open()) {
        throw std::runtime_error(
            "Nao foi possivel abrir o arquivo: " + nomeArquivo
        );
    }

    // Lê todo o arquivo, inclusive espaços e quebras de linha.
    std::string conteudo(
        (std::istreambuf_iterator<char>(arquivo)),
        std::istreambuf_iterator<char>()
    );

    return conteudo;
}


int main() {

    try {

        // ==================================================
        // CONSTRUÇÃO DO AFD
        // ==================================================

        AFD afd;

        // Q: conjunto de estados
        afd.adicionarEstado("q0");
        afd.adicionarEstado("q1");

        afd.definirEstadoInicial("q0");
        afd.adicionarEstadoFinal("q1");

        // Σ: Alfabeto da Linguagem Xuxu
        std::vector<char> simbolosAlfabeto;

        for (char c = 'a'; c <= 'z'; ++c) {
            simbolosAlfabeto.push_back(c);
        }

        for (char c = 'A'; c <= 'Z'; ++c) {
            simbolosAlfabeto.push_back(c);
        }

        for (char c = '0'; c <= '9'; ++c) {
            simbolosAlfabeto.push_back(c);
        }

        const std::string pontuacaoOperadores = "!@#$%&*|()'\"-=+\\/?_<>.:;{}[]";
        for (char c : pontuacaoOperadores) {
            simbolosAlfabeto.push_back(c);
        }
        simbolosAlfabeto.push_back(',');
        simbolosAlfabeto.push_back(' ');
        simbolosAlfabeto.push_back('\t');
        simbolosAlfabeto.push_back('\n');
        simbolosAlfabeto.push_back('\r');

        // Registra cada simbolo em Σ
        for (char c : simbolosAlfabeto) {
            afd.adicionarSimbolo(c);
        }


        for (char c : simbolosAlfabeto) {
            bool ehMaiuscula = std::isupper(static_cast<unsigned char>(c));
            bool ehNumero    = std::isdigit(static_cast<unsigned char>(c));

            if (!ehMaiuscula && !ehNumero) {
                afd.adicionarTransicao("q0", c, "q1");
            }

            afd.adicionarTransicao("q1", c, "q1");
        }

        // CASOS DE VERIFICAÇÃO
        int casosTotal = 0;
        int casosRecusa = 0;
        int casosFalhos = 0;

        auto caso = [&](const std::string& palavra,
                        bool esperadoAceita,
                        const std::string& descricao) {
            ++casosTotal;
            if (!esperadoAceita) ++casosRecusa;
            if (!testarCaso(afd, palavra, esperadoAceita, descricao)) {
                ++casosFalhos;
            }
        };

        caso("hello",  true,  "comeca com minuscula: aceita");
        caso("_teste", true,  "comeca com simbolo do alfabeto: aceita");
        caso("",       false, "cadeia vazia: q0 nao e final");
        caso("Hello",  false, "comeca com maiuscula: sem transicao em q0");
        caso("9abc",   false, "comeca com digito: sem transicao em q0");

        std::cout << "\ncasos totais: " << casosTotal
                  << ", casos de recusa: " << casosRecusa
                  << ", casos falhos: " << casosFalhos << "\n\n";


        // LEITURA DO ARQUIVO
        const std::string nomeArquivo = "hello.xuxu";

        std::string palavra = lerArquivo(nomeArquivo);


        // ==================================================
        // EXECUÇÃO DO AFD
        // ==================================================

        if (afd.aceita(palavra)) {
            std::cout << "ACEITA\n";
        }
        else {
            std::cout << "REJEITADA\n";
        }

        if (casosFalhos > 0) {
            return 1;
        }

    }
    catch (const std::exception& erro) {

        std::cerr << "Erro: "
                  << erro.what()
                  << '\n';

        return 1;
    }

    return 0;
}
