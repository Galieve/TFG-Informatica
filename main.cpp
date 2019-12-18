#include <vector>
#include <iostream>
#include <bitset>
#include <string>
#include <iomanip>
#include <cmath>
#include <set>
#include <cassert>
#include <functional>
#include "function_generator.cpp"
#include "node_info.cpp"
#include "function.cpp"
#include "node.cpp"
#include "meta_function_generator.cpp"
//https://github.com/adah1972/nvwa
#include "nvwa/debug_new.cpp"
#include <thread>
#include <fstream>

void use_all(const std::unique_ptr<function_generator> &fg){
    std::ofstream out("files/output_file(d="+std::to_string(fg->get_depth())+
        +",s="+std::to_string(fg->get_size())+",i="
        +std::to_string(fg->get_input_size())+").out");
    std::cout << "START (d: " << fg->get_depth()<<", s: "<<fg->get_size()<<")\n";
    int cont = 0;
    std::vector<std::vector<node_info>> * v = fg->get_current();
    function f = function::build(*v, fg->get_input_size());
    f.evaluate_all();
    out <<"Function number: "<<cont<<"\n";
    out << *v << "The id of this function is: "
        <<f.get_id()<<"\n-----\n";
    while((v = fg->generate_next()), v!= nullptr){
        ++cont;
        f = function::build(*v, fg->get_input_size());
        f.evaluate_all();
        out <<"Function number: "<<cont<<"\n";
        out << *v << "The id of this function is: "
            <<f.get_id()<<"\n-----\n";
    }
    out << "END\n";
    std::cout << "END (d: " << fg->get_depth()<<", s: "<<fg->get_size()<<")\n";
}


int main(){
    meta_function_generator mfg(10, 10, FULL_INPUT_SIZE);
    std::vector<std::thread> v_th;
    std::cout <<"START\n";
    do{
        auto upfg = mfg.get();
        std::thread t(use_all,std::move(upfg));
        v_th.push_back(std::move(t));
        mfg.advance();
    }while(mfg.can_get_next());
    for(auto &i: v_th){
        i.join();
    }
    std::cout<<"END\n";
    return 0;
}
/*
OR ( 0,  1)
OR ( 0,  0) | OR ( 1,  2)
AND( 0,  0) | AND( 0,  1) | AND( 1,  1)
CABLE ( 2 ) | CABLE ( 3 )
*/