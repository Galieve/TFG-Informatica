#pragma once
#include "function.h"
#include <cassert>

void function::evaluate_all(){
    for(ullong i=0; i < (1 << INPUT_SIZE); ++i){
        bits[i] = evaluate(i);
    }
}
std::string function::get_id() const{
    return bits.to_string();
}

bool function::evaluate(ullong n) const{
    assert(output_node != nullptr);
    create(n, *input);
    return output_node->evaluate(depth, size, input_size);
}

function function::build(const std::vector<std::vector<node_info>> & parse_info, 
    std::size_t input_size){

    assert(parse_info.size() > 0);
    std::shared_ptr<std::vector<bool_enum>> input 
        = std::make_shared<std::vector<bool_enum>>(std::vector<bool_enum>(input_size));
    auto output_node = node::build(*input, parse_info,parse_info.size(), input_size, input_size);
    return function(input, output_node, input_size, parse_info[0].size(), parse_info.size());
}

void function::create(const ullong &n, std::vector<bool_enum> &v){
    for(int i = 0; i < v.size() / 2; ++i){
        v[i] = false_;
    }
    for(int i = v.size() / 2; i < v.size(); ++i){
        v[i] = true_;
    }
    int m = n;
    int k = 0;
    while(m>0){
        int op = m%2;
        m = m / 2;
        if(op == 1){
            v[k] = true_;
            v[k+v.size() / 2] = false_;
        }
        ++k;
    }
}

size_t function::get_size() const{
    return size;
}

size_t function::get_depth() const{
    return depth;
}

size_t function::get_input_size() const{
    return input_size;
}

function::~function(){
}