#include "function_dispatcher.h"
#ifndef FUNCTION_H
    #include "../function/function.h"
#endif

//11111111111111111111111101000000
//00000000000000000000000010111111
void function_dispatcher::evaluate(std::size_t i, const std::shared_ptr<vvnode_info> &v, 
        std::size_t input_size){

    function f = function::build(*v, input_size);
    f.evaluate_all();
    std::pair<std::unique_ptr<function>, std::unique_ptr<vvnode_info>>
        p = std::make_pair(std::make_unique<function>(f),
                           std::make_unique<vvnode_info>(*v.get()));
    stat->update_statistics(circuit(v, f.get_id()));

}

#ifdef DEBUG_MODE

void conditional_print(const vvnode_info &v, std::ostream &out){
    out << v << "\n";
}
/*
o 0 1 
b 0 0 a 1 2 
a 0 2 b 1 1 a 1 2 
c 3 c 7 c 8
*/


void function_dispatcher::debug_function(){
    std::vector<std::string> vs = 
        {"o 0 1 ", "a 0 2 a 1 2 ", "a 0 5 a 1 4 a 2 3 ", 
        "c 4 c 5 c 6 c 7 c 8 c 9 " };
    auto curr = node_info::generate_vvnode_info(vs);
    conditional_print(curr, std::cout);
    fg->set_current(curr);
    auto v = fg->generate_next();
    conditional_print(*v, std::cout);
}
#endif

void function_dispatcher::dispatch_all(){
    std::cout << "START (d: " << fg->get_depth() << ", s: " << fg->get_size() << ")\n";
    std::size_t cont = 0;

#ifdef DEBUG_MODE
    //debug_function();
    //return;
    long long int count = 0;
#endif

    auto v = fg->get_current();
    int mod = 0;
    long long contaux = 0;
    std::vector<data_task> tasks;
    
    //std::ofstream debug("aux.err");
    do{
    
        tasks.push_back(std::make_pair(std::shared_ptr(v), std::make_pair(cont, fg->get_input_size())));
#ifdef DEBUG_MODE
        conditional_print(*v, debug);
#endif
        ++mod;
        if(mod == 10000){
            contaux++;
            mod = 0;
            std::cout << "vamos a por el: "<< contaux << "0000"  << std::endl;
        }
        if(tasks.size() == 10000){
            bool set_all = false;
            while(t_disp.is_busy());
            std::function<void()> f = [fd = std::move(this), t = std::move(tasks), &set_all](){
                set_all = true;
                for(auto i: t){
                    fd->evaluate(i.second.first, i.first, fd->fg->get_input_size());
                }
            };
            t_disp.dispatch_task(f);
            while(!set_all);           
            tasks.clear();
        }
        v = fg->generate_next();

#ifdef DEBUG_MODE  
        // ++cont;
        // if(cont == 161699){
        //     std::cout<< "Vamos a morir\n";
        // }
#endif
        

    } while (v != nullptr);
    std::function<void()> f = [fd = std::move(this), t = std::move(tasks)](){
        for(auto i: t){
            fd->evaluate(i.second.first, i.first, fd->fg->get_input_size());
        }
    };
    t_disp.dispatch_task(f);
    t_disp.stop();
    stat->save_partial_results();
    std::cout << "END (d: " << fg->get_depth() << ", s: " << fg->get_size() << ")\n";
}