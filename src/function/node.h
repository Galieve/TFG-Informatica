#pragma once
#ifndef NODE_H
#define NODE

class node;

#ifndef FUNCTION_H
    #include "function.h"
#endif
#ifndef NODE_INFO_H
    #include "node_info.h"
#endif
#ifndef BOOL_ENUM_H
    #include "bool_enum.h"
#endif
#include "../infint/InfInt.h"
#include <vector>
#include <memory>
#include <unordered_map>


const unsigned int SIZE = 4;

const unsigned int DEPTH = 1;

class node{

public:

    enum logic_gate_t{AND, OR, DATA};

    struct InfIntHasher{
    std::size_t operator()(const InfInt& k) const{
        return std::hash<std::string>()(k.toString());
        }
    };

protected:

    std::shared_ptr<node> left;

    std::shared_ptr<node> right;

    bool_enum * data;

    logic_gate_t logic_gate;

    InfInt id;
    
public:

    node(bool_enum * d, InfInt id) : logic_gate(DATA), data(d), 
        left(nullptr), right(nullptr), id(id) {};

    node(std::shared_ptr<node> &l, std::shared_ptr<node> &r, logic_gate_t type, 
        InfInt id) : logic_gate(type), left(l), right(r), id(id) {};

    bool evaluate();

    static std::shared_ptr<node> build(std::vector<bool_enum> &v, 
        const std::vector<std::vector<node_info>> &build_v, 
        size_t depth, size_t size, size_t input_size);

    bool operator== (const node &n);

protected:

    bool evaluate(std::vector<int> &dp_table);

    static std::shared_ptr<node> build(std::vector<bool_enum> &v, const vvnode_info &ady_circuit,
        const std::pair<int,int> &p, std::vector<std::vector<std::shared_ptr<node>>> &nodes,
        InfInt &id);

    static void delete_node(node * n);

    InfInt get_id();

};

#endif