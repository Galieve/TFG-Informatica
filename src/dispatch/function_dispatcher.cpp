#include "function_dispatcher.h"
#ifndef FUNCTION_H
    #include "../function/function.h"
#endif

void function_dispatcher::evaluate(std::size_t i, const std::shared_ptr<vvnode_info> &v, 
        std::size_t input_size){

    function f = function::build(*v, input_size);
    f.evaluate_all();
    std::pair<std::unique_ptr<function>, std::unique_ptr<vvnode_info>>
        p = std::make_pair(std::make_unique<function>(f),
                           std::make_unique<vvnode_info>(*v.get()));
    stat->update_statistics(circuit(v, f.get_id()));

}

void function_dispatcher::dispatch_all(){
    std::cout << "START (d: " << fg->get_depth() << ", s: " << fg->get_size() << ")\n";
    std::size_t cont = 0;
    auto v = fg->get_current();
    int mod = 0;
    long long contaux = 0;
    std::vector<data_task> tasks;
    do{

        tasks.push_back(std::make_pair(std::shared_ptr(v), std::make_pair(cont, fg->get_input_size())));
        
#ifdef DEBUG_MODE
        ++mod;
        if(mod == 10000){
            contaux++;
            mod = 0;
            std::cout << "vamos a por el: "<< contaux << "0000"  << std::endl;
        }
#endif
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
        ++cont;
        v = fg->generate_next();
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