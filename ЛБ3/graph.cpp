#include <algorithm>
#include <functional>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
using namespace std;
using Graph = vector<vector<int>>;
using Edge = pair<int, int>;

Graph makeGraph(int n, const vector<Edge>& edges) {
    Graph g(n, vector<int>(n));
    for (auto [u, v] : edges) g[u-1][v-1] = g[v-1][u-1] = 1;
    return g;
}
vector<int> degrees(const Graph& g) {
    vector<int> d;
    for (const auto& row : g) d.push_back(accumulate(row.begin(), row.end(), 0));
    return d;
}
int reachable(const Graph& g, int start) {
    vector<int> seen(g.size()), stack{start};
    seen[start] = 1;
    while (!stack.empty()) {
        int u = stack.back(); stack.pop_back();
        for (int v = 0; v < int(g.size()); ++v)
            if (g[u][v] && !seen[v]) { seen[v] = 1; stack.push_back(v); }
    }
    return accumulate(seen.begin(), seen.end(), 0);
}
bool connected(const Graph& g) {
    return !g.empty() && reachable(g, 0) == int(g.size());
}
// 0: no Euler traversal, 1: open trail, 2: cycle.
int eulerType(const Graph& g) {
    if (!connected(g)) return 0;
    auto d = degrees(g);
    int odd = count_if(d.begin(), d.end(), [](int x) { return x % 2; });
    return odd == 0 ? 2 : (odd == 2 ? 1 : 0);
}
// Fleury: do not use a bridge while another edge is available.
vector<int> euler(Graph g) {
    if (!eulerType(g)) return {};
    auto d = degrees(g);
    int u = 0;
    for (int v = 0; v < int(g.size()); ++v)
        if (d[v] % 2) { u = v; break; }
    vector<int> path{u};
    while (true) {
        int chosen = -1;
        int degree = accumulate(g[u].begin(), g[u].end(), 0);
        for (int v = 0; v < int(g.size()); ++v) if (g[u][v]) {
            int before = reachable(g, u);
            g[u][v] = g[v][u] = 0;
            bool bridge = reachable(g, u) < before;
            g[u][v] = g[v][u] = 1;
            if (degree == 1 || !bridge) { chosen = v; break; }
        }
        if (chosen < 0) break;
        g[u][chosen] = g[chosen][u] = 0;
        u = chosen; path.push_back(u);
    }
    return path;
}
// Backtracking. For an open path, try every starting vertex.
vector<int> hamilton(const Graph& g, bool cycle) {
    int n = int(g.size());
    if (!n || (cycle && n < 3)) return {};
    vector<int> path, used(n);
    function<bool()> dfs = [&]() {
        if (int(path.size()) == n)
            return !cycle || g[path.back()][path.front()];
        for (int v = 0; v < n; ++v)
            if (!used[v] && g[path.back()][v]) {
                used[v] = 1; path.push_back(v);
                if (dfs()) return true;
                path.pop_back(); used[v] = 0;
            }
        return false;
    };
    for (int start = 0; start < (cycle ? 1 : n); ++start) {
        path = {start}; fill(used.begin(), used.end(), 0); used[start] = 1;
        if (dfs()) {
            if (cycle) path.push_back(start);
            return path;
        }
    }
    return {};
}
// Minimum number of added edges; intended for small lab graphs.
vector<Edge> repairEuler(Graph& g) {
    if (eulerType(g) == 2) return {};
    vector<Edge> missing, added;
    int n = int(g.size());
    for (int u = 0; u < n; ++u)
        for (int v = u+1; v < n; ++v)
            if (!g[u][v]) missing.emplace_back(u, v);
    function<bool(int,int)> search = [&](int pos, int left) {
        if (!left) return eulerType(g) == 2;
        for (int i = pos; i <= int(missing.size())-left; ++i) {
            auto [u, v] = missing[i];
            g[u][v] = g[v][u] = 1; added.emplace_back(u, v);
            if (search(i+1, left-1)) return true;
            added.pop_back(); g[u][v] = g[v][u] = 0;
        }
        return false;
    };
    for (int k = 1; k <= int(missing.size()); ++k)
        if (search(0, k)) return added;
    throw runtime_error("Cannot create an Euler cycle by adding edges");
}
vector<Edge> repairHamilton(Graph& g) {
    vector<Edge> added;
    if (!hamilton(g, true).empty()) return added;
    int n = int(g.size());
    if (n < 3) throw runtime_error("A simple cycle needs at least 3 vertices");
    for (int u = 0; u < n; ++u) {
        int v = (u+1) % n;
        if (!g[u][v]) {
            g[u][v] = g[v][u] = 1; added.emplace_back(u, v);
        }
    }
    return added;
}
void printPath(const string& label, const vector<int>& path) {
    cout << label << ": ";
    if (path.empty()) cout << "нет";
    for (size_t i = 0; i < path.size(); ++i)
        cout << (i ? " -> " : "") << path[i]+1;
    cout << '\n';
}
void printEdges(const vector<Edge>& edges) {
    if (edges.empty()) cout << "нет";
    for (auto [u, v] : edges) cout << '(' << u+1 << ',' << v+1 << ") ";
    cout << '\n';
}
void show(const Graph& g) {
    cout << "Степени: ";
    for (int d : degrees(g)) cout << d << ' ';
    cout << "\nЭйлеров граф: " << (eulerType(g) == 2 ? "да" : "нет") << '\n';
    printPath("Эйлерова цепь", euler(g));
    auto hc = hamilton(g, true);
    cout << "Гамильтонов граф: " << (!hc.empty() ? "да" : "нет") << '\n';
    printPath("Гамильтонов цикл", hc);
    printPath("Гамильтонов путь", hamilton(g, false));
}
int main() {
    Graph g = makeGraph(8, {{1,2}, {1,6}, {2,3}, {2,5},
        {2,8}, {3,4}, {3,6}, {4,5}, {5,6}, {5,8}, {6,7}, {7,8}});
    cout << "Вариант 12. Исходный граф\n"; show(g);
    cout << "\nЭтап 1. Построение эйлерова цикла\nДобавлены рёбра: ";
    printEdges(repairEuler(g));
    printPath("Эйлеров цикл", euler(g));
    cout << "\nЭтап 2. Поиск гамильтонова цикла в текущем графе\n";
    cout << "Добавлены рёбра: "; printEdges(repairHamilton(g));
    show(g);
    cout << "Матрица смежности итогового графа\n";
    for (const auto& row : g) {
        for (int x : row) cout << x << ' ';
        cout << '\n';
    }
    return 0;
}
