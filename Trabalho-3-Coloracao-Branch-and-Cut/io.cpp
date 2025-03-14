#include "coloracao.h"

using namespace std;

void output_message(string message, ofstream& result_file) {
    cout << message << endl;
    result_file << message << endl;
}

void read_input_file(const string& input_file, set<int>& V, EdgeSet& E, int& n, int& m) {
    ifstream entry_file(input_file);
    if (!entry_file.is_open()) {
        cerr << "Erro ao abrir o arquivo " << input_file << endl;
        exit(1);
    }

    char p, e;
    string edge;
    entry_file >> p >> edge >> n >> m;

    int i, j;
    for(int k=0; k<m; k++) {
        entry_file >> e >> i >> j;
        V.insert(i);
        V.insert(j);
        E.insert(make_pair(i, j));
    }

    entry_file.close();
}

void print_and_write_result(IloCplex& solver, string input_file, double total_time, int n, IloArray<IloBoolVarArray>& x, int num_c, int user_cuts) {
    const string user_cuts_active = user_cuts? "com_cortes_usuario_" : "sem_cortes_usuario_"; 
    ofstream result_file("saida_" + user_cuts_active + input_file);
    if (!result_file.is_open()) {
        cerr << "Erro ao abrir o arquivo de saída." << endl;
        exit(1);
    }

    const string solved = solver.getStatus() == IloAlgorithm::Optimal? "SIM" : "NAO"; 
    const string user_constraints = user_cuts? "SIM" : "NAO"; 
    int result = solver.getObjValue();
    
    output_message("-----FIM-DE-EXECUCAO-----", result_file);
    output_message("ARQUIVO DE ENTRADA: " + input_file, result_file);
    output_message("CORTES USUARIO: " + user_constraints, result_file);
    output_message("RESOLVIDO: " + solved, result_file);
    output_message("RESULTADO: " + to_string(result), result_file);
    output_message("TEMPO: " + to_string(total_time) + " SEGUNDO(S)", result_file);
    output_message("NUMERO DE CORTES: " + to_string(num_c), result_file);

    print_and_write_v_colors(solver, result_file, n, x);

    result_file.close();
}

void print_and_write_v_colors(IloCplex& solver, ofstream& result_file, int n, IloArray<IloBoolVarArray>& x) {
    output_message("COLORACAO OTIMA (VERTICE, COR): ", result_file);

    int x_ij, pos1, pos2;
    string x_name, i_E;
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            x_ij = solver.getValue(x[i][j]);
            if(x_ij >= (1-MAX_ERROR)) {
                x_name = x[i][j].getName();
                pos1 = x_name.find('_'); 
                pos2 = x_name.find('_', pos1 + 1); 
                i_E = x_name.substr(pos1 + 1, pos2 - pos1 - 1);
                output_message("(" + i_E + ", " + to_string(j+1) + ")", result_file);
            }
        }
    }
}
