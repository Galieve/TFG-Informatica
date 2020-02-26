#include "function_dispatcher.h"

void function_dispatcher::evaluate(std::size_t i, const std::shared_ptr<vvnode_info> &v, 
        std::size_t input_size, stored_info &si){

    function f = function::build(*v, input_size);
    f.evaluate_all();
    std::pair<std::unique_ptr<function>, std::unique_ptr<vvnode_info>>
        p = std::make_pair(std::make_unique<function>(f),
                           std::make_unique<vvnode_info>(*v.get()));
    //si.fill_position(i, std::ref(p));
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
                    fd->evaluate(i.second.first, i.first, fd->fg->get_input_size(), std::ref(fd->si));
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
            fd->evaluate(i.second.first, i.first, fd->fg->get_input_size(), std::ref(fd->si));
        }
    };
    //tpool.push(f);
    //tpool.stop(true);
    t_disp.dispatch_task(f);
    t_disp.stop();
    std::cout << "END (d: " << fg->get_depth() << ", s: " << fg->get_size() << ")\n";
}



std::string function_dispatcher::generate_name_store_info
    (const std::unique_ptr<function_generator> &fg){

#ifdef FULL_LOG
    return "../files/output_file(d="+std::to_string(fg->get_depth())+
        +",s="+std::to_string(fg->get_size())+",i="
        +std::to_string(fg->get_input_size())+").out";
#elif defined(SIMPLE_LOG)
    return "../files/simple_output_file(d="+std::to_string(fg->get_depth())+
        +",s="+std::to_string(fg->get_size())+",i="
        +std::to_string(fg->get_input_size())+").out";
#else
    return "";
#endif
}