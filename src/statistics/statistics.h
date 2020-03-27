#pragma once
#ifndef STATISTICS_H
#define STATISTICS_H
#ifndef DEFINE_H
    #include "../define.h"
#endif
#ifndef CIRCUIT_H
    #include "../function/circuit.h"
#endif
#include "../infint/InfInt.h"
#include <mutex>
#include <map>
#include <string>
#include <utility>
#include <atomic>

class statistics{

protected:

    std::map<std::string, InfInt> times;

    std::mutex times_mtx;

    std::map<std::string, circuit> less_gates;

    std::mutex gates_mtx;

    std::map<std::string, circuit> less_depth;

    std::mutex depth_mtx;

    std::map<std::string, circuit> less_size;

    std::mutex size_mtx;

    InfInt circuits_visited;

    std::mutex part_res_mtx;

    int partial_results_saved;

    // std::ofstream uno;

    // std::ofstream dos;

public:

    statistics();

    statistics(int number_file);

    void update_statistics(const circuit & circ);

    void output_results(const std::string & number_file = "");

    void save_partial_results();

    int get_saved();

protected:

    void update_times(const circuit &circ);

    void update_gates(const circuit &circ);

    void update_depth(const circuit &circ);

    void update_size(const circuit &circ);

};

#endif