#pragma once
#ifndef FUNCTION_GENERATOR_H
#define FUNCTION_GENERATOR_H
#include <utility>
#include <vector> 
#ifndef NODE_INFO_H
    #include "node_info.h"
#endif

class function_generator{

protected:

    std::size_t depth;

    std::size_t max_depth;

    std::size_t input_size; //2*SIZE

    std::size_t size;

    std::vector<std::vector<node_info>> current;

    std::vector<std::vector<std::size_t>> occurrences;

public:

    function_generator(std::size_t depth,std::size_t size, std::size_t input_size);

    function_generator(std::size_t depth, std::size_t input_size);
    
    std::vector<std::vector<node_info>> * generate_next();

    std::vector<std::vector<node_info>> * get_current();

    //friend std::ostream & operator<< (std::ostream &out, const function_generator &fg);

    friend std::ostream & operator<< (std::ostream &out, 
        const std::vector<std::vector<node_info>> &v);

    size_t get_depth() const;

    size_t get_size() const;

    size_t get_input_size() const;

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

};

#endif