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
    #include "define.h"
#endif
#include <thread>
#include <fstream>
#include <functional>
#include <atomic>

#define BAD_USAGE(I) \
    ++I; \
    if(I == argc){ \
        std::cerr<< "Bad usage\n"; \
        return -1; \
    }

int main(int argc, char * argv[]){
    int max_depth = 8, file_saved = 0;
    std::unique_ptr<function_generator> upfg_log = nullptr;
    bool restore_log = false;
    for(int i = 1; i < argc; ++i){
        if(std::string(argv[i]) == "--max_depth"){
            BAD_USAGE(i)
            max_depth = std::atoi(argv[i]);
        }
        else if(std::string(argv[i]) == "--files_completed"){
            BAD_USAGE(i)
            file_saved = std::atoi(argv[i]);
        }
        else if(std::string(argv[i]) == "--restore_log"){
            restore_log = true;
            
        }
    }

    statistics stat(file_saved);
    if(restore_log){
        upfg_log = std::make_unique<function_generator>(stat.restore_log());
        std::cout << "Log restored\n";
    }
        
    meta_function_generator mfg(max_depth, max_depth, FULL_INPUT_SIZE);
    std::vector<function_dispatcher> vfd;
    std::cout <<"START\n";   
    int count = 0;

    do{
        auto upfg = mfg.get();
        if(upfg_log != nullptr && upfg->get_depth() == upfg_log->get_depth() && 
            upfg->get_size() == upfg_log->get_size() &&
            upfg->get_input_size() == upfg_log->get_input_size()){
            
            //To make it consistent with "fully" saved files.
            upfg = std::make_unique<function_generator>(*upfg_log);
        }
        auto fd = function_dispatcher(std::move(upfg), stat);
        if(stat.get_saved() == count)
            fd.dispatch_all();
        //else: we skip this value.
        ++count;
        mfg.advance();
        upfg = nullptr;
    }while(mfg.can_get_next());

    stat.output_results();
    std::cout<<"END\n";
    return 0;
}

/*
OR ( 0,  1)
AND( 0,  2) | AND( 1,  2)
AND( 0,  5) | AND( 1,  4) | AND( 2,  3)
CABLE ( 4 ) | CABLE ( 5 ) | CABLE ( 6 ) | CABLE ( 7 ) | CABLE ( 8 ) | CABLE ( 9 )

*/