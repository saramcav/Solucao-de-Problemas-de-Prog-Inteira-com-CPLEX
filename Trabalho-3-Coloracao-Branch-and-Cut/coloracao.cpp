#include "coloracao.h"

//---------------------------------------------------------------------------------------------------------------------------------------
//ALUNA: SARA MAIA CAVALCANTE - GRADUACAO
//---------------------------------------------------------------------------------------------------------------------------------------

using namespace std;

typedef set<pair<int, int>> EdgeSet;

void set_solver_config(IloCplex& solver, IloEnv& env) {
    solver.setParam(IloCplex::Param::WorkMem, 1024 * 2);
    solver.setParam(IloCplex::Param::MIP::Strategy::File, 2);
    //solver.setParam(IloCplex::Param::TimeLimit, 3600);
    solver.setParam(IloCplex::Param::Threads, 1); 
    solver.setParam(IloCplex::Param::MIP::Interval, 100);
    //solver.setOut(env.getNullStream()); 

    solver.setParam(IloCplex::Param::Preprocessing::Presolve, 0);   
    solver.setParam(IloCplex::Param::MIP::Limits::CutsFactor, 0);      
    solver.setParam(IloCplex::Param::MIP::Strategy::HeuristicFreq, -1);  
}

/*
Para cada cor j=1...n faça
    • monte grafo residual Gj
    • enquanto <critério de parada>
      • encontrar clique maximal S violada (heurística) e inserir no modelo
*/
ILOUSERCUTCALLBACK5(CB_mincut, IloArray<IloBoolVarArray>&, x, IloBoolVarArray&, w, int, n, EdgeSet&, E, int*, num_c)  {
    IloEnv env  = getEnv();
    pair<set<int>, bool> greedy_S;
    set<int> maximal_S;
    bool violated_S;
    
    for(int j=0; j<n; j++) {
        double w_j = getValue(w[j]);

        vector<double> x_j_values(n);
        for (int i = 0; i < n; i++) {
            x_j_values[i] = getValue(x[i][j]);
        }

        VertexWeightedGraph G_j(n);
        fill_residual_graph(G_j, E, x_j_values);
        
        greedy_S = greedy_maximal_clique(G_j, w_j);
        maximal_S = greedy_S.first;
        violated_S = greedy_S.second;

        //enquanto for encontrada uma clique violada, adiciona-se o corte no modelo e os vertices contidos nessa clique sao marcados
        //apos marca-los, procura-se uma nova clique com os vertices restantes (nao marcados), pois greedy_maximal_clique so considera vertices desmarcados
        while (violated_S) {
            IloExpr user_cut(env);
            for (int i : maximal_S) {
                user_cut += x[i][j];
                G_j.mark_vertex(i);
            }
            add(user_cut <= w[j]); 
            user_cut.end();

            (*num_c)++;
    
            greedy_S = greedy_maximal_clique(G_j, w_j);
            maximal_S = greedy_S.first;
            violated_S = greedy_S.second;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cerr <<  "Se quiser cortes de usuário:" << endl;
        cerr << "\tExecute com: " << argv[0] << " nome_do_arquivo.txt 1" << endl;
        cerr <<  "Caso contrário::" << endl;
        cerr << "\tExecute com: " << argv[0] << " nome_do_arquivo.txt 0" << endl;
        return 1;
    }

    int user_cuts = stoi(argv[2]);
    if(user_cuts) {
        cout << "Executando " << argv[1] << " COM cortes de usuário...\n\n" << endl;
    } else {
        cout << "Executando "  << argv[1] << " SEM cortes de usuário...\n\n" << endl;
    }

    //TRABALHO ANTIGO
    //------------------------------------------------------------------------------------------------------------------------------
    const string input_file = argv[1];
    int n, m;
    set<int> V;
    set<pair<int, int>> E;

    read_input_file(input_file, V, E, n, m);

    clock_t start_time = clock();
    IloEnv env;
    IloModel model(env);

    IloArray<IloBoolVarArray> x(env, n);
    IloBoolVarArray w(env, n); 

    map<int, int> i_index;

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

    IloExpr fo(env);
    for(int j=0; j<n; j++) {
        fo += w[j];
    }
    model.add(IloMinimize(env, fo, "fo"));
    fo.end();


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


    IloConstraintArray constraints_4(env); 
    int num = n-1;
    for(int j=0; j<num; j++) {
        constraints_4.add(w[j] >= w[j+1]);
    }
    model.add(constraints_4);
    constraints_4.end();

    
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


    IloCplex solver(model);           
    //solver.exportModel(("modelo_" + input_file.substr(0, input_file.size() - 4) + ".lp").c_str());          
    

    //NOVA PARTE
    //------------------------------------------------------------------------------------------------------------------------------
    int num_c = 0;

    if (user_cuts) {
        solver.use(CB_mincut(env, x, w, n, E, &num_c));
    }

    set_solver_config(solver, env);

    try {
        solver.solve();
    } catch(IloException& e) {
        cout << e;
    }

    double total_time = ((double) clock() - (double) start_time) / CLOCKS_PER_SEC;

    print_and_write_result(solver, input_file, total_time, n, x, num_c, user_cuts);

    env.end();
    return 0;
}