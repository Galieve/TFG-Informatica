#pragma once
#ifndef FUNCTION_H
#define FUNCTION_H

class function;

#ifndef NODE_H
    #include "node.h"
#endif
#ifndef BOOL_ENUM_H
    #include "bool_enum.h"
#endif
#ifndef NODE_INFO_H
    #include "node_info.h"
#endif
#ifndef DEFINE_H
    #include "../define.h"
#endif
#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <memory>
#include <mutex>
#include <memory>
#include <atomic>

class function{

protected:

    std::size_t input_size;

    std::size_t depth;

    std::size_t size;

    std::bitset<(1<<INPUT_SIZE)> bits;

    std::shared_ptr<node> output_node;

    std::vector<std::shared_ptr<bool_enum>> input;

public:

    inline ~function() = default;

    static void create(const ullong &n, const std::vector<std::shared_ptr<bool_enum>> &v);

    static function build(const vvnode_info & parse_info,
        std::size_t size,
        std::size_t input_size);

#ifdef PRODUCTION_MODE

    std::string get_function_equivalent();

#endif

    void evaluate_all();

    bool evaluate(ullong n) const;

    std::size_t get_depth() const;

    std::size_t get_size() const;

    std::size_t get_input_size() const;

    std::string get_id() const;

    inline function(const std::vector<std::shared_ptr<bool_enum>> &v, 
        std::shared_ptr<node> n, size_t input_size,
        std::size_t size, std::size_t dep) : 
        input(std::move(v)), output_node(n), 
        input_size(input_size), size(size), depth(dep) {};

    inline function(const function &f) :
        input(f.input), 
        output_node(std::shared_ptr<node>(f.output_node)),
        input_size(f.input_size),
        size(f.size), depth(f.depth) ,bits(std::move(f.bits)) {};        
    
};
#endif