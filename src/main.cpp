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

int main(int argc, char * argv[]){
    if(argc > 3){
        std::cerr<< "Bad usage\n";
        return -1;
    }

    int max_diago = 4;
    if(argc == 3)
        max_diago = std::atoi(argv[2]);
    meta_function_generator mfg(max_diago, max_diago, FULL_INPUT_SIZE);
    std::vector<function_dispatcher> vfd;
    std::cout <<"START\n";

    int file_saved = 0;
    if(argc >= 2){
       file_saved = std::atoi(argv[1]);
    }
    statistics stat(file_saved);
    
    int count = 0;
    do{
        
        auto upfg = mfg.get();
        auto fd = function_dispatcher(std::move(upfg), stat);
        if(stat.get_saved() == count)
            fd.dispatch_all();
        //else: we skip this value.
        ++count;
        mfg.advance();
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