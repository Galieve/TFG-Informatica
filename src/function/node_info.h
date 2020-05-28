#pragma once
#ifndef NODE_INFO_H
#define NODE_INFO_H

#include <utility>
#include <iostream>
#include <string>
#include <vector>
class node_info;
typedef std::vector<std::vector<node_info>> vvnode_info;

#ifndef DEFINE_H
    #include "../define.h"
#endif

class node_info{

public:

    struct gate_info{
        std::pair<std::size_t,std::size_t> left;

        std::pair<std::size_t,std::size_t> right;
        char c;
    };

protected:

    struct data_t{
        int pos;
        gate_info info_gate;
        data_t() : pos(0) {};
    };

    data_t data_type;

    bool logic_gate = true;

    std::size_t top_size;

    std::size_t bottom_size;

public:

    inline node_info() : data_type() {};

    static node_info get_default(int level, int depth, int top_size, int bottom_size);

    std::size_t get_logic_id() const;

    std::size_t get_cable_id() const;

    std::size_t get_bottom_size() const;

    std::size_t get_top_size() const;

    void set_logic_id(std::size_t n);

    void set_cable_id(std::size_t n);

    void set_bottom_size(std::size_t bottom_size);

    void set_top_size(std::size_t top_size);

    void set_logic_level(std::size_t level);

    void set_left_cable_gate(std::size_t n);

    void set_right_cable_gate(std::size_t n);

    //doesn't guarantee existent cable positions.
    void increase_cable_id();

    void increase_gate_id();

    //only works if the node it's already a gate
    void set_as_and_gate();

    //only works if the node it's already a gate
    void set_as_or_gate();

    bool is_gate() const;

    bool is_and_gate() const;

    bool is_or_gate() const;

    gate_info * get_cables();

    const gate_info * get_cables() const;
    
    size_t get_level() const;

    bool operator== (const node_info &n) const;

    bool operator!= (const node_info &n) const;

    friend std::ostream & operator<< (std::ostream &out, const node_info &n);

    bool is_bypass() const;

#ifdef PRODUCTION_MODE
    static std::shared_ptr<vvnode_info> generate_reversed
        (const std::shared_ptr<vvnode_info> &v);
#endif

    friend std::ostream & operator<< (std::ostream &out, const vvnode_info &v);

    static std::shared_ptr<vvnode_info> generate_vvnode_info(const std::vector<std::string> &v);

protected:

    std::string to_string() const;
};

typedef node_info::gate_info gate_info;


#endif