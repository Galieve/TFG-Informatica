#include "function_generator.h"
#include <cassert>
#include <set>
#include <cmath>

function_generator::function_generator(std::size_t depth, std::size_t size, std::size_t input_size)
    : depth(depth), max_depth(depth), input_size(input_size), size(size){
    occurrences.resize(depth - 1,std::vector<std::size_t>(1,0));
    current.resize(depth);
    for(int i = 0; i < depth - 1; ++i){
        current[i].push_back(node_info::get_default(i,depth,1,1));
    }
    current[depth - 1].push_back(node_info::get_default(depth - 1, depth, 1,input_size));
}

function_generator::function_generator(std::size_t depth, std::size_t input_size) : 
    function_generator(depth, input_size,input_size){
}

inline bool increase_right(int &left, int &right, int bottom_size){
    bool delay = false;
    ++right;
    if(right == bottom_size) {
        ++left, right = left; 
        delay = true;
    }
    return delay;
}

void function_generator::set_init(std::size_t level){
    assert(level < current.size() - 1);
    std::size_t top_size = current[level].size(), bottom_size = current[level+1].size();
    int f = 0, s = 0, max_v = -1, pos = 0;
    bool delay_increase = true;
    for(int i = 0; i < 2*top_size; ++i){
        current[level][pos/2].set_left_cable_gate(f);
        occurrences[level][f]++;
        max_v = std::max(f,max_v);
        ++pos;
        if(bottom_size - max_v - 1 == 2*top_size - pos) break;
        current[level][pos/2].set_right_cable_gate(s);
        occurrences[level][s]++;
        max_v = std::max(s,max_v);
        ++pos;
        if(bottom_size - max_v - 1 == 2*top_size - pos) break;
        
        if(delay_increase){
            delay_increase = increase_right(f,s,bottom_size);
        }
        else{
            delay_increase = true;
        }
    }
    while(max_v + 1 < bottom_size){
        ++max_v;
        if(pos %2 == 0) current[level][pos/2].set_left_cable_gate(max_v);
        else current[level][pos/2].set_right_cable_gate(max_v);
        occurrences[level][max_v]++;
        ++pos;
    }
}

void function_generator::resize_node_level(std::size_t level){
    assert(level < current.size() - 1);
    /*
    Si llamamos l = current[level+1].size() y k = current[level].size(),
    con l puertas se pueden hacer l*l parejas lexicográficas distinguiendo
    orden + bypass.
    así, dado k, el menor l que cumple que hay suficientes es ceil(sqrt(k)).
    */
    int init = ceil(sqrt(current[level].size()));
    
    current[level + 1].resize(init);
    occurrences[level].resize(init);
    for(auto &i : occurrences[level]) i = 0;
}

void function_generator::restart_level(std::size_t level){
    if(level < current.size() - 1) restart_gate_value_level(level);
    else restart_cable_level(level);
}

void function_generator::restart_gate_value_level(std::size_t level){
    std::size_t i = level;
    assert(level < current.size() - 1);
    for(std::size_t j = 0; j < current[i].size(); ++j){
        assert(current[i][j].is_gate());
        current[i][j].set_bottom_size(current[i+1].size());
        current[i][j].set_top_size(current[i].size());
        current[i][j].set_logic_level(i+1);
    }
    set_init(level);
    if(current[i][0].is_bypass())
        current[i][0].set_as_and_gate();
    else current[i][0].set_as_or_gate();
    for(std::size_t j = 1; j < current[i].size(); ++j){
        current[i][j].set_as_or_gate();
        //se pone a or para que el == funcione.
        if(current[i][j] == current[i][j-1])
            current[i][j].set_as_and_gate();
        else if(current[i][j].is_bypass())
            current[i][j].set_as_and_gate();
    }
}

void function_generator::restart_cable_level(std::size_t level){
    for(std::size_t j = 0; j < current[level].size(); ++j){
        current[level][j].set_cable_id(j);
    }
}

bool function_generator::cable_level_full_linked(std::size_t top_size){
    auto m = current[current.size() - 1][0];
    return m.get_cable_id() == input_size-top_size;
}

bool function_generator::and_gate_value_full(std::size_t level){
    int i = current[level].size() - 1;
    auto n = current[level][i];
    if(!n.is_gate()) assert(false);
    else if(n.is_or_gate()) return false;
    --i;
    for(; i >= 0; --i){
        auto m = current[level][i];
        if(!m.is_gate()) assert(false);
        //m es una copia, no es el mismo objeto. current[i][j] queda igual.
        m.set_as_and_gate();
        if(m == n) ;
        else if(current[level][i].is_or_gate()) return false;
        n = current[level][i];
    }
    return true;
}

//if it's full, do nothing.
void function_generator::increase_and_gate_value(std::size_t level){
    
    int i = level;
    int j = current[i].size() - 1;
    if(current[i][j].is_or_gate()){
        current[i][j].set_as_and_gate();
        return;
    }
    --j;
    for(; j >= 0; --j){
        assert(current[i][j].is_gate());
        auto t = current[i][j];
        t.set_as_and_gate();
        //t es una copia. Si to_and(t) == current[i][j+1] => no debemos hacer and
        if(t == current[i][j+1]) continue;
        current[i][j].set_as_and_gate();
        for(int k = j+1; k < current[i].size(); ++k){
            auto t = current[i][k];
            //t es una copia.
            t.set_as_or_gate();
            if(t == current[i][k - 1]) continue;
            else if(t.is_bypass()) continue;
            current[i][k].set_as_or_gate();
        }
        break;
    }
}

void function_generator::increase_cabled_value(std::size_t top_size){
    int i = current.size() - 1;
    auto n = current[i][0];
    for(int j = current[i].size() - 1; j >= 0; --j){
        assert(!current[i][j].is_gate());
        auto m = &current[i][j];
        if(m->get_cable_id() - 
            n.get_cable_id() == j){
            //it is not used with care, it can produce unexpected results.
            m->increase_cable_id();
            for(int k = 0; k < j; ++ k){
                current[i][k].set_cable_id(k);
            }
            break;
        }
    }
    //it wasn't possible;

}
#define REDUCE_OCCURRENCE(LVL, VAL) \
    occurrences[LVL][VAL]--; \
    if(occurrences[LVL][VAL] == 0){ \
        s.insert(VAL); \
    }

inline bool can_increase(std::size_t value, std::size_t bottom_size) {
    return value + 1 < bottom_size;
}

inline bool enough_holes(std::size_t value, const std::set<int> &set_, std::size_t holes){
    auto s = set_;
    s.erase(value);
    return s.size() <= holes;
}

inline bool too_many_holes(std::size_t left, std::size_t right, std::size_t bottom_size, 
    std::size_t holes, bool rep = false){
    node_info n;
    n.set_left_cable_gate(left),n.set_right_cable_gate(right);
    n.set_bottom_size(bottom_size);
    if(rep)
        return bottom_size*(bottom_size+1) - (2*n.get_logic_id()) < holes;
    else return bottom_size*(bottom_size+1) - (2*n.get_logic_id()-1) < holes;
}

inline bool left_test(std::size_t value, const std::set<int> &s){
    auto t = s;
    t.erase(value);
    if(t.empty() || value <= *t.begin()){
        return true;
    }
    else return false;
}

inline bool check_good_prefix_property(std::size_t left, std::size_t right, const std::set<int> &s,
    std::size_t bottom_size, std::size_t holes, bool left_mode = false){
    if(left_mode){
        return can_increase(left, bottom_size) && enough_holes(left + 1,s,holes) 
        && !too_many_holes(left + 1, left + 1,bottom_size, holes) && left_test(left + 1,s);
    }
    else{
        return can_increase(right, bottom_size) && enough_holes(right + 1,s,holes) 
        && !too_many_holes(left, right + 1,bottom_size, holes);
    } 
    
}

bool function_generator::increase_gate_link_value(std::size_t level){
    std::size_t top_size = current[level].size(),bottom_size = current[level+1].size();
    std::set<int> s;
    int pos = 0, left = -1, right = -1;
    bool left_increased = false;
    bool delay_increase = false;
    for(int j = top_size - 1; j >= 0; --j){
        pos = j + 1;
        auto n = &current[level][j];
        auto cables = n->get_cables();
        auto left_ = &cables->left.second, right_ = &cables->right.second;
        REDUCE_OCCURRENCE(level, *right_);
        if(check_good_prefix_property(*left_, *right_,s,bottom_size,2*(top_size -  pos))){
            ++*right_;
            s.erase(*right_);  
            occurrences[level][*right_]++;
            left = *left_;
            right = *right_;
            delay_increase = true;
            break;
        }
        else  *right_ = -1;
        if(j == 0) {
            pos = 0; break;
        }
        REDUCE_OCCURRENCE(level, *left_);
        if(check_good_prefix_property(*left_, *right_,s,bottom_size,2*(top_size -  pos) + 1, true)){
            ++*left_;
            ++occurrences[level][*left_];
            s.erase(*left_);
            left = *left_;
            right = left;
            left_increased = true;
            break;
        }
        else *left_ = -1;
    }
    int real_pos = 2*pos;
    if(left_increased){
        --real_pos;
    }
    if(pos == 0) return false;
    while(real_pos < 2*current[level].size()){
        int val = -1;
        if(2*current[level].size() - real_pos > s.size()){
            if(real_pos %2 == 0){
                val = left;
                current[level][real_pos/2].set_left_cable_gate(left);
                s.erase(left);
            }
            else{
                val = right;
                current[level][real_pos/2].set_right_cable_gate(right);
                s.erase(right);
                if(delay_increase){
                    delay_increase = increase_right(left, right, bottom_size);
                }
                else delay_increase = true;
            }
        }
        else{
            val = *s.begin();
            if(real_pos % 2 == 0) current[level][real_pos/2].set_left_cable_gate(*s.begin());
            else current[level][real_pos/2].set_right_cable_gate(*s.begin());
            s.erase(s.begin());
        }
        occurrences[level][val]++;
        ++real_pos;
    }
    return true;
}

void function_generator::reset_gate_value(std::size_t level){
    current[level][0].set_as_or_gate();
    if(current[level][0].is_bypass()){
        current[level][0].set_as_and_gate();
    }
    for(std::size_t j = 1; j < current[level].size(); ++j){
        current[level][j].set_as_or_gate();
        /*funciona porque no hay dos puertas con mismos left y right 
            que cumplan left == right*/
        if(current[level][j] == current[level][j-1]){
            current[level][j].set_as_and_gate();
        }
        else if(current[level][j].is_bypass()){
            current[level][j].set_as_and_gate();
        }
    }
}

/*
void function_generator::increase_depth(){
    ++depth;
    current = vector<vector<node_info>>(depth);
    for(int i = 0; i < depth - 1; ++i){
        current[i].push_back(node_info::get_default(depth, max_depth));
        current[i][0].logic_gate = true;
        current[i][0].set_logic_level(i+1);
        current[i][0].set_logic_id(0, current[i].size());
        current[i][0].data_type.c = '|';
    }
    current[depth - 1].push_back(node_info::get_default(depth, max_depth));
    current[depth - 1][0].logic_gate = false;
    current[depth - 1][0].data_type.pos = 0;
    
}*/

bool function_generator::max_nodes_achieved(std::size_t level){
    if(level + 1 == current.size() - 1) 
        return 2*current[level].size() == current[level+1].size() || 
            current[level+1].size() == input_size;
    else return 2*current[level].size() == current[level+1].size() || 
            current[level+1].size() == size;
}

void function_generator::add_node(std::size_t level){
    node_info n = node_info::get_default
        (level+1, depth,current[level+1].size()+1,current[level].size());
    current[level+1].push_back(n);
    occurrences[level].resize(current[level+1].size());
    for(auto i: occurrences[level]) i = 0;
}



std::vector<std::vector<node_info>> * function_generator::generate_next(){
    if(!cable_level_full_linked(current[current.size() -1].size())){
        increase_cabled_value(current[current.size() -1].size());
        return &current;
    }
    int depth = current.size() - 2;
    //FIXME
    for(; depth >= 0 ; --depth){
        if(!and_gate_value_full(depth)){
            increase_and_gate_value(depth);
            ++depth;
            break;
        }
        else if(increase_gate_link_value(depth)){
            reset_gate_value(depth);
            ++depth;
            break;
        }
        else if(!max_nodes_achieved(depth)){
            add_node(depth);
            reset_gate_value(depth);
            restart_level(depth);
            ++depth;
            break;
        }
    }
    if(depth == -1) return nullptr;
    while(depth < current.size() - 1){
        resize_node_level(depth);
        restart_level(depth);
        ++depth;
    }
    restart_level(depth);
    return &current;

}

std::vector<std::vector<node_info>> * function_generator::get_current() {
    return &current;
}

std::ostream & operator<< (std::ostream &out, 
    const std::vector<std::vector<node_info>> &v){
    for(int i = 0; i < v.size(); ++i){
        for(int j = 0; j < v[i].size() - 1; ++j){
            out << v[i][j] << " | ";
        }
        out << v[i][v[i].size() - 1] << "\n";
    }
    return out;
}

size_t function_generator::get_depth() const{
    return this->depth;
}

size_t function_generator::get_input_size() const{
    return this->input_size;
}

size_t function_generator::get_size() const{
    return this->size;
}