#pragma once
#ifndef STORED_INFO_H
#define STORED_INFO_H
#include <memory>
#include <vector>
#include <fstream>
#include <mutex>
#include <thread>
#include <functional>
#include <condition_variable>
#include <sstream>
#ifndef FUNCTION_H
    #include "../function/function.h"
#endif
#ifndef DEFINE_H
    #include "../define.h"
#endif

class stored_info{

protected:

    std::unordered_map<int, 
        std::pair<std::unique_ptr<function>,std::unique_ptr<vvnode_info>> 
        > function_list;

    std::mutex cont_mtx;

    std::size_t cont;

    std::ofstream out;

#ifdef SIMPLE_LOG

    std::stringstream ss;

#endif

    std::mutex signal_mtx;

    bool end_signal;

    bool signal_sended;

    std::condition_variable * cv;

    std::size_t num_elements;

    std::mutex thread_mtx;

    std::vector<std::thread> thread_vector;


public:

    stored_info(const std::string &name) : cont(0), function_list(0),
        out(name), signal_sended(false), end_signal(false), num_elements(0) { };

    void fill_position(std::size_t pos, 
        const std::pair<std::unique_ptr<function>,std::unique_ptr<
        vvnode_info>> &p);

    bool end(std::condition_variable &cv);

protected:

    void output_next();

    void send_signal();

};

#endif
