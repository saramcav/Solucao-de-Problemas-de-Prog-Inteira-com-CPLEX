#ifndef IO_TRABALHO_1
#define IO_TRABALHO_1

#include <iostream>
#include <string>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <map>
#include <utility>
#include <set> 
#include <ilcplex/ilocplex.h>

#define MAX_ERROR 0.00001

void read_input_file(std::string file_name, std::map<std::pair<int, int>, std::pair<double, int>>& g_p, 
                    std::map<int, int>& facility_occurrences, std::set<int>& facility_ids, std::set<int>& clients_ids,
                    int& c, int& Q);

void output_message(std::string message, std::ofstream& result_file);

void print_and_write_result(IloCplex& solver, std::string input_file, double total_time, int ni, IloArray<IloBoolVarArray>& x);

void print_and_write_optimal_edges(IloCplex& solver, std::ofstream& result_file, int ni, IloArray<IloBoolVarArray>& x);

#endif 