#include "statistics.h"
#include <fstream>

void statistics::update_statistics(const circuit &circ){
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
        times[circ.get_id()] += 1;
    }
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

void statistics::output_results(){
    std::ofstream out("../files/times.out");
    for(auto p: times){
        out << p.first << " "<< p.second<< "\n";
    }
    out.close();
    out.open("../files/gates.out");
    for(auto p: less_gates){
        out << p.first << "\n" << *p.second.get_logic_circuit() << "\n";
    }
    out.close();
    out.open("../files/size.out");
    for(auto p: less_size){
        out << p.first << "\n" << *p.second.get_logic_circuit() << "\n";
    }
    out.close();
    out.open("../files/depth.out");
    for(auto p: less_depth){
        out << p.first << "\n" << *p.second.get_logic_circuit() << "\n";
    }
    out.close();
}