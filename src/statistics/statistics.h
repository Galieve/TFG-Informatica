#pragma once
#ifndef STATISTICS_H
#define STATISTICS_H
#ifndef DEFINE_H
    #include "../define.h"
#endif
#ifndef CIRCUIT_H
    #include "../function/circuit.h"
#endif
#include <mutex>
#include <map>
#include <string>
#include <utility>
#include <atomic>

class statistics{

protected:

    std::map<std::string, std::size_t> times;

    std::mutex times_mtx;

    std::map<std::string, circuit> less_gates;

    std::mutex gates_mtx;

    std::map<std::string, circuit> less_depth;

    std::mutex depth_mtx;

    std::map<std::string, circuit> less_size;

    std::mutex size_mtx;

    std::atomic<int> circuits_visited;

public:

    statistics();

    void update_statistics(const circuit & circ);

    void output_results();

protected:

    void update_times(const circuit &circ);

    void update_gates(const circuit &circ);

    void update_depth(const circuit &circ);

    void update_size(const circuit &circ);

};

#endif