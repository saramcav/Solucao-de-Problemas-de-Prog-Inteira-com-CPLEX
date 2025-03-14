#include "coloracao.h"

using namespace std;

typedef set<pair<int, int>> EdgeSet;

VertexWeightedGraph::VertexWeightedGraph(int vertex_num) : vertex_group(vertex_num) {}

void VertexWeightedGraph::add_vertex(int u, double weight) {
    vertex_group[u].weight = weight;
}

void VertexWeightedGraph::add_edge(int u, int v) {
    vertex_group[u].neighbors.insert(v);
    vertex_group[v].neighbors.insert(u);
}

bool VertexWeightedGraph::are_adjacent(int u, int v) const {
    return vertex_group[u].neighbors.find(v) != vertex_group[u].neighbors.end();
}

bool VertexWeightedGraph::is_marked(int u) const {
    return vertex_group[u].marked;
}

double VertexWeightedGraph::vertex_weight(int u) const {
    return vertex_group[u].weight;
}

int VertexWeightedGraph::vertex_degree(int v) const {
    return vertex_group[v].neighbors.size();
}

int VertexWeightedGraph::vertex_num() const {
    return vertex_group.size();
}

void VertexWeightedGraph::mark_vertex(int v) {
    vertex_group[v].marked = true;
}

void fill_residual_graph(VertexWeightedGraph& G_j, EdgeSet& E, vector<double>& x_j_values) {
    int n = G_j.vertex_num();
    double i_j_value;
    for (int i = 0; i < n; i++) {
        i_j_value = x_j_values[i];
        G_j.add_vertex(i, i_j_value); 
    }

    for (const auto& [i, k] : E) {
        G_j.add_edge(i-1, k-1);
    }
}

bool forms_clique(const set<int>& partial_maximal_S, int v, const VertexWeightedGraph& G_j) {
    for (int w : partial_maximal_S) {
        if (v != w && !G_j.are_adjacent(v, w)) {
            return false;
        }
    }
    return true;
}

pair<set<int>, bool> greedy_maximal_clique(const VertexWeightedGraph& G_j, double w_j) {
    int n = G_j.vertex_num();
    set<int> maximal_S;
    double sum_S_weight = 0.0;

    //ordena os indices dos vertices de forma nao crescente pela heuristica peso do vertice * grau do vertice 
    vector<int> V;
    for (int i = 0; i < n; i++) {
        if(!G_j.is_marked(i)) {
            V.push_back(i);
        }
    }
    sort(V.begin(), V.end(), [&G_j](int u, int v) {
        return G_j.vertex_weight(u) * G_j.vertex_degree(u) > G_j.vertex_weight(v) * G_j.vertex_degree(v);
    });

    //tenta adicionar cada vertice na clique seguindo a ordenacao acima
    for (int v : V) {
        if (forms_clique(maximal_S, v, G_j)) {
            maximal_S.insert(v);
            sum_S_weight += G_j.vertex_weight(v);
        }
    }

    bool violated_S = (sum_S_weight > (w_j + MAX_ERROR));
    return {maximal_S, violated_S};
}