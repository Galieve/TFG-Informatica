#pragma once
#ifndef FUNCTION_GENERATOR_H
#define FUNCTION_GENERATOR_H
#include <utility>
#include <vector>
#include <unordered_set> 
#include <memory>
#ifndef NODE_INFO_H
    #include "node_info.h"
#endif
#ifndef DEFINE_H
    #include "../define.h"
#endif

class function_generator{

protected:

    std::size_t depth;

    std::size_t max_depth;

    std::size_t input_size; //2*SIZE

    std::size_t size;

    vvnode_info current;

    std::vector<std::vector<std::size_t>> occurrences;

    std::unordered_set<int> completed_levels;

    //if you want to set up a nullptr vector (i.e. the end), this flag makes
    //computations easier
    bool nullptr_flag = false;

public:

    function_generator(std::size_t depth,std::size_t size, std::size_t input_size);

    function_generator(std::size_t depth, std::size_t input_size);
    
    std::shared_ptr<vvnode_info> generate_next();

    std::shared_ptr<vvnode_info> get_current() const;

    void set_current(const std::shared_ptr<vvnode_info> curr);

    std::size_t get_depth() const;

    std::size_t get_size() const;

    std::size_t get_input_size() const;

protected:

    inline function_generator() : depth(0) {};

    void increase_depth();

    void set_init(std::size_t level); 

    void restart_level(std::size_t level); 

    void restart_cable_level(std::size_t level); 

    void restart_gate_value_level(std::size_t level);

    bool and_gate_value_full(std::size_t level); //
    
    bool increase_gate_link_value(std::size_t level); 

    bool cable_level_full_linked(std::size_t level); 

    void increase_and_gate_value(std::size_t level); 

    void increase_cabled_value(std::size_t level); 

    void reset_gate_value(std::size_t level); 

    bool max_nodes_achieved(std::size_t level); 

    void add_node(std::size_t level); 

    void resize_node_level(std::size_t level); 

    void complete_last_floors(std::size_t level);

    void resize_floor(std::size_t level, std::size_t elems);

    // return v = [k,...,m], |v| = v_size.
    std::vector<std::size_t> get_minimal_sizes(int k, int m, int v_size);

};

#endif