#include <vector>
#include <iostream>
#include <bitset>
#include <string>
#include <iomanip>
#include <cmath>
#include <set>
#include <cassert>
#include <functional>
#include <future>
#include "function/function_generator.cpp"
#include "function/node_info.cpp"
#include "function/function.cpp"
#include "function/node.cpp"
#include "function/meta_function_generator.cpp"
#include "dispatch/task_dispatcher.cpp"
#include "dispatch/function_dispatcher.cpp"
#include "dispatch/stored_info.cpp"
#include "statistics/statistics.cpp"
#include "function/circuit.cpp"
//https://github.com/adah1972/nvwa
#include "nvwa/debug_new.cpp"
#ifndef DEFINE_H
    #include "function/define.h"
#endif
#include <thread>
#include <fstream>
#include <functional>
#include <atomic>

int main(){
    meta_function_generator mfg(6, 6, FULL_INPUT_SIZE);
    std::vector<function_dispatcher> vfd;
    std::cout <<"START\n";
    statistics stat;
#ifdef DEBUG_MODE
    do{
        auto upfg = mfg.get();
        auto fd = function_dispatcher(std::move(upfg), &stat);
        fd.dispatch_all();
        mfg.advance();
    }while(mfg.can_get_next());
#else

    try{
        do{
            auto upfg = mfg.get();
            auto fd = function_dispatcher(std::move(upfg));
            fd.dispatch_all();
            mfg.advance();
        }while(mfg.can_get_next());
    }
    catch(std::exception &e){
        std::ofstream err("files/error.err");
        std::cerr <<"Try_catch (2): " << e.what() << "\n";
        err << e.what() << "\n";
        return 0;
    }
#endif
    stat.output_results();
    std::cout<<"END\n";
    return 0;
}
