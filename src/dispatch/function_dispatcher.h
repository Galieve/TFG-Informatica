#pragma once
#ifndef FUNCTION_DISPATCHER_H
#define FUNCTION_DISPATCHER_H
#ifndef DEFINE_H
    #include "../define.h"
#endif
#include <memory>
#include <string>
#include <utility>
#include <atomic>
#include "../CTPL/ctpl_stl.h"
#ifndef FUNCTION_GENERATOR_H
    #include "../function/function_generator.h"
#endif
#ifndef STORED_INFO_H
    #include "../dispatch/stored_info.h"
#endif
#ifndef TASK_DISPATCHER_H
    #include "task_dispatcher.h"
#endif
#ifndef STATISTICS_H
    #include "../statistics/statistics.h"
#endif


class function_dispatcher{

protected:
    std::unique_ptr<function_generator> fg;

    stored_info si;

    task_dispatcher t_disp;

    statistics * stat;
    

public:
    function_dispatcher(const std::unique_ptr<function_generator> &upfg, 
        statistics * stat) : fg(std::make_unique<function_generator>(*upfg)),
            si(generate_name_store_info(upfg)), 
            t_disp(task_dispatcher::get_instance()),
            stat(stat) {};

    // function_dispatcher(const std::unique_ptr<function_generator> &upfg)
    //      : fg(std::make_unique<function_generator>(*upfg)),
    //         si(generate_name_store_info(upfg)), 
    //         t_disp(task_dispatcher::get_instance())
    //        {};

    void dispatch_all();

protected:

    void evaluate(std::size_t i, const std::shared_ptr<vvnode_info> &v, 
        std::size_t input_size, stored_info &si);

    std::string generate_name_store_info
        (const std::unique_ptr<function_generator> &fg);
};


#endif