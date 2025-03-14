#ifndef TRABALHO_3
#define TRABALHO_3

#include <iostream>
#include <string>
#include <stdio.h>
#include <fstream>
#include <map>
#include <utility>
#include <set> 
#include <unordered_set>
#include <ilcplex/ilocplex.h>
#include <vector>
#include <algorithm>

#define MAX_ERROR 0.0001

struct Vertex {
    double weight;
    bool marked = false;
    std::unordered_set<int> neighbors;
};

class VertexWeightedGraph {
public:
    VertexWeightedGraph(int vertex_num);

    void add_vertex(int u, double weight);

    void add_edge(int u, int v);

    bool are_adjacent(int u, int v) const;

    bool is_marked(int u) const;

    double vertex_weight(int u) const;

    int vertex_degree(int v) const;

    int vertex_num() const;

    void mark_vertex(int v);

private:
    std::vector<Vertex> vertex_group;
};

typedef std::set<std::pair<int, int>> EdgeSet;

void set_solver_config(IloCplex& solver, IloEnv& env);

void read_input_file(const std::string& input_file, std::set<int>& V, EdgeSet& E, int& n, int& m);

void output_message(std::string message, std::ofstream& result_file);

void print_and_write_result(IloCplex& solver, std::string input_file, double total_time, int n, IloArray<IloBoolVarArray>& x, int num_c, int user_cuts);

void print_and_write_v_colors(IloCplex& solver, std::ofstream& result_file, int n, IloArray<IloBoolVarArray>& x);

void fill_residual_graph(VertexWeightedGraph& G_, EdgeSet& E, std::vector<double>& valores_x_j);

bool forms_clique(const std::set<int>& V, const VertexWeightedGraph& G);

std::pair<std::set<int>, bool> greedy_maximal_clique(const VertexWeightedGraph& G_j, double w_j);

#endif // TRABALHO_3