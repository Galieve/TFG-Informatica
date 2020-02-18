#pragma once
#ifndef META_FUNCTION_GENERATOR_H
#define META_FUNCTION_GENERATOR_H
#ifndef FUNCTION_GENERATOR_H
    #include "function_generator.h"
#endif
#include <memory>

class meta_function_generator{

protected:
    std::size_t depth = 1;

    std::size_t size = 1;

    std::size_t input_size;

    std::size_t max_size;

    std::size_t max_depth;

public:

    meta_function_generator(std::size_t max_values, std::size_t input_size);

    meta_function_generator(std::size_t max_depth, std::size_t max_size,
        std::size_t input_size);

    bool can_get_next();

    void advance();

    std::unique_ptr<function_generator> get();
};


#endif
