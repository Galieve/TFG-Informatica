#include "node_info.h"
#include <cassert>
#include <cmath>
#include <iomanip>
#include <sstream>

std::size_t node_info::get_logic_id() const{
    assert(logic_gate);
    return data_type.info_gate.right.second+data_type.info_gate.left.second
        * bottom_size-data_type.info_gate.left.second*(data_type.info_gate.left.second+1)/2 + 1;
}

std::size_t node_info::get_cable_id() const{
    assert(!logic_gate);
    return data_type.pos;
}
std::size_t node_info::get_bottom_size() const{
    return this->bottom_size;
}
std::size_t node_info::get_top_size() const{
   return this->top_size;
}

void node_info::increase_cable_id(){
    assert(!logic_gate);
    ++data_type.pos;
}

void node_info::increase_gate_id(){
    assert(logic_gate);
    if(data_type.info_gate.right.second == bottom_size){
        ++data_type.info_gate.left.second;
        data_type.info_gate.right.second = data_type.info_gate.left.second;
    }
    else{
        ++data_type.info_gate.right.second;
    }
}

void node_info::set_cable_id(std::size_t n){
    data_type.pos = n;
}

void node_info::set_logic_id(std::size_t n){
    int coef = 2*n-bottom_size*(bottom_size+1);
    int ans = ceil(0.5*(-1+sqrt(1-4*coef)));
    data_type.info_gate.left.second = bottom_size - ans;
    data_type.info_gate.right.second =
        n - bottom_size * data_type.info_gate.left.second + 
        data_type.info_gate.left.second*(data_type.info_gate.left.second+1)/2;
}

void node_info::set_bottom_size(std::size_t bottom_size){
    this->bottom_size = bottom_size;
}

void node_info::set_top_size(std::size_t top_size){
    this->top_size = top_size;
}


void node_info::set_logic_level(std::size_t level){
    data_type.info_gate.left.first = level;
    data_type.info_gate.right.first = level;
}

node_info node_info::get_default(int level, int depth, int bottom_size, int top_size){
    node_info n;
    n.bottom_size = bottom_size;
    n.top_size = top_size;
    if(level < depth - 1){
        n.data_type.info_gate.left = {level + 1, 0};
        n.data_type.info_gate.right = {level + 1, 0};
        n.data_type.info_gate.c = '&';
        n.logic_gate = true;
    }
    else{
        n.data_type.pos = 0;
        n.logic_gate = false;
    }
    return n;
}

bool node_info::is_gate() const{
    return this->logic_gate;
}

bool node_info::is_and_gate() const{
    return is_gate() && this->data_type.info_gate.c == '&';
}

bool node_info::is_or_gate() const{
    return is_gate() && this->data_type.info_gate.c == '|';
}

void node_info::set_as_and_gate(){
    if(!is_gate()) assert(false);
    this->data_type.info_gate.c = '&';
}

void node_info::set_as_or_gate(){
    if(!is_gate()) assert(false);
    this->data_type.info_gate.c = '|';
}

gate_info * node_info::get_cables(){
    assert(this->is_gate());
    return &(this->data_type.info_gate);
}

const gate_info *node_info::get_cables() const{
    assert(this->is_gate());
    return &(data_type.info_gate);
}

void node_info::set_left_cable_gate(std::size_t n){
    assert(this->is_gate());
    assert(n < this->bottom_size);
    this->data_type.info_gate.left.second = n;
}

void node_info::set_right_cable_gate(std::size_t n){
    assert(this->is_gate());
    assert(n < this->bottom_size);
    this->data_type.info_gate.right.second = n;
}

std::string node_info::to_string() const{
    std::stringstream out;
    if(!is_gate()) 
        out<<"CABLE ("<<std::setfill(' ') << std::setw(2)<<data_type.pos
            <<std::setfill(' ') << std::setw(2)<<")";
    else if(is_or_gate()) 
        out<<"OR ("<<std::setfill(' ') << std::setw(2)
            <<data_type.info_gate.left.second<<", "
            <<std::setfill(' ') << std::setw(2)
            <<data_type.info_gate.right.second<<")";
    else
        out<<"AND("<<std::setfill(' ') << std::setw(2)
            <<data_type.info_gate.left.second<<", "
            <<std::setfill(' ') << std::setw(2)
            <<data_type.info_gate.right.second<<")";
    return out.str();
}

std::ostream & operator<< (std::ostream &out, const node_info &n){
    out << n.to_string();
    return out;
}

bool node_info::operator==(const node_info &n) const{
    if(n.is_gate() != is_gate()) return false;
    if(!n.is_gate()) return n.get_cable_id() == get_cable_id();
    if(n.get_level() != get_level()) return false;
    if(n.is_and_gate() && is_or_gate()) return false;
    if(is_and_gate() && n.is_or_gate()) return false;
    return n.get_cables()->left.second == get_cables()->left.second &&
        n.get_cables()->right.second == get_cables()->right.second &&
        n.get_cables()->c == get_cables()->c;
}

bool node_info::operator!=(const node_info &n) const{
    return !(n == *this);
}

size_t node_info::get_level() const{
    assert(is_gate());
    assert(data_type.info_gate.left.first == data_type.info_gate.right.first);
    return data_type.info_gate.left.first;
}

bool node_info::is_bypass() const{
    if(!is_gate()) return false;
    return this->get_cables()->left == this->get_cables()->right;
}