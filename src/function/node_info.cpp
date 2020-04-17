#include "node_info.h"
#include <cassert>
#include <cmath>
#include <iomanip>
#include <sstream>

std::size_t node_info::get_logic_id() const{
    assert(logic_gate);
    std::size_t res = 0;
    auto l = data_type.info_gate.left.second;
    auto r = data_type.info_gate.right.second;
    res += bottom_size*bottom_size;
    res = res - (bottom_size - l)*(bottom_size - l);
    res += 2 * (r - l);
    if(is_or_gate())
        res -= 1;
    return res;
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
#ifdef PRETTY_PRINTING
        out<<"CABLE ("<<std::setfill(' ') << std::setw(2)<<data_type.pos
            <<std::setfill(' ') << std::setw(2)<<")";
#elif defined(USEFUL_PRINTING)
        out << "c "<<data_type.pos;
#endif
    else if(is_or_gate())
#ifdef PRETTY_PRINTING
        out<<"OR ("<<std::setfill(' ') << std::setw(2)
            <<data_type.info_gate.left.second<<", "
            <<std::setfill(' ') << std::setw(2)
            <<data_type.info_gate.right.second<<")";
#elif defined(USEFUL_PRINTING)
        out << "o " <<data_type.info_gate.left.second 
            << " "<<data_type.info_gate.right.second;
#endif
    else
#ifdef PRETTY_PRINTING
        out<<"AND("<<std::setfill(' ') << std::setw(2)
            <<data_type.info_gate.left.second<<", "
            <<std::setfill(' ') << std::setw(2)
            <<data_type.info_gate.right.second<<")";
#elif defined(USEFUL_PRINTING)
        if(is_bypass())
            out << "b " <<data_type.info_gate.left.second 
                << " "<<data_type.info_gate.right.second;
        else
            out << "a " <<data_type.info_gate.left.second 
                << " "<<data_type.info_gate.right.second;
#endif
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

std::ostream & operator<<(std::ostream &out, const vvnode_info &v){
    try{
        for(int i = 0; i < v.size(); ++i){
#ifdef PRETTY_PRINTING
            for(int j = 0; j < v[i].size() - 1; ++j){
                out << v[i][j] << " | ";
            }
            out << v[i][v[i].size() - 1] << "\n";
#elif defined(USEFUL_PRINTING)
            for(int j = 0; j < v[i].size() - 1; ++j){
                out << v[i][j] << " ";
            }
            out << v[i][v[i].size() - 1] << "\n";
#endif
        }
    }catch(std::exception & e){
        std::cerr << "La excepción de los cojones dice: " << e.what() << "\n";
        throw e;
    }
    return out;
}

std::shared_ptr<vvnode_info> node_info::generate_vvnode_info
    (const std::vector<std::string> &v){
    if(v.size() == 1 && v[0] == "nullptr") return nullptr;
    vvnode_info vi(v.size());
    
    for(int i = 0; i < v.size(); ++i){
        std::stringstream ss(v[i]);
        int size = 0; char x;
        while(ss >> x) ++size;
        if(i == v.size() - 1) vi[i].resize(size / 2);
        else vi[i].resize(size / 3);
    }
    int i = 0;
    for(auto s: v){
        std::stringstream ss(s);
        char c;
        int a, b;
        int j = 0;
        while(ss >> c){
            if(c == 'c'){
                ss >> a;
                vi[i][j] = node_info::get_default(i,v.size(), -1, v[i].size());
                vi[i][j].set_cable_id(a);
            }
            else{
                ss >> a >> b;
                vi[i][j] = node_info::get_default(i,v.size(), v[i + 1].size(), v[i].size());
                vi[i][j].set_left_cable_gate(a);
                vi[i][j].set_right_cable_gate(b);
                if (c == 'o'){
                    vi[i][j].set_as_or_gate();

                }
                else{
                    vi[i][j].set_as_and_gate();

                }
            }
            ++j;
        }
        ++i;
    }
    for(int i = 0; i < vi.size() - 1; ++i){
        for(int j = 0; j < vi[i].size(); ++j){
            vi[i][j].set_top_size(vi[i].size());
            vi[i][j].set_bottom_size(vi[i + 1].size());
        }
    }
    i = vi.size() - 1;
    for(int j = 0; j < vi[i].size(); ++j){
        vi[i][j].set_top_size(vi[i].size());
    }
    return std::make_shared<vvnode_info>(vi);
}