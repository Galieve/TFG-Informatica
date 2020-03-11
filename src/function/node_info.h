#pragma once
#ifndef NODE_INFO_H
#define NODE_INFO_H

class node_info;
#include <utility>
#include <iostream>
#include <string>
#include <vector>

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

    node_info() : data_type() {};

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

    friend std::ostream & operator<< (std::ostream &out, const std::vector<std::vector<node_info>> &v){
    try{
        for(int i = 0; i < v.size(); ++i){
            for(int j = 0; j < v[i].size() - 1; ++j){
                out << v[i][j] << " | ";
            }
            out << v[i][v[i].size() - 1] << "\n";
        }
    }catch(std::exception & e){
        std::cerr << "La excepción de los cojones dice: " << e.what() << "\n";
        throw e;
    }
    return out;
}


protected:

    std::string to_string() const;
};

typedef node_info::gate_info gate_info;

#endif