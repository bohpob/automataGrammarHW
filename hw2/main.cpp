#include <algorithm>
#include <cassert>
#include <deque>
#include <list>
#include <map>
#include <set>
#include <vector>

using namespace std;
using Symbol = char;
using Word = std::vector<Symbol>;

struct Grammar {
    std::set<Symbol> m_Nonterminals;
    std::set<Symbol> m_Terminals;
    std::vector<std::pair<Symbol, std::vector<Symbol> > > m_Rules;
    Symbol m_InitialSymbol;
};


void ruleIndices(map<int, map<int, map<Symbol, list<pair<int, pair<Symbol, Symbol> > > > > > &back, int n, int i,
                 Symbol A, vector<size_t> &result, const Grammar &grammar, const Word &word) {
    if (n == 1) {
        for (auto it = grammar.m_Rules.begin(); it != grammar.m_Rules.end(); it++) {
            if (it->first == A && it->second.size() == 1 && word[i - 1] == it->second[0]) {
                size_t ruleIdx = distance(grammar.m_Rules.begin(), it);
                result.push_back(ruleIdx);
                break;
            }
        }
        return;
    }

    for (const auto &b: back[n][i][A]) {
        int p = b.first;
        Symbol B = b.second.first;
        Symbol C = b.second.second;
        for (auto it = grammar.m_Rules.begin(); it != grammar.m_Rules.end(); it++) {
            if (it->first == A && it->second.size() == 2 && it->second[0] == B && it->second[1] == C) {
                size_t ruleIdx = distance(grammar.m_Rules.begin(), it);
                result.push_back(ruleIdx);

                ruleIndices(back, p, i, B, result, grammar, word);
                ruleIndices(back, n - p, i + p, C, result, grammar, word);
                return;
            }
        }
    }
}

std::vector<size_t> trace(const Grammar &grammar, const Word &word) {
    int n = (int) word.size();
    map<int, map<int, set<Symbol> > > table;
    map<int, map<int, map<Symbol, list<pair<int, pair<Symbol, Symbol> > > > > > back;

    if (n == 0) {
        for (auto it = grammar.m_Rules.begin(); it != grammar.m_Rules.end(); it++) {
            if (it->first == grammar.m_InitialSymbol && it->second.empty()) {
                vector<size_t> x;
                x.push_back(distance(grammar.m_Rules.begin(), it));
                return x;
            }
        }
    }

    if (n == 1) {
        for (auto it = grammar.m_Rules.begin(); it != grammar.m_Rules.end(); it++) {
            if (it->first == grammar.m_InitialSymbol && it->second.size() == 1 && word[0] == it->second[0]) {
                vector<size_t> x;
                x.push_back(distance(grammar.m_Rules.begin(), it));
                return x;
            }
        }
    }

    for (int i = 1; i <= n; i++) {
        for (const auto &rule: grammar.m_Rules) {
            if (rule.second.size() == 1 && rule.second[0] == word[i - 1]) {
                table[1][i].insert(rule.first);
            }
        }
    }

    for (int i = 2; i <= n; i++) {
        for (int j = 1; j <= n - i + 1; j++) {
            for (int k = 1; k <= i - 1; k++) {
                for (const auto &rule: grammar.m_Rules) {
                    if (rule.second.size() == 2) {
                        Symbol A = rule.first;
                        Symbol B = rule.second[0];
                        Symbol C = rule.second[1];

                        if (table[k][j].find(B) != table[k][j].end() &&
                            table[i - k][j + k].find(C) != table[i - k][j + k].end()) {
                            table[i][j].insert(A);
                            back[i][j][A].push_back({k, {B, C}});
                        }
                    }
                }
            }
        }
    }

    vector<size_t> result;

    if (table[n][1].find(grammar.m_InitialSymbol) != table[n][1].end()) {
        for (const auto &b: back[n][1][grammar.m_InitialSymbol]) {
            for (auto it = grammar.m_Rules.begin(); it != grammar.m_Rules.end(); it++) {
                int p = b.first;
                Symbol B = b.second.first;
                Symbol C = b.second.second;

                if (it->first == grammar.m_InitialSymbol && it->second.size() == 2
                    && it->second[0] == B && it->second[1] == C) {
                    size_t ruleIdx = distance(grammar.m_Rules.begin(), it);
                    result.push_back(ruleIdx);

                    ruleIndices(back, p, 1, B, result, grammar, word);
                    ruleIndices(back, n - p, 1 + p, C, result, grammar, word);

                    if ((int) result.size() == n + (n - 1)) {
                        return result;
                    }
                }
            }
        }
    }
    return result;
}

int main() {
    Grammar g0{
        {'A', 'B', 'C', 'S'},
        {'a', 'b'},
        {
            {'S', {'A', 'B'}},
            {'S', {'B', 'C'}},
            {'A', {'B', 'A'}},
            {'A', {'a'}},
            {'B', {'C', 'C'}},
            {'B', {'b'}},
            {'C', {'A', 'B'}},
            {'C', {'a'}},
        },
        'S'
    };

    assert(trace(g0, {'b'}) == std::vector<size_t>({}));
    assert(trace(g0, {'a'}) == std::vector<size_t>({}));
    assert(trace(g0, {}) == std::vector<size_t>({}));
    assert(trace(g0, {'a', 'b'}) == std::vector<size_t>({0, 3, 5}));
    assert(trace(g0, {'b', 'a'}) == std::vector<size_t>({1, 5, 7}));
    assert(trace(g0, {'c', 'a'}) == std::vector<size_t>({}));

    Grammar g1{
        {'A', 'B'},
        {'x', 'y'},
        {
            {'A', {}},
            {'A', {'x'}},
            {'B', {'x'}},
            {'A', {'B', 'B'}},
            {'B', {'B', 'B'}},
        },
        'A'
    };

    assert(trace(g1, {}) == std::vector<size_t>({0}));
    assert(trace(g1, {'x'}) == std::vector<size_t>({1}));
    assert(trace(g1, {'x', 'x'}) == std::vector<size_t>({3, 2, 2}));

    Grammar g2{
        {'A', 'B'},
        {'x', 'y'},
        {
            {'A', {'x'}},
            {'B', {'x'}},
            {'A', {'B', 'B'}},
            {'B', {'B', 'B'}},
        },
        'B'
    };

    assert(trace(g2, {}) == std::vector<size_t>({}));
    assert(trace(g2, {'x'}) == std::vector<size_t>({1}));
    assert(trace(g2, {'x', 'x'}) == std::vector<size_t>({3, 1, 1}));

    Grammar g3{
        {'A', 'B', 'C', 'D', 'E', 'S'},
        {'a', 'b'},
        {
            {'S', {'A', 'B'}},
            {'S', {'S', 'S'}},
            {'S', {'a'}},
            {'A', {'B', 'S'}},
            {'A', {'C', 'D'}},
            {'A', {'b'}},
            {'B', {'D', 'D'}},
            {'B', {'b'}},
            {'C', {'D', 'E'}},
            {'C', {'b'}},
            {'C', {'a'}},
            {'D', {'a'}},
            {'E', {'S', 'S'}},
        },
        'S'
    };

    assert(trace(g3, {}) == std::vector<size_t>({}));
    assert(trace(g3, {'b'}) == std::vector<size_t>({}));
    assert(trace(g3, {'a', 'b', 'a', 'a', 'b'}) == std::vector<size_t>({1, 2, 0, 3, 7, 1, 2, 2, 7}));
}
