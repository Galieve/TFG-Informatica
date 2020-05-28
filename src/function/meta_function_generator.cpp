#include "meta_function_generator.h"
#include <cmath>

meta_function_generator::meta_function_generator
    (std::size_t max_depth, std::size_t max_size, std::size_t input_size) :
    max_depth(max_depth), max_size(max_size), input_size(input_size){
    
#ifndef PRODUCTION_MODE
    depth = 1;
    size = 1;
#else
    depth = PROD_BITS + 1;
    size = 1;
#endif
}

meta_function_generator::meta_function_generator
    (std::size_t max_values, std::size_t input_size) :
    max_depth(max_values), max_size(max_values), input_size(input_size){

#ifndef PRODUCTION_MODE
    depth = 1;
    size = 1;
#else
    depth = PROD_BITS + 1;
    size = 1;
#endif
}

void meta_function_generator::advance(){

#ifndef PRODUCTION_MODE
    //que hace lo d la dcha?
    if(depth == 1 || depth - 2 <= ceil(log2(size + 1))){
        depth = size + depth;
        size = 1;
    }
    else{
        --depth;
        ++size;
    }
#else
    if(size <= 8){
        ++size;
    }
#endif
}

std::unique_ptr<function_generator> meta_function_generator::get(){
    return std::unique_ptr<function_generator>
        (new function_generator(depth,size,input_size));
}

//Por el momento, se recorre en diagonal y se avanza hasta que alguna 
//de las dos dimensiones se supere, es decir, vemos el triángulo inferior izquierdo
//del mayor cuadrado inscrito en el rectángulo que pasa por (1,1).
bool meta_function_generator::can_get_next(){
    return size <= max_size && depth <= max_depth;
}