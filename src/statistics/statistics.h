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
#ifndef FUNCTION_GENERATOR_H
    #include "../function/function_generator.h"
#endif
#include <mutex>
#include <map>
#include <string>
#include <utility>
#include <atomic>

class statistics{

protected:

    std::map<std::string, InfInt> times;

    std::mutex times_mtx;

    std::map<std::string, circuit> less_logic_gates;

    std::mutex logic_gates_mtx;

    std::map<std::string, circuit> less_depth;

    std::mutex depth_mtx;

    std::map<std::string, circuit> less_size;

    std::mutex size_mtx;

    InfInt circuits_visited;

    std::mutex part_res_mtx;

    int partial_results_saved;

    const std::string log_file = LOG_FILENAME;

public:

    statistics();

    statistics(int number_file);

    void update_statistics(const circuit & circ);

    void output_results(const std::string & number_file = "");

    void save_partial_results();

    int get_saved();

    void save_log(const function_generator &fg);

    function_generator restore_log();

protected:

    void output_results_protected(const std::string & number_file,
        const std::string & file_type = "out");

    void update_times(const circuit &circ);

    void update_logic_gates(const circuit &circ);

    void update_depth(const circuit &circ);

    void update_size(const circuit &circ);

    void restore_times();

    void restore_logic_gates();

    void restore_depth();

    void restore_size();

};

#endif