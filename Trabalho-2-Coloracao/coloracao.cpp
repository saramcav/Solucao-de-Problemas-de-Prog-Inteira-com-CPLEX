#include "coloracao.h"

//---------------------------------------------------------------------------------------------------------------------------------------
//ALUNA: SARA MAIA CAVALCANTE - GRADUACAO
//---------------------------------------------------------------------------------------------------------------------------------------

using namespace std;

void set_solver_config(IloCplex& solver, IloEnv& env) {
    solver.setParam(IloCplex::Param::WorkMem, 1024 * 2);
    solver.setParam(IloCplex::Param::MIP::Strategy::File, 2);
    solver.setParam(IloCplex::Param::TimeLimit, 3600);
    solver.setParam(IloCplex::Param::Threads, 0); //quant. max de threads
    solver.setParam(IloCplex::Param::MIP::Interval, 100);
    solver.setOut(env.getNullStream()); 
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Execute com: " << argv[0] << " nome_do_arquivo.txt" << endl;
        return 1;
    }

    //leitura do arquivo e captura das arestas
    //---------------------------------------------------------------------------------------------------------------------------------------
    const string input_file = argv[1];
    int n, m;
    set<int> V;
    set<pair<int, int>> E;

    read_input_file(input_file, V, E, n, m);

    //criacao do modelo
    //---------------------------------------------------------------------------------------------------------------------------------------
    clock_t start_time = clock();
    IloEnv env;
    IloModel model(env);

    IloArray<IloBoolVarArray> x(env, n);
    IloBoolVarArray w(env, n); 

    //i_index: vertice v -> indice i associado ao vertice v em x_ij
    //considerando que os vertices podem ser nomeados com valores arbitrarios: V={89, 6, 1}, por exemplo
    map<int, int> i_index;

    //preenchimento e adicao das variaveis w_j e x_ij
    //---------------------------------------------------------------------------------------------------------------------------------------
    char w_name[25];
    for(int j=0; j<n; j++) {
        sprintf(w_name, "w_%d", j+1);
        w[j] = IloBoolVar(env, w_name);
        model.add(w[j]);
    }
    
    int i = 0;
    char x_name[25];
    for(int v : V) {
        i_index[v] = i;

        x[i] = IloBoolVarArray(env, n);
        for(int j=0; j<n; j++) {
            sprintf(x_name, "x_%d_%d", v, j+1);
            x[i][j] = IloBoolVar(env, x_name);
            model.add(x[i][j]);
        }
        i++;
    }

    //funcao objetivo
    //---------------------------------------------------------------------------------------------------------------------------------------
    IloExpr fo(env);
    for(int j=0; j<n; j++) {
        fo += w[j];
    }
    model.add(IloMinimize(env, fo, "fo"));
    fo.end();

    //restricoes do tipo (2)
    //---------------------------------------------------------------------------------------------------------------------------------------
    IloConstraintArray constraints_2(env); 
    for(int v : V) {
        IloExpr constraint(env);
        i = i_index[v];
        for(int j=0; j<n; j++) {
            constraint += x[i][j];
        }
        constraints_2.add(constraint == 1);
        constraint.end();
    }
    model.add(constraints_2);
    constraints_2.end();

    //restricoes do tipo (3)
    //---------------------------------------------------------------------------------------------------------------------------------------
    IloConstraintArray constraints_3(env); 
    int i_E, k_E;
    for(const auto& [i, k] : E) {
        i_E = i_index[i];
        k_E = i_index[k];
        for(int j=0; j<n; j++) {
            constraints_3.add(x[i_E][j] + x[k_E][j] <= w[j]);
        }
    }
    model.add(constraints_3);
    constraints_3.end();

    //restricoes do tipo (4)
    //---------------------------------------------------------------------------------------------------------------------------------------
    IloConstraintArray constraints_4(env); 
    int num = n-1;
    for(int j=0; j<num; j++) {
        constraints_4.add(w[j] >= w[j+1]);
    }
    model.add(constraints_4);
    constraints_4.end();

    //restricoes do tipo (5)
    //---------------------------------------------------------------------------------------------------------------------------------------
    IloConstraintArray constraints_5(env); 
    for(int j=0; j<n; j++) {
        IloExpr constraint(env);
        for(int v : V) {
            i = i_index[v];
            constraint += x[i][j];
        }
        constraints_5.add(w[j] <= constraint);
        constraint.end();
    }
    model.add(constraints_5);
    constraints_5.end();

    //Escrita do modelo e execucao
    //---------------------------------------------------------------------------------------------------------------------------------------
    IloCplex solver(model);
    solver.exportModel(("modelo_" + input_file.substr(0, input_file.size() - 4) + ".lp").c_str());                 

    set_solver_config(solver, env);

    try {
        solver.solve();
    } catch(IloException& e) {
        cout << e;
    }

    double total_time = ((double) clock() - (double) start_time) / CLOCKS_PER_SEC;

    print_and_write_result(solver, input_file, total_time, n, x);

    env.end();
    return 0;
}