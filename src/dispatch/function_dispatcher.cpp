#include "function_dispatcher.h"
#ifndef FUNCTION_H
    #include "../function/function.h"
#endif
#include "../infint/InfInt.h"

//11111111111111111111111101000000
//00000000000000000000000010111111
void function_dispatcher::evaluate(std::size_t i, const std::shared_ptr<vvnode_info> v, 
        std::size_t input_size){

#ifndef PRODUCTION_MODE            
    function f = function::build(*v, fg->get_size(), input_size);
    f.evaluate_all();
    std::pair<std::unique_ptr<function>, std::unique_ptr<vvnode_info>>
        p = std::make_pair(std::make_unique<function>(f),
                           std::make_unique<vvnode_info>(*v.get()));
    stat->update_statistics(circuit(v, f.get_id()));
#else
    
    function f = function::build(*v, this->fg->get_size(), input_size);
    auto f_eq = f.get_function_equivalent();
    stat->update_statistics(circuit(v, f_eq));    

#endif

}

#ifdef DEBUG_MODE

void conditional_print(const vvnode_info &v, std::ostream &out){
    int i = v.size() - 1;
    int l = v[i].size();
    for(int j = 0; j < v[i].size(); ++j){
        if(v[i][j].get_cable_id() != FULL_INPUT_SIZE - l + j )
            return;
    }
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
        {"o 0 1 ", "b 0 0 o 1 2 ", "b 0 0 o 1 2 o 3 4 ", 
        "a 0 3 a 1 3 o 2 3 a 2 3 b 3 3 ", "b 0 0 o 0 1 a 0 1 b 1 1 ", "c 0 c 1" };
    auto curr = node_info::generate_vvnode_info(vs);
    conditional_print(*curr, std::cout);
    fg->set_current(curr);
    auto v = fg->generate_next();
    conditional_print(*v, std::cout);
}
#endif

void function_dispatcher::dispatch_all(){
    std::cout << "START (d: " << fg->get_depth() << ", s: " << fg->get_size() << ")\n";
    std::size_t cont = 0;
    long long int log_cont = 0;
#ifdef DEBUG_MODE
    //debug_function();
    //return;
    long long int count = 0;
#endif

    auto v = fg->get_current();
    int mod = 0;
    long long contaux = 0;
    std::vector<data_task> tasks;
    
    do{
        if(v != nullptr)
            tasks.emplace_back(std::make_pair(std::shared_ptr(v), std::make_pair(cont, fg->get_input_size())));
#ifdef DEBUG_MODE
        conditional_print(*v, std::cout);
#endif
#ifdef WHERE_AM_I_MODE
        ++mod;
        if(mod == PATH_FRAGMENT_SIZE){
            contaux++;
            mod = 0;
            std::cout << "vamos a por el: "<< InfInt(contaux*PATH_FRAGMENT_SIZE)  << "\n";
        }
#endif
        if(tasks.size() == TASK_SIZE){
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
        ++log_cont;
        //task_size | task_size * scale_factor => tasks is empty
        if(log_cont == TASK_SIZE * SCALE_FACTOR){
            while(!t_disp.is_empty());
            stat->save_log(*fg);
            log_cont = 0;
        }


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
    stat->save_log(*fg);
    std::cout << "END (d: " << fg->get_depth() << ", s: " << fg->get_size() << ")\n";
}