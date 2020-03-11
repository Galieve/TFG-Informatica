#include "stored_info.h"

void stored_info::fill_position(std::size_t pos, 
    const std::pair<std::unique_ptr<function>,std::unique_ptr<
        vvnode_info>> &p){
    cont_mtx.lock();
    ++num_elements;
    function_list[pos] = 
        std::make_pair(std::make_unique<function>(*p.first.get()),
         std::make_unique<vvnode_info>(*p.second.get()));
    if(pos == cont){
        cont_mtx.unlock();
        thread_mtx.lock();
        output_next();
        //thread_vector.emplace_back(std::thread(&stored_info::output_next, this));
        thread_mtx.unlock();
    }
    else cont_mtx.unlock();
}

void stored_info::output_next(){
    cont_mtx.lock();
    assert(cont < num_elements);
    out << "The id of this function is: "
        << function_list[cont].first->get_id()<<"\n-----\n";

    function_list.erase(cont);
    ++cont;
    if(cont < num_elements && function_list[cont].first != nullptr){
        cont_mtx.unlock();
        output_next();
    }
    else{
        signal_mtx.lock();
        if(end_signal && cont == num_elements){
            out << "END\n";
            out.close();
            cont_mtx.unlock();
            signal_mtx.unlock();
            send_signal();
        }
        else{
            cont_mtx.unlock();
            signal_mtx.unlock();
        } 
    } 

}

bool stored_info::end(std::condition_variable & cv){
    signal_mtx.lock();
    end_signal = true;
    this->cv = &cv;
    if(!signal_sended && cont == num_elements){
        out << "END\n";
        out.close();
        signal_mtx.unlock();
        send_signal();
        return true;
    }
    else if(signal_sended){
        signal_mtx.unlock();
        return true;
    }
    else{
        signal_mtx.unlock();
        return false;
    }
}

void stored_info::send_signal(){
    signal_sended = true;
    cv->notify_one();
    for(auto &i: thread_vector){
        if(i.joinable())
            i.join();
    }
}