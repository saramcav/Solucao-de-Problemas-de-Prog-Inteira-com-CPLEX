#include "io.h"

//---------------------------------------------------------------------------------------------------------------------------------------
//ALUNA: SARA MAIA CAVALCANTE - GRADUACAO
//---------------------------------------------------------------------------------------------------------------------------------------

using namespace std;

void set_solver_config(IloCplex& solver, IloEnv& env);

void set_solver_config(IloCplex& solver, IloEnv& env) {
    solver.setParam(IloCplex::Param::WorkMem, 1024 * 2);
    solver.setParam(IloCplex::Param::MIP::Strategy::File, 2);
    solver.setParam(IloCplex::Param::TimeLimit, 3600);
    solver.setParam(IloCplex::Param::Threads, 1);
    solver.setParam(IloCplex::Param::MIP::Interval, 100);
    solver.setOut(env.getNullStream());    
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Execute com: " << argv[0] << " nome_do_arquivo.txt" << endl;
        return 1;
    }

    //leitura do arquivo e captura dos dados
    //---------------------------------------------------------------------------------------------------------------------------------------
    string input_file = argv[1];
    set<int> facility_ids;
    set<int> client_ids;
    //g_p: (facilidade f, cliente c) -> (custo-atendimento, recursos-consumidos)
    map<pair<int, int>, pair<double, int>> g_p;
    //facility_occurrences: facilidade -> |N_out(facilidade)|
    map<int, int> facility_occurrences;
    //c: custo de construcao de cada facilidade, Q: quantidade maxima de recursos de cada facilidade
    int c, Q;

    read_input_file(input_file, g_p, facility_occurrences, facility_ids, client_ids, c, Q);


    //criacao do modelo
    //---------------------------------------------------------------------------------------------------------------------------------------
    clock_t start_time = clock();
    IloEnv env;
    IloModel model(env);

    int n_facilities = facility_ids.size();
    IloBoolVarArray y(env, n_facilities); 
    IloArray<IloBoolVarArray> x(env, n_facilities);
    
    int i, j, p_ij; 
    double g_ij;
    char x_name[25], y_name[25];
    int nf_clients, facility, client;
    
    //i_index: facilidade f -> indice i em y associado a f
    map<int, int> i_index;
    //ij_index: (facilidade f, cliente c) -> indice j em x associado ao cliente c na linha i de x associada a facilidade f 
    map<pair<int, int>, int> ij_index;


    //preenchimento e adicao das variaveis y_i e x_ij
    //---------------------------------------------------------------------------------------------------------------------------------------
    i = 0;
    for(int f : facility_ids) {
        sprintf(y_name, "y_%d", f);
        y[i] = IloBoolVar(env, y_name);
        model.add(y[i]);  
        i_index[f] = i;

        //alocando |N_out(f)| para a facilidade f alocada no indice i de y
        nf_clients = facility_occurrences[f];
        x[i] = IloBoolVarArray(env, nf_clients);
        j = 0;
        for(const auto& [facility_client, cost_resource] : g_p) {
            facility = facility_client.first;
            if(f == facility) {
                client = facility_client.second;
                sprintf(x_name, "x_%d_%d", facility, client);
                x[i][j] = IloBoolVar(env, x_name); 
                //note que a facilidade f esta alocada na linha de x no mesmo indice i em que foi alocada em y
                model.add(x[i][j]);

                ij_index[make_pair(facility, client)] = j;

                j++;
            }
        }
        i++;
    }


    //funcao objetivo
    //---------------------------------------------------------------------------------------------------------------------------------------
    int pos1, pos2;
    string var_name;
    IloExpr fo(env);
    for(int f : facility_ids) {
        i = i_index[f];
        fo += c * y[i];

        nf_clients = facility_occurrences[f];
        for(int j=0; j<nf_clients; j++) {
            var_name = x[i][j].getName();
            pos1 = var_name.find('_'); 
            pos2 = var_name.find('_', pos1 + 1); 
            client = stoi(var_name.substr(pos2 + 1));

            g_ij = g_p[make_pair(f, client)].first;

            fo += g_ij * x[i][j];
        }
    }
    model.add(IloMinimize(env, fo, "fo"));


    //restricoes do tipo (2)
    //---------------------------------------------------------------------------------------------------------------------------------------
    IloConstraintArray constraints_2(env); 
    for(int c : client_ids) {
        IloExpr constraint(env);
        for(const auto& [facility_client, cost_resource] : g_p) {
            client = facility_client.second;
            
            if(client == c) {
                facility = facility_client.first;
                i = i_index[facility];
                j = ij_index[make_pair(facility, client)];
                constraint += x[i][j];
            }
        }
        constraints_2.add(constraint == 1);
        constraint.end();
    }
    model.add(constraints_2);
    constraints_2.end();


    //restricoes do tipo (3) 
    //---------------------------------------------------------------------------------------------------------------------------------------
    IloConstraintArray constraints_3(env); 
    for(const auto& [facility_client, cost_resource] : g_p) {
        facility = facility_client.first;
        client = facility_client.second;
        i = i_index[facility];
        j = ij_index[make_pair(facility, client)];

        constraints_3.add(x[i][j] <= y[i]);
    }
    model.add(constraints_3);
    constraints_3.end();


    //restricoes do tipo (4)
    //---------------------------------------------------------------------------------------------------------------------------------------
    IloConstraintArray constraints_4(env); 
    for(int f : facility_ids) {
        IloExpr constraint(env);
        i = i_index[f];

        for(const auto& [facility_client, cost_resource] : g_p) {
            facility = facility_client.first;

            if(facility == f) {
                client = facility_client.second;
                p_ij = cost_resource.second;
                j = ij_index[make_pair(facility, client)];
                constraint += p_ij * x[i][j];
            }
        }
        constraints_4.add(constraint <= Q * y[i]);
        constraint.end();
    }    
    model.add(constraints_4);
    constraints_4.end();


    //Escrita e execucao do modelo
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

    print_and_write_result(solver, input_file, total_time, n_facilities, x);

    env.end();
    return 0;
}