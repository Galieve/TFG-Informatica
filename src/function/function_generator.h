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
    #include "define.h"
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

public:

    function_generator(std::size_t depth,std::size_t size, std::size_t input_size);

    function_generator(std::size_t depth, std::size_t input_size);
    
    std::unique_ptr<vvnode_info> generate_next();

    std::unique_ptr<vvnode_info> get_current();

    //friend std::ostream & operator<< (std::ostream &out, const function_generator &fg);

    friend std::ostream & operator<< (std::ostream &out, 
        const vvnode_info &v);

    std::size_t get_depth() const;

    std::size_t get_size() const;

    std::size_t get_input_size() const;

protected:

    function_generator() : depth(0) {};

    void increase_depth();

    void set_init(std::size_t level); //

    void restart_level(std::size_t level); //

    void restart_cable_level(std::size_t level); //

    void restart_gate_value_level(std::size_t level);

    bool and_gate_value_full(std::size_t level); //
    
    bool increase_gate_link_value(std::size_t level); //

    bool cable_level_full_linked(std::size_t level); //

    void increase_and_gate_value(std::size_t level); //

    void increase_cabled_value(std::size_t level); //

    void reset_gate_value(std::size_t level); //

    bool max_nodes_achieved(std::size_t level); //

    void add_node(std::size_t level); //

    void resize_node_level(std::size_t level); //

    void complete_last_floors(std::size_t level);

    void resize_floor(std::size_t level, std::size_t elems);

};

#endif