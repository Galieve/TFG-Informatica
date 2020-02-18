#include "function_dispatcher.h"

void function_dispatcher::evaluate(std::size_t i,
    const std::unique_ptr<vvnode_info> &v, 
    std::size_t input_size, stored_info &si){

    function f = function::build(*v, input_size);
    f.evaluate_all();
    std::pair<std::unique_ptr<function>, std::unique_ptr<vvnode_info>>
        p = std::make_pair(std::make_unique<function>(f),
                           std::make_unique<vvnode_info>(*v.get()));
    si.fill_position(i, std::ref(p));

}

void function_dispatcher::dispatch_all(){
    std::cout << "START (d: " << fg->get_depth() << ", s: " << fg->get_size() << ")\n";
    std::size_t cont = 0;
    auto v = fg->get_current();
    int mod = 0;
    long long contaux = 0;
    std::vector<std::pair<std::unique_ptr<vvnode_info>,std::pair<int, int>>> tasks;
    do
    {
        // evaluate(cont, std::move(v), fg->get_input_size(), std::ref(si));
        // while (tpool.size() > std::thread::hardware_concurrency());
        tasks.push_back(std::make_pair(std::move(v), std::make_pair(cont, fg->get_input_size())));
        
        if(tasks.size() == 1000){
            bool set_all = false;
            tpool.push([this, t = std::move(tasks), &set_all](int id){
                for(auto &i: t){
                    evaluate(i.second.first, std::move(i.first), fg->get_input_size(), std::ref(si));
                }
                set_all = true;                
            });
            while(!set_all);            
            tasks.clear();
        }
        /*tpool.push([this, cont, v = std::move(v)](int id){
            evaluate(cont, std::move(v), fg->get_input_size(), std::ref(si));
        });*/
        
        /*finished_mtx.lock();
        thread_map[cont] = std::make_shared<std::thread>(
            std::thread(&function_dispatcher::evaluate, this, 
                cont, std::move(v), fg->get_input_size(), std::ref(si)));
        finished_mtx.unlock();
        ++num_threads;*/
#ifdef DEBUG_MODE
        ++mod;
        if(mod == 1000){
            contaux++;
            mod = 0;
            std::cout << "vamos a por el: "<< contaux << "000"  << std::endl;
        }
        
#endif
        ++cont;
        v = fg->generate_next();
    } while (v != nullptr);
    tpool.push([this, t = std::move(tasks)](int id){
        for(auto &i: t){
            evaluate(i.second.first, std::move(i.first), fg->get_input_size(), std::ref(si));
        }             
    });
    // std::condition_variable cv;
    // std::mutex mtx;
    // std::unique_lock<std::mutex> lck(mtx);
    // //un if porque si se le despierta, ya no se va a volver a dormir
    // if (!si.end(cv))
    //     cv.wait(lck);
    
    tpool.stop(true);

    std::cout << "END (d: " << fg->get_depth() << ", s: " << fg->get_size() << ")\n";
}



std::string function_dispatcher::generate_name_store_info
    (const std::unique_ptr<function_generator> &fg){

#ifdef FULL_LOG
    return "files/output_file(d="+std::to_string(fg->get_depth())+
        +",s="+std::to_string(fg->get_size())+",i="
        +std::to_string(fg->get_input_size())+").out";
#elif defined(SIMPLE_LOG)
    return "files/simple_output_file(d="+std::to_string(fg->get_depth())+
        +",s="+std::to_string(fg->get_size())+",i="
        +std::to_string(fg->get_input_size())+").out";
#else
    return "";
#endif
}