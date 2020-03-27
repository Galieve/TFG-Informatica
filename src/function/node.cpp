#include "node.h"
#include <cassert>

bool node::evaluate(){
    std::vector<int> dp_table(id.toLong() + 1, -1);
    return evaluate(dp_table);
}

bool node::evaluate(std::vector<int> & dp_table){
    if(logic_gate == DATA){
        assert(right == nullptr && left == nullptr);
        return cast(*data);
    }

    bool l = false, r = false;
    l = left->evaluate(dp_table);
    r = right->evaluate(dp_table);
    // if(dp_table[left->id.toLong()] != -1) 
    //     l = dp_table[left->id.toLong()];
    // else{
    //     l = left->evaluate(dp_table);
    //     dp_table[left->id.toLong()] = l;
    // }

  
    // if(dp_table[right->id.toLong()] != -1) 
    //     r = dp_table[right->id.toLong()];
    // else{
    //     r = right->evaluate(dp_table);
    //     dp_table[right->id.toLong()] = r;
    // }

    switch (logic_gate){
    case OR:
        return l || r;
    
    default:
        //case AND
        return l && r;
    }
}

std::shared_ptr<node> node::build(std::vector<bool_enum> &v, const vvnode_info &build_v, 
    size_t depth, size_t size, size_t input_size){

    std::vector<std::vector<std::shared_ptr<node>>> 
        nodes(depth - 1, std::vector<std::shared_ptr<node>>(size, nullptr));
    nodes.emplace_back(std::vector<std::shared_ptr<node>>(input_size, nullptr));
    InfInt id = 0;
    return node::build(v, build_v, {0, 0}, nodes, id);
}

std::shared_ptr<node> node::build(std::vector<bool_enum> &v, const vvnode_info &ady_circuit,
    const std::pair<int,int> &p, std::vector<std::vector<std::shared_ptr<node>>> &nodes,
    InfInt &id
    ){
        
    auto i = p.first, j = p.second;
    if(nodes[i][j] != nullptr) return nodes[i][j];
    
    if(!ady_circuit[i][j].is_gate()){
        nodes[i][j] = std::make_shared<node>(&v[ady_circuit[i][j].get_cable_id()], id);
        ++id;
        return nodes[i][j];
    }

    auto left = build(v,ady_circuit,ady_circuit[i][j].get_cables()->left, nodes, id);
    auto right = build(v,ady_circuit,ady_circuit[i][j].get_cables()->right, nodes, id);
    std::shared_ptr<node> n;
    if(ady_circuit[i][j].is_and_gate()){
        n = std::make_shared<node>(left,right,node::logic_gate_t::AND, id);
    }else{
        n = std::make_shared<node>(left,right,node::logic_gate_t::OR, id);
    }
    ++id;
    nodes[i][j] = n;
    return n;
}

InfInt node::get_id(){
    return id;
}