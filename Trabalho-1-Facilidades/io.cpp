#include "io.h"

using namespace std;

void read_input_file(string file_name, map<pair<int, int>,pair<double, int>>& g_p, 
                    map<int, int>& facility_occurrences, set<int>& facility_ids, set<int>& clients_ids,
                    int& c, int& Q) {
    ifstream entry_file(file_name);
    if (!entry_file.is_open()) {
        cerr << "Erro ao abrir o arquivo " << file_name << endl;
        exit(1);
    }

    string line;
    int ni, nj, NL, i, j, p_ij;
    double g_ij;
    pair<int, int> facility_client;
    pair<double, int> cost_resource;

    if (!getline(entry_file, line)) {
        cerr << "Erro ao ler linha inicial" << endl;
        exit(1);
    } 
    istringstream iss(line);
    iss >> ni >> nj >> c >> Q >> NL;

    for (int k = 0; k < NL; k++) {
        if (!getline(entry_file, line)) {
            cerr << "Erro ao ler a linha " << k + 1 << endl;
            exit(1);
        }

        istringstream iss(line);
        iss >> i >> j >> g_ij >> p_ij;
        facility_ids.insert(i);
        facility_occurrences[i]++;
        clients_ids.insert(j);

        facility_client = make_pair(i, j);
        cost_resource = make_pair(g_ij, p_ij);
        g_p[facility_client] = cost_resource;
    }

    entry_file.close();
}

void output_message(string message, ofstream& result_file) {
    cout << message << endl;
    result_file << message << endl;
}

void print_and_write_result(IloCplex& solver, string input_file, double total_time, int ni, IloArray<IloBoolVarArray>& x) {
    ofstream result_file("saida_" + input_file);
    if (!result_file.is_open()) {
        cerr << "Erro ao abrir o arquivo de saída." << endl;
        exit(1);
    }

    string solved = solver.getStatus() == IloAlgorithm::Optimal? "SIM" : "NAO"; 
    double result = solver.getObjValue();
    
    output_message("-----FIM-DE-EXECUCAO-----", result_file);
    output_message("ARQUIVO DE ENTRADA: " + input_file, result_file);
    output_message("RESOLVIDO: " + solved, result_file);
    output_message("RESULTADO:" + to_string(result), result_file);
    output_message("TEMPO: " + to_string(total_time) + " SEGUNDO(S)", result_file);

    print_and_write_optimal_edges(solver, result_file, ni, x);

    result_file.close();
}

void print_and_write_optimal_edges(IloCplex& solver, ofstream& result_file, int ni, IloArray<IloBoolVarArray>& x) {
    output_message("ATENDIMENTOS DA SOLUCAO OTIMA (FACILIDADE, CLIENTE): ", result_file);
    
    int x_ij, pos1, pos2;
    string x_name, edge_i, edge_j;
    for(int i=0; i<ni; i++) {
        for(int j=0; j < x[i].getSize(); j++) {
            x_ij = solver.getValue(x[i][j]);
            if(x_ij >= (1-MAX_ERROR)) {
                x_name = x[i][j].getName();
                pos1 = x_name.find('_'); 
                pos2 = x_name.find('_', pos1 + 1); 
                edge_i = x_name.substr(pos1 + 1, pos2 - pos1 - 1);
                edge_j = x_name.substr(pos2 + 1);
                output_message("(" + edge_i + ", " + edge_j + ")", result_file);
            }
        }
    }
}
