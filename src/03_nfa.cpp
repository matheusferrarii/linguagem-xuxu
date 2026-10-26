// Tarefa 1 do módulo 3: converte a árvore de padrão em máquina não determinística
// (construção de Thompson). Cobre todos os nós do núcleo: símbolo, vazio, qualquer,
// concat, alt e fecho. Um nó desconhecido lança erro, nunca vira máquina vazia.
//
// Compilar: g++ -std=c++17 -o nfa src/03_nfa.cpp
// Usar:     ./nfa            (roda os testes)
//           ./nfa '[0-9]+'   (imprime árvore, máquina e testa cadeias lidas do stdin)

#include <cstdio>
#include <iostream>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

// ---------- árvore ----------

enum class Tipo { Simbolo, Vazio, Qualquer, Concat, Alt, Fecho };

struct No;
using Ptr = std::shared_ptr<No>;
struct No {
    Tipo tipo;
    char c = 0;
    Ptr a, b;
};

static Ptr mk(Tipo t, Ptr a = nullptr, Ptr b = nullptr, char c = 0) {
    auto n = std::make_shared<No>();
    n->tipo = t; n->a = a; n->b = b; n->c = c;
    return n;
}
static Ptr simbolo(char c) { return mk(Tipo::Simbolo, nullptr, nullptr, c); }
static Ptr vazio() { return mk(Tipo::Vazio); }
static Ptr qualquer() { return mk(Tipo::Qualquer); }
static Ptr concat(Ptr a, Ptr b) { return mk(Tipo::Concat, a, b); }
static Ptr alt(Ptr a, Ptr b) { return mk(Tipo::Alt, a, b); }
static Ptr fecho(Ptr a) { return mk(Tipo::Fecho, a); }

static std::string linear(const Ptr& n) {
    switch (n->tipo) {
    case Tipo::Simbolo: return std::string("s:'") + n->c + "'";
    case Tipo::Vazio: return "vazio";
    case Tipo::Qualquer: return "qualquer";
    case Tipo::Concat: return "concat(" + linear(n->a) + "," + linear(n->b) + ")";
    case Tipo::Alt: return "alt(" + linear(n->a) + "," + linear(n->b) + ")";
    case Tipo::Fecho: return "fecho(" + linear(n->a) + ")";
    }
    throw std::logic_error("no desconhecido");
}

// ---------- leitor de padrões (gramática de docs/01_gramatica.txt, já reduzindo ao núcleo) ----------

struct Leitor {
    std::string s;
    size_t i = 0;
    [[noreturn]] void erro(const std::string& m) {
        throw std::runtime_error(m + " (posicao " + std::to_string(i) + ")");
    }
    bool fim() const { return i >= s.size(); }

    Ptr alternancia() {
        Ptr r = sequencia();
        while (!fim() && s[i] == '|') { i++; r = alt(r, sequencia()); }
        return r;
    }
    Ptr sequencia() {
        Ptr r = nullptr;
        while (!fim() && s[i] != '|' && s[i] != ')') {
            Ptr p = repeticao();
            r = r ? concat(r, p) : p;
        }
        return r ? r : vazio();
    }
    Ptr repeticao() {
        if (s[i] == '*' || s[i] == '+' || s[i] == '?') erro("operador sem expressao a que se aplicar");
        Ptr a = atomo();
        if (!fim() && s[i] == '*') { i++; return fecho(a); }
        if (!fim() && s[i] == '+') { i++; return concat(a, fecho(a)); }
        if (!fim() && s[i] == '?') { i++; return alt(a, vazio()); }
        return a;
    }
    Ptr atomo() {
        char c = s[i++];
        if (c == '.') return qualquer();
        if (c == '(') {
            Ptr r = alternancia();
            if (fim() || s[i] != ')') erro("falta o fecha-parenteses do grupo");
            i++;
            return r;
        }
        if (c == '[') return classe();
        if (c == '\\') {
            if (fim()) erro("barra sem simbolo a escapar");
            return simbolo(s[i++]);
        }
        return simbolo(c);
    }
    Ptr classe() {
        std::vector<char> v;
        while (true) {
            if (fim()) erro("classe de caracteres nao fechada");
            char c = s[i++];
            if (c == ']') break;
            if (!fim() && s[i] == '-' && i + 1 < s.size() && s[i + 1] != ']') {
                char f = s[i + 1];
                i += 2;
                if (f < c) erro("intervalo invertido na classe");
                for (int x = c; x <= f; x++) v.push_back((char)x);
            } else v.push_back(c);
        }
        if (v.empty()) erro("classe vazia");
        Ptr r = simbolo(v[0]);
        for (size_t k = 1; k < v.size(); k++) r = alt(r, simbolo(v[k]));
        return r;
    }
    Ptr ler() {
        Ptr r = alternancia();
        if (!fim()) erro("simbolo inesperado apos o fim da expressao");
        return r;
    }
};

static Ptr le_padrao(const std::string& p) { Leitor l; l.s = p; return l.ler(); }

// ---------- máquina não determinística ----------

enum class Rotulo { Eps, Simbolo, Qualquer };

struct Transicao { int de; Rotulo rot; char c; int para; };

struct Maquina {
    int inicio = 0, aceita = 0;
    int n_estados = 0;
    std::vector<Transicao> trans;

    int novo() { return n_estados++; }
    void liga(int de, Rotulo r, char c, int para) { trans.push_back({de, r, c, para}); }
};

struct Fragmento { int ini, fim; };

// Thompson: cada fragmento tem um estado inicial sem entradas de fora e um final sem saídas.
static Fragmento constroi(Maquina& m, const Ptr& n) {
    switch (n->tipo) {
    case Tipo::Simbolo: {
        int i = m.novo(), f = m.novo();
        m.liga(i, Rotulo::Simbolo, n->c, f);
        return {i, f};
    }
    case Tipo::Qualquer: {
        int i = m.novo(), f = m.novo();
        m.liga(i, Rotulo::Qualquer, 0, f);
        return {i, f};
    }
    case Tipo::Vazio: {
        int i = m.novo(), f = m.novo();
        m.liga(i, Rotulo::Eps, 0, f);
        return {i, f};
    }
    case Tipo::Concat: {
        Fragmento x = constroi(m, n->a), y = constroi(m, n->b);
        m.liga(x.fim, Rotulo::Eps, 0, y.ini);
        return {x.ini, y.fim};
    }
    case Tipo::Alt: {
        Fragmento x = constroi(m, n->a), y = constroi(m, n->b);
        int i = m.novo(), f = m.novo();
        m.liga(i, Rotulo::Eps, 0, x.ini);
        m.liga(i, Rotulo::Eps, 0, y.ini);
        m.liga(x.fim, Rotulo::Eps, 0, f);
        m.liga(y.fim, Rotulo::Eps, 0, f);
        return {i, f};
    }
    case Tipo::Fecho: {
        Fragmento x = constroi(m, n->a);
        int i = m.novo(), f = m.novo();
        m.liga(i, Rotulo::Eps, 0, x.ini);
        m.liga(i, Rotulo::Eps, 0, f);
        m.liga(x.fim, Rotulo::Eps, 0, x.ini);
        m.liga(x.fim, Rotulo::Eps, 0, f);
        return {i, f};
    }
    }
    throw std::logic_error("no da arvore sem construcao correspondente");
}

static Maquina arvore_para_maquina(const Ptr& raiz) {
    Maquina m;
    Fragmento f = constroi(m, raiz);
    m.inicio = f.ini;
    m.aceita = f.fim;
    return m;
}

// ---------- simulação (para testar a máquina) ----------

static std::set<int> fecho_eps(const Maquina& m, std::set<int> S) {
    std::vector<int> pilha(S.begin(), S.end());
    while (!pilha.empty()) {
        int e = pilha.back(); pilha.pop_back();
        for (auto& t : m.trans)
            if (t.de == e && t.rot == Rotulo::Eps && S.insert(t.para).second) pilha.push_back(t.para);
    }
    return S;
}

static bool aceita(const Maquina& m, const std::string& w) {
    std::set<int> S = fecho_eps(m, {m.inicio});
    for (char c : w) {
        std::set<int> T;
        for (auto& t : m.trans)
            if (S.count(t.de) && ((t.rot == Rotulo::Simbolo && t.c == c) || t.rot == Rotulo::Qualquer))
                T.insert(t.para);
        S = fecho_eps(m, T);
    }
    return S.count(m.aceita) > 0;
}

static void imprime(const Maquina& m) {
    std::printf("estados=%d inicio=%d aceita=%d transicoes=%zu\n", m.n_estados, m.inicio, m.aceita, m.trans.size());
    for (auto& t : m.trans) {
        std::string r = t.rot == Rotulo::Eps ? "eps" : t.rot == Rotulo::Qualquer ? "qualquer" : std::string(1, t.c);
        std::printf("  %d --%s--> %d\n", t.de, r.c_str(), t.para);
    }
}

// ---------- testes ----------

static int falhas = 0;
static void confere(const std::string& padrao, const std::string& w, bool esperado) {
    bool got = aceita(arvore_para_maquina(le_padrao(padrao)), w);
    if (got != esperado) {
        falhas++;
        std::printf("FALHA: /%s/ sobre \"%s\": esperado %d, obtido %d\n", padrao.c_str(), w.c_str(), esperado, got);
    }
}

static int testes() {
    // um teste por tipo de nó, mais as peças reais da Xuxu
    confere("a", "a", true);  confere("a", "b", false); confere("a", "", false);          // símbolo
    confere("a?", "", true);  confere("a?", "a", true); confere("a?", "aa", false);       // vazio (via opcional)
    confere("", "", true);    confere("", "a", false);                                     // vazio puro
    confere(".", "x", true);  confere(".", "", false);  confere(".", "xy", false);        // qualquer
    confere("ab", "ab", true); confere("ab", "a", false);                                  // concat
    confere("a|b", "b", true); confere("a|b", "c", false);                                 // alt
    confere("a*", "", true);  confere("a*", "aaa", true); confere("a*", "ab", false);      // fecho
    confere("(a|b)*", "abba", true); confere("(a*b*)*", "abba", true);                     // par que não converge
    confere("(a*)*", "aaa", true);                                                         // fecho de fecho (sem laço eps)
    confere("(a?)*", "", true);                                                            // fecho de vazível
    confere("\".*\"", "\"Chuchu\"", true); confere("\".*\"", "\"abc", false);              // texto entre aspas
    confere("\".*\"", "\"\"", true);
    confere("[0-9]+", "123", true); confere("[0-9]+", "", false); confere("[0-9]+", "12a", false);
    confere("[0-9]+(\\.[0-9]+)?", "7.5", true); confere("[0-9]+(\\.[0-9]+)?", "7.", false);
    confere("[a-zA-Z_][a-zA-Z0-9_]*", "media_1", true); confere("[a-zA-Z_][a-zA-Z0-9_]*", "1a", false);
    confere("talvez|fudeu|chega|pula", "chega", true); confere("talvez|fudeu|chega|pula", "chegar", false);
    confere("<|>|=|\\+", "+", true); confere("<|>|=|\\+", "<>", false);
    confere("([01][0-9]|2[0-3]):[0-5][0-9]", "08:00", true);
    confere("([01][0-9]|2[0-3]):[0-5][0-9]", "24:00", false);

    // tamanho: Thompson tem no máximo 2 estados por nó e no máximo 2 transições saindo de cada estado
    Maquina m = arvore_para_maquina(le_padrao("[0-9]+(\\.[0-9]+)?"));
    std::vector<int> saidas(m.n_estados, 0);
    for (auto& t : m.trans) saidas[t.de]++;
    for (int s : saidas) if (s > 2) { falhas++; std::printf("FALHA: estado com mais de 2 saidas\n"); break; }
    if (saidas[m.aceita] != 0) { falhas++; std::printf("FALHA: estado de aceitacao com saidas\n"); }

    // recusas do leitor continuam recusas
    for (const char* ruim : {"(talvez", "+[0-9]", "[a-z", "abc)"}) {
        try { le_padrao(ruim); falhas++; std::printf("FALHA: aceitou /%s/\n", ruim); }
        catch (const std::runtime_error&) {}
    }

    std::printf(falhas ? "%d falha(s)\n" : "todos os testes passaram\n", falhas);
    return falhas ? 1 : 0;
}

int main(int argc, char** argv) {
    if (argc < 2) return testes();
    try {
        Ptr t = le_padrao(argv[1]);
        std::printf("arvore: %s\n", linear(t).c_str());
        Maquina m = arvore_para_maquina(t);
        imprime(m);
        std::string w;
        while (std::getline(std::cin, w)) std::printf("\"%s\" -> %s\n", w.c_str(), aceita(m, w) ? "aceita" : "rejeita");
    } catch (const std::exception& e) {
        std::fprintf(stderr, "erro: %s\n", e.what());
        return 1;
    }
}
