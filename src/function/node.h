#pragma once
#ifndef NODE_H
#define NODE_H
#ifndef FUNCTION_H
    #include "function.h"
#endif
#ifndef NODE_INFO_H
    #include "node_info.h"
#endif
#ifndef BOOL_ENUM_H
    #include "bool_enum.h"
#endif
#include <vector>
#include <memory>
#include <unordered_map>


const uint SIZE = 4;

const uint DEPTH = 1;

class node{

public:

    enum logic_gate_t{AND, OR, DATA};

protected:

    std::shared_ptr<node> left;

    std::shared_ptr<node> right;

    bool_enum * data;

    logic_gate_t logic_gate;

public:

    node(bool_enum * d) : logic_gate(DATA), data(d), left(nullptr), right(nullptr) {};

    node(std::shared_ptr<node> &l, std::shared_ptr<node> &r, logic_gate_t type) 
        : logic_gate(type), left(l), right(r) {};

    bool evaluate(std::size_t depth, std::size_t size, std::size_t input_size);

    static std::shared_ptr<node> build(std::vector<bool_enum> &v, 
        const std::vector<std::vector<node_info>> &build_v, 
        size_t depth, size_t size, size_t input_size);

protected:

    bool evaluate(std::unordered_map<std::shared_ptr<node> *,bool> dp_table);

    static std::shared_ptr<node> build(std::vector<bool_enum> &v, const std::vector<std::vector<node_info>> &ady_circuit,
        const std::pair<int,int> &p, std::vector<std::vector<std::shared_ptr<node>>> &nodes);

    static void delete_node(node * n);

};

#endif