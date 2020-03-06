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
#include "function/function_generator.h"
#include "function/node_info.h"
#include "function/function.h"
#include "function/node.h"
#include "function/meta_function_generator.h"
#include "dispatch/task_dispatcher.h"
#include "dispatch/function_dispatcher.h"
#include "dispatch/stored_info.h"
#include "statistics/statistics.h"
#include "function/circuit.h"
//https://github.com/adah1972/nvwa
#include "nvwa/debug_new.h"
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
