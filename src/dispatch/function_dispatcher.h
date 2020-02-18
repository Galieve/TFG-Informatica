#pragma once
#ifndef FUNCTION_DISPATCHER_H
#define FUNCTION_DISPATCHER_H
#ifndef DEFINE_H
    #include "../function/define.h"
#endif
#include <memory>
#include <set>
#include <map>
#include <thread>
#include <atomic>
#include <string>
#include <mutex>
#include <utility>
#include "../CTPL/ctpl_stl.h"
#ifndef FUNCTION_GENERATOR_H
    #include "../function/function_generator.h"
#endif
#ifndef STORED_INFO_H
    #include "../dispatch/stored_info.h"
#endif

class function_dispatcher{

protected:
    std::unique_ptr<function_generator> fg;

    stored_info si;

    ctpl::thread_pool tpool;

public:
    function_dispatcher(const std::unique_ptr<function_generator> &upfg) 
        : fg(std::make_unique<function_generator>(*upfg)),
        si(generate_name_store_info(upfg)), 
        tpool(std::thread::hardware_concurrency()) {};

    void dispatch_all();

protected:

    void evaluate(std::size_t i, const std::unique_ptr<vvnode_info> &v, 
        std::size_t input_size, stored_info &si);

    std::string generate_name_store_info
        (const std::unique_ptr<function_generator> &fg);
};


#endif