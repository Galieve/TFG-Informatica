#include "statistics.h"
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
    while(process < 0 || process_flag != 1){
        while(process < 0); 
        process_mtx.lock();
        if(process >= 0 && process_flag == 0)
            process_flag = 1;
        process_mtx.unlock();
    }
    process_mtx.lock();
    ++process;
    process_mtx.unlock();
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
    process_mtx.lock();
    --process;
    if(process == 0)
        process_flag = 0;
    process_mtx.unlock();
}

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
    output_results_protected(number_file);
}

void statistics::output_results_protected(const std::string & number_file,
    const std::string & file_type){

    std::ofstream out("../files/times"+number_file+"."+file_type);
    for(auto p: times){
        out << p.first << " "<< p.second<< "\n";
    }
    out << "\nTotal: "<< circuits_visited <<"\n";
    out.close();
    out.open("../files/gates"+number_file+"."+file_type);
    for(auto p: less_logic_gates){
        out << p.first << "\n" << *p.second.get_logic_circuit() << "\n";
    }
    out.close();
    out.open("../files/size"+number_file+"."+file_type);
    for(auto p: less_size){
        out << p.first << "\n" << *p.second.get_logic_circuit() << "\n";
    }
    out.close();
    out.open("../files/depth"+number_file+"."+file_type);
    for(auto p: less_depth){
        out << p.first << "\n" << *p.second.get_logic_circuit() << "\n";
    }
    out.close();
}

#define NO_PROCESS_RUNNING() \
    while(process > 0 || process_flag!= -1){ \
        while(process > 0); \
        process_mtx.lock(); \
        if(process <= 0 && process_flag == 0) \
           process_flag = -1; \
        process_mtx.unlock(); \
    } \
    process_mtx.lock(); \
    --process; \
    process_mtx.unlock();

#define LET_PROCESS_RUN() \
    process_mtx.lock(); \
    ++process; \
    if(process == 0) \
        process_flag = 0; \
    process_mtx.unlock();

void statistics::save_partial_results(){
    part_res_mtx.lock();
    partial_results_saved++;
    std::string file_name = std::to_string(partial_results_saved);
    NO_PROCESS_RUNNING()
    output_results(file_name);
    LET_PROCESS_RUN()
    part_res_mtx.unlock();
}

void statistics::save_log(const function_generator &fg){
    NO_PROCESS_RUNNING()
    log_mtx.lock();
    output_results_protected("_log", "log");
    LET_PROCESS_RUN()
    std::ofstream log_file(LOG_FILENAME, std::ofstream::out | std::ofstream::trunc);
    log_file << fg.get_depth() << " "<<fg.get_size() <<" " << fg.get_input_size() << "\n";
    if(fg.get_current() == nullptr)
        log_file << "nullptr\n";
    else
        log_file << *fg.get_current() << "\n";
    log_mtx.unlock();  
}

function_generator statistics::restore_log(){
    restore_times();
    restore_depth();
    restore_size();
    restore_logic_gates();

    std::ifstream log_file(LOG_FILENAME);

    std::size_t depth, size, input_size;
    log_file >> depth >> size >> input_size;
    std::vector<std::string> vs;
    std::string s = "";
    getline(log_file, s);
    while(getline(log_file, s), s!= ""){
        vs.push_back(s);
    }
    auto curr = node_info::generate_vvnode_info(vs);
    log_file.close();

    function_generator fg(depth, size, input_size);
    fg.set_current(curr);
    return fg;
}

#define RESTORE(FIELD) \
    std::string field_name = #FIELD; \
    std::ifstream in("../files/" + field_name + "_log.log"); \
    std::string id, s; \
    less_ ## FIELD.clear(); \
    while(getline(in, id), id != ""){ \
        std::vector<std::string> vs; \
        while(getline(in, s), s!= ""){ \
            vs.push_back(s); \
        } \
        auto spvvnode_info = node_info::generate_vvnode_info(vs); \
        less_ ## FIELD[id] = circuit(spvvnode_info, id); \
    } \
    in.close();

void statistics::restore_depth(){
    RESTORE(depth)
}

void statistics::restore_size(){
    RESTORE(size)
}

void statistics::restore_logic_gates(){
    RESTORE(logic_gates)
}

void statistics::restore_times(){
    std::ifstream in("../files/times_log.log");
    std::string id, times_str;
    while(in >> id, id != "Total:"){
        in >> times_str;
        times[id] = InfInt(times_str);
    }
    in >> times_str;
    circuits_visited = InfInt(times_str);
    in.close();
}