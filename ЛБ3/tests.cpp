#define main laboratory_main
#include "graph.cpp"
#undef main
#include <cassert>
#include <set>

bool validEuler(Graph g, const vector<int>& p) {
    int edges = 0;
    for (auto row : g) edges += accumulate(row.begin(), row.end(), 0);
    if (int(p.size()) != edges/2+1) return false;
    for (size_t i = 1; i < p.size(); ++i) {
        int u = p[i-1], v = p[i];
        if (!g[u][v]) return false;
        g[u][v] = g[v][u] = 0;
    }
    return true;
}
bool validHamilton(const Graph& g, vector<int> p, bool cycle) {
    if (p.size() != g.size() + (cycle ? 1 : 0)) return false;
    if (cycle && p.front() != p.back()) return false;
    for (size_t i = 1; i < p.size(); ++i)
        if (!g[p[i-1]][p[i]]) return false;
    if (cycle) p.pop_back();
    return set<int>(p.begin(), p.end()).size() == g.size();
}
bool bruteHamilton(const Graph& g, bool cycle) {
    if (cycle && g.size() < 3) return false;
    vector<int> p(g.size()); iota(p.begin(), p.end(), 0);
    do {
        bool ok = !cycle || g[p.back()][p.front()];
        for (size_t i = 1; i < p.size(); ++i) ok &= g[p[i-1]][p[i]];
        if (ok) return true;
    } while (next_permutation(p.begin(), p.end()));
    return false;
}
int main() {
    int checked = 0;
    for (int n = 1; n <= 5; ++n) {
        vector<Edge> possible;
        for (int u = 0; u < n; ++u)
            for (int v = u+1; v < n; ++v) possible.emplace_back(u,v);
        for (unsigned mask = 0; mask < (1u << possible.size()); ++mask) {
            Graph g(n, vector<int>(n));
            for (size_t i = 0; i < possible.size(); ++i) if (mask & (1u << i)) {
                auto [u,v] = possible[i]; g[u][v] = g[v][u] = 1;
            }
            auto ep = euler(g);
            assert((!ep.empty()) == (eulerType(g) != 0));
            if (!ep.empty()) assert(validEuler(g, ep));
            for (bool cycle : {false, true}) {
                auto hp = hamilton(g, cycle);
                assert((!hp.empty()) == bruteHamilton(g, cycle));
                if (!hp.empty()) assert(validHamilton(g, hp, cycle));
            }
            ++checked;
        }
    }
    Graph original = makeGraph(8, {{1,2},{1,6},{2,3},{2,5},{2,8},{3,4},
        {3,6},{4,5},{5,6},{5,8},{6,7},{7,8}});
    assert(eulerType(original) == 1);
    assert(validEuler(original, euler(original)));
    assert(validHamilton(original, hamilton(original, true), true));
    Graph g = original;
    auto added = repairEuler(g);
    assert((added == vector<Edge>{{2,7}}));
    assert(eulerType(g) == 2 && validEuler(g, euler(g)));
    assert(repairHamilton(g).empty());
    Graph star = makeGraph(5, {{1,2},{1,3},{1,4},{1,5}});
    assert(hamilton(star, true).empty());
    assert(!repairHamilton(star).empty());
    assert(validHamilton(star, hamilton(star, true), true));
    Graph twoTriangles = makeGraph(6, {{1,2},{2,3},{3,1},{4,5},{5,6},{6,4}});
    assert(eulerType(twoTriangles) == 0);
    assert(!repairEuler(twoTriangles).empty());
    assert(eulerType(twoTriangles) == 2 && validEuler(twoTriangles, euler(twoTriangles)));
    cout << "PASS: " << checked << " graphs, variant 12, both repair branches\n";
}
