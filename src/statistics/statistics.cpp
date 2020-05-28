#include "statistics.h"
#ifndef TASK_DISPATCHER_H   
    #include "../dispatch/task_dispatcher.h"
#endif
#include <fstream>
#include <climits>
#include <string>
#include <algorithm>

statistics::statistics() : circuits_visited(0), 
    partial_results_saved(0) {};

statistics::statistics(int number_file) : circuits_visited(0), 
    partial_results_saved(number_file) {};

int statistics::get_saved(){
    return partial_results_saved;
}

void conditional_print(const vvnode_info &v, std::ofstream &uno){

    for(int i = 0; i < v.size(); ++i){
        for(int j = 0; j < v[i].size(); ++j){
            auto n = v[i][j];
            if(!n.is_gate()){
                uno <<"c "<< n.get_cable_id()<<" ";
            }
            else if(n.is_bypass()){
                auto cab = n.get_cables();
                uno << "b "<< cab->left.second << " "<< cab->right.second<<" ";
            }
            else if(n.is_and_gate()){
                auto cab = n.get_cables();
                uno << "a "<< cab->left.second << " "<< cab->right.second<<" ";
            }
            else{
                auto cab = n.get_cables();
                uno << "o "<< cab->left.second << " "<< cab->right.second<<" ";
            }
        }
        uno << "\n";
    }
    uno << "\n";
    
}

void statistics::update_statistics(const circuit &circ){
    update_times(circ);
    auto itlg = less_logic_gates.find(circ.get_id());
    if(itlg == less_logic_gates.end() || itlg->second.get_logic_gates() > circ.get_logic_gates()){
        update_logic_gates(circ);
    }
    auto itld = less_depth.find(circ.get_id());
    if(itld == less_depth.end() || itld->second.get_depth() > circ.get_depth()){
        update_depth(circ);
    }
    auto itls = less_size.find(circ.get_id());
    if(itls == less_size.end() || itls->second.get_size() > circ.get_size()){
        update_size(circ);
    }
#ifdef PRODUCTION_MODE
    flush_mtx.lock();
    if(times.size() >= MAX_SIZE){
        flush_data();
    }
    flush_mtx.unlock();  
        
#endif
}

#ifdef PRODUCTION_MODE
void statistics::flush_data(){

    output_results_protected(folder,std::to_string(data_files_created));
    ++data_files_created;
    less_depth.clear();
    less_size.clear();
    less_logic_gates.clear();
    times.clear();
}
#endif

void statistics::update_times(const circuit &circ){
    times_mtx.lock();
    circuits_visited += 1;
    auto it = times.find(circ.get_id());
    if(it == times.end()){
        times[circ.get_id()] = 1;
    }
    else{
        times[circ.get_id()] += 1;
    }
    times_mtx.unlock();
}

#define UPDATE(FIELD) \
    FIELD ## _mtx.lock(); \
    auto it = less_ ## FIELD.find(circ.get_id()); \
    if(it == less_ ## FIELD.end()){ \
        less_ ## FIELD [circ.get_id()] = circ; \
    } \
    else if(it->second.get_## FIELD() > circ.get_## FIELD()){ \
        less_ ## FIELD [circ.get_id()] = circ; \
    } \
    FIELD ## _mtx.unlock(); \

void statistics::update_logic_gates(const circuit &circ){
    UPDATE(logic_gates)
}

void statistics::update_depth(const circuit &circ){
    UPDATE(depth)
}

void statistics::update_size(const circuit &circ){
    UPDATE(size)
}

void statistics::output_results(const std::string &number_file){
#ifndef PRODUCTION_MODE   
    output_results_protected("../files/circuit/",number_file);
#else
    output_results_protected("../files/production/",number_file);
#endif
}

void statistics::output_results_protected(const std::string &folder,
    const std::string & number_file,
    const std::string & file_type){

    std::ofstream out(folder + "times"+number_file+"."+file_type);
    for(auto p: times){
        out << p.first << " "<< p.second<< "\n";
    }
    out << "\nTotal: "<< circuits_visited <<"\n";
    out.close();
    out.open(folder + "gates"+number_file+"."+file_type);
    for(auto p: less_logic_gates){
        out << p.first << "\n" << *p.second.get_logic_circuit() << "\n";
    }
    out.close();
    out.open(folder + "size"+number_file+"."+file_type);
    for(auto p: less_size){
        out << p.first << "\n" << *p.second.get_logic_circuit() << "\n";
    }
    out.close();
    out.open(folder + "depth"+number_file+"."+file_type);
    for(auto p: less_depth){
        out << p.first << "\n" << *p.second.get_logic_circuit() << "\n";
    }
    out.close();
}

void statistics::save_partial_results(){
    part_res_mtx.lock();
    partial_results_saved++;
    std::string file_name = std::to_string(partial_results_saved);
    output_results(file_name);
    part_res_mtx.unlock();
}

void statistics::save_log(const function_generator &fg){
    output_results_protected("../files/log/", "_log", "log");
    std::ofstream log_file(LOG_FILENAME, std::ofstream::out | std::ofstream::trunc);
    log_file << fg.get_depth() << " "<<fg.get_size() <<" " << fg.get_input_size() << "\n";
    log_file << data_files_created+1 <<"\n";
    if(fg.get_current() == nullptr)
        log_file << "nullptr\n";
    else
        log_file << *fg.get_current() << "\n";
    
}

function_generator statistics::restore_log(){
    auto td = task_dispatcher::get_instance();
    std::vector<std::function<void()>> vf;
    vf.emplace_back([s = std::move(this)](){
        s->restore_times();
    });
    vf.emplace_back([s = std::move(this)](){
        s->restore_logic_gates();
    });
    vf.emplace_back([s = std::move(this)](){
        s->restore_size();
    });
    vf.emplace_back([s = std::move(this)](){
        s->restore_depth();
    });
    for(auto &f: vf){
        td.dispatch_task(f);
    }

    std::ifstream log_file(LOG_FILENAME);

    std::size_t depth, size, input_size;
    log_file >> depth >> size >> input_size;
    log_file >> data_files_created;
    std::vector<std::string> vs;
    std::string s = "";
    getline(log_file, s);
    while(getline(log_file, s), s!= ""){
        vs.emplace_back(s);
    }
    auto curr = node_info::generate_vvnode_info(vs);
    log_file.close();

    function_generator fg(depth, size, input_size);
    fg.set_current(curr);
    while(!td.is_empty());
    return fg;
}

#define RESTORE(FIELD, FIELD_NAME) \
    std::string field_name = #FIELD_NAME; \
    std::ifstream in("../files/log/" + field_name + "_log.log"); \
    std::string id, s; \
    less_ ## FIELD.clear(); \
    std::vector<std::string> vs; \
    while(getline(in, id), id != ""){ \
        vs.clear(); \
        while(getline(in, s), s!= ""){ \
            vs.emplace_back(s); \
        } \
        auto spvvnode_info = node_info::generate_vvnode_info(vs); \
        less_ ## FIELD[id] = circuit(spvvnode_info, id); \
    } \
    in.close();

void statistics::restore_depth(){
    RESTORE(depth, depth)
}

void statistics::restore_size(){
    RESTORE(size, size)
}

void statistics::restore_logic_gates(){
    RESTORE(logic_gates, gates)
}

void statistics::restore_times(){
    std::ifstream in("../files/log/times_log.log");
    times.clear();
    std::string id, times_str;
    while(in >> id, id != "Total:"){
        in >> times_str;
        times[id] = InfInt(times_str);
    }
    in >> times_str;
    circuits_visited = InfInt(times_str);
    in.close();
}