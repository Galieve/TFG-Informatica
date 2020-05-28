#include "function.h"
#include <cassert>

void function::evaluate_all(){

    for(ullong i=0; i < (1 << INPUT_SIZE); ++i){
        bits[i] = evaluate(i);
    }

}
std::string function::get_id() const{

    return bits.to_string();
}

bool function::evaluate(ullong n) const{
    assert(output_node != nullptr);
    create(n, input);
    return output_node->evaluate();
}

function function::build(const vvnode_info & parse_info, 
    std::size_t size,
    std::size_t input_size){
    assert(parse_info.size() > 0);
    std::vector<std::shared_ptr<bool_enum>> input
        (input_size, nullptr);
    for(int i = 0; i < input.size(); ++i){
        input[i] = std::make_shared<bool_enum>(undef_);
    }
    auto output_node = 
        node::build(std::move(input), parse_info,parse_info.size(), size, input_size);
    auto depth = parse_info.size();
    return 
        function(std::move(input), output_node, size, input_size, depth);
}

void function::create(const ullong &n, const std::vector<std::shared_ptr<bool_enum>> &v){
    for(int i = 0; i < v.size() / 2; ++i){
        *v[i] = false_;
    }
    for(int i = v.size() / 2; i < v.size(); ++i){
        *v[i] = true_;
    }
    int m = n;
    int k = 0;
    while(m>0){
        int op = m%2;
        m = m / 2;
        if(op == 1){
            *v[k] = true_;
            *v[k+v.size() / 2] = false_;
        }
        ++k;
    }
}

size_t function::get_size() const{
    return size;
}

size_t function::get_depth() const{
    return depth;
}

size_t function::get_input_size() const{
    return input_size;
}

#ifdef PRODUCTION_MODE

std::string function::get_function_equivalent(){
    create(0, input);
    return output_node->get_function_equivalent();
}

#endif