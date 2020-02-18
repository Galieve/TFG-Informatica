#pragma once
#include "node.h"
#include <cassert>

bool node::evaluate(std::size_t depth, std::size_t size, std::size_t input_size){
    std::unordered_map<std::shared_ptr<node> *,bool> dp_table;
    //vamos por aqui :)
    //return true;
    bool l = evaluate(dp_table);
    return l;
}

bool node::evaluate(std::unordered_map<std::shared_ptr<node> *,bool> dp_table){
    if(logic_gate == DATA){
        assert(right == nullptr && left == nullptr);
        return cast(*data);
    }
    bool l = false, r = false;
    l = left->evaluate(dp_table);
    r = right->evaluate(dp_table);
    // if(dp_table.find(&left)!=dp_table.end()) l = dp_table.find(&left)->second;
    // else{
    //     l = left->evaluate(dp_table);
    //     dp_table[&left] = l;
    // }
    // if(dp_table.find(&right)!=dp_table.end()) r = dp_table.find(&right)->second;
    // else{
    //     r = right->evaluate(dp_table);
    //     dp_table[&right] = r;
    // }
    switch (logic_gate){
    case OR:
        return l || r;
    
    default:
        //case AND
        return l && r;
    }
}

std::shared_ptr<node> node::build(std::vector<bool_enum> &v, 
    const std::vector<std::vector<node_info>> &build_v, 
    size_t depth, size_t size, size_t input_size){
    std::vector<std::vector<std::shared_ptr<node>>> 
        nodes(depth - 1, std::vector<std::shared_ptr<node>>(size, nullptr));
    nodes.emplace_back(std::vector<std::shared_ptr<node>>(input_size, nullptr));
    return node::build(v, build_v, {0, 0}, nodes);
}

std::shared_ptr<node> node::build(std::vector<bool_enum> &v, 
    const std::vector<std::vector<node_info>> &ady_circuit,
    const std::pair<int,int> &p, 
    std::vector<std::vector<std::shared_ptr<node>>> &nodes){
        
    auto i = p.first, j = p.second;
    if(nodes[i][j] != nullptr) return nodes[i][j];
    
    if(!ady_circuit[i][j].is_gate()){
        nodes[i][j] = std::make_shared<node>(&v[ady_circuit[i][j].get_cable_id()]);
        return nodes[i][j];
    }

    auto left = build(v,ady_circuit,ady_circuit[i][j].get_cables()->left, nodes);
    auto right = build(v,ady_circuit,ady_circuit[i][j].get_cables()->right, nodes);
    std::shared_ptr<node> n;
    if(ady_circuit[i][j].is_and_gate()){
        n = std::make_shared<node>(left,right,node::logic_gate_t::AND);
    }else{
        n = std::make_shared<node>(left,right,node::logic_gate_t::OR);
    }
    nodes[i][j] = n;
    return n;
}