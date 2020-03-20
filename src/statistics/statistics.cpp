#include "statistics.h"
#include <fstream>
#include <climits>
#include <string>
#include <algorithm>

statistics::statistics() : circuits_visited(0), partial_results_saved(0)
                            // ,uno("uno.txt"), 
                            // dos("dos.txt") 
                            {};


void conditional_print(const vvnode_info &v, std::ofstream &uno){
    // int l = v[v.size() -1].size();
    // for(int i = 0; i < l; ++i){
    //     if(v.back()[v.back().size() - l + i].get_cable_id() != FULL_INPUT_SIZE - (l - i)){
    //         return;
    //     }
    // }
    // ++contador;
    // std::cout << contador << "\n\n";
    // std::cout << v << "\n"; 
    

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
    ++circuits_visited;
    update_times(circ);
    auto itlg = less_gates.find(circ.get_id());
    if(itlg == less_gates.end() || itlg->second.get_logic_gates() > circ.get_logic_gates()){
        update_gates(circ);
    }
    auto itld = less_depth.find(circ.get_id());
    if(itld == less_depth.end() || itld->second.get_depth() > circ.get_depth()){
        update_depth(circ);
    }
    auto itls = less_size.find(circ.get_id());
    if(itls == less_size.end() || itls->second.get_size() > circ.get_size()){
        update_size(circ);
    }

}

void statistics::update_times(const circuit &circ){
    times_mtx.lock();
    auto it = times.find(circ.get_id());
    if(it == times.end()){
        times[circ.get_id()] = 1;
    }
    else{
        times[circ.get_id()] = std::min(1 + times[circ.get_id()], ULLONG_MAX - 1);
    }
    
    // if(circ.get_id() == "10101010101010101111111111111111"){
    //     conditional_print(*circ.get_logic_circuit(), uno);
    // }
    // else if(circ.get_id() == "01010101010101010000000000000000"){
    //     conditional_print(*circ.get_logic_circuit(), dos);
    // }
    times_mtx.unlock();
}

void statistics::update_gates(const circuit &circ){
    gates_mtx.lock();
    auto it = less_gates.find(circ.get_id());
    if(it == less_gates.end()){
        less_gates[circ.get_id()] = circ;
    }
    else if(it->second.get_logic_gates() > circ.get_logic_gates()){
        less_gates[circ.get_id()] = circ;
    }
    gates_mtx.unlock();
}

void statistics::update_depth(const circuit &circ){
    depth_mtx.lock();
    auto it = less_depth.find(circ.get_id());
    if(it == less_depth.end()){
        less_depth[circ.get_id()] = circ;
    }
    else if(it->second.get_depth() > circ.get_depth()){
        less_depth[circ.get_id()] = circ;
    }
    depth_mtx.unlock();
}

void statistics::update_size(const circuit &circ){
    size_mtx.lock();
    auto it = less_size.find(circ.get_id());
    if(it == less_size.end()){
        less_size[circ.get_id()] = circ;
    }
    else if(it->second.get_size() > circ.get_size()){
        less_size[circ.get_id()] = circ;
    }
    size_mtx.unlock();
}

void statistics::output_results(const std::string & number_file){
    std::ofstream out("../files/times"+number_file+".out");
    for(auto p: times){
        out << p.first << " "<< p.second<< "\n";
    }
    out << "\nTotal: "<< circuits_visited <<"\n";
    out.close();
    out.open("../files/gates"+number_file+".out");
    for(auto p: less_gates){
        out << p.first << "\n" << *p.second.get_logic_circuit() << "\n";
    }
    out.close();
    out.open("../files/size"+number_file+".out");
    for(auto p: less_size){
        out << p.first << "\n" << *p.second.get_logic_circuit() << "\n";
    }
    out.close();
    out.open("../files/depth"+number_file+".out");
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