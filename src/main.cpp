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
#include "dispatch/function_dispatcher.cpp"
#include "dispatch/stored_info.cpp"
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
    std::vector<std::thread> v_th;
    std::vector<function_dispatcher> vfd;
    std::cout <<"START\n";
#ifdef DEBUG_MODE
    auto upfg = 
        std::unique_ptr<function_generator>
            (new function_generator(4, 3, FULL_INPUT_SIZE));
    auto fd = function_dispatcher(upfg);
    v_th.emplace_back(std::thread(&function_dispatcher::dispatch_all,
        std::ref(fd)));
   
#else
    try{
        do{
            /*auto upfg = mfg.get();
            v_th.emplace_back(std::thread(use_all,std::move(upfg)));
            mfg.advance();*/
        }while(mfg.can_get_next());
    }
    catch(std::exception &e){
        std::ofstream err("files/error.err");
        std::cerr <<"Try_catch (2): " << e.what() << "\n";
        err << e.what() << "\n";
        return 0;
    }
#endif
    for(auto &i: v_th){
        if(i.joinable())
            i.join();
    }
    std::cout<<"END\n";
    return 0;
}
