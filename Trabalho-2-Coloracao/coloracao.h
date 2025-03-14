#ifndef TRABALHO_2
#define TRABALHO_2

#include <iostream>
#include <string>
#include <stdio.h>
#include <fstream>
#include <map>
#include <utility>
#include <set> 
#include <ilcplex/ilocplex.h>

#define MAX_ERROR 0.00001

void set_solver_config(IloCplex& solver, IloEnv& env);

void read_input_file(const std::string& input_file, std::set<int>& V, std::set<std::pair<int, int>>& E, int& n, int& m);

void output_message(std::string message, std::ofstream& result_file);

void print_and_write_result(IloCplex& solver, std::string input_file, double total_time, int n, IloArray<IloBoolVarArray>& x);

void print_and_write_v_colors(IloCplex& solver, std::ofstream& result_file, int n, IloArray<IloBoolVarArray>& x);

#endif 