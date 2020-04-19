#pragma once
#ifndef CIRCUIT_H
#define CIRCUIT_H
#ifndef DEFINE_H
    #include "../define.h"
#endif

class circuit{

public:

    std::shared_ptr<vvnode_info> logic_circuit_ptr;

    std::size_t size;

    std::size_t depth;

    std::size_t logic_gates;

    std::string id;

public:

    inline circuit(const std::shared_ptr<vvnode_info> &l_circ, const std::string &id) : 
        logic_circuit_ptr(l_circ), id(id), size(l_circ->size()),
            depth((*l_circ)[0].size()){
                calculate_logic_gates();
            };

    inline circuit(const circuit &c) : logic_circuit_ptr(c.logic_circuit_ptr), id(c.id),
        size(c.size), depth(c.depth), logic_gates(c.logic_gates) {};

    inline circuit() = default;

    inline std::size_t get_size() const { return size; }

    inline std::size_t get_depth() const { return depth; }

    inline std::size_t get_logic_gates() const { return logic_gates; }

    inline std::string get_id() const { return id; }

    inline std::shared_ptr<vvnode_info> get_logic_circuit() const { return logic_circuit_ptr; }

protected:

    void calculate_logic_gates();
};

#endif