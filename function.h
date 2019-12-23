#pragma once
#ifndef FUNCTION_H
#define FUNCTION_H
#ifndef NODE_H
    #include "node.h"
#endif
#ifndef BOOL_ENUM_H
    #include "bool_enum.h"
#endif
#ifndef NODE_INFO_H
    #include "node_info.h"
#endif
#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <memory>


typedef unsigned long long ullong;

#define INPUT_SIZE 5
#define FULL_INPUT_SIZE 2*INPUT_SIZE

class function{

protected:

    std::size_t input_size;

    std::size_t depth;

    std::size_t size;

    std::bitset<(1<<INPUT_SIZE)> bits;

    std::shared_ptr<node> output_node;

    std::shared_ptr<std::vector<bool_enum>> input;

public:

    ~function();

    static void create(const ullong &n, std::vector<bool_enum> &v);

    static function build(const std::vector<std::vector<node_info>> & parse_info,
        std::size_t input_size);

    void evaluate_all();

    bool evaluate(ullong n) const;

    std::size_t get_depth() const;

    std::size_t get_size() const;

    std::size_t get_input_size() const;

    std::string get_id() const;

protected:

    function(std::shared_ptr<std::vector<bool_enum>>v, std::shared_ptr<node> n, size_t input_size,
        std::size_t size = SIZE, std::size_t dep = DEPTH) : 
        input(v), output_node(n), input_size(input_size), size(size), depth(dep) {};
    
};
#endif