#include "function_generator.h"
#include <cassert>
#include <set>
#include <cmath>
#include <sstream>
#include <exception>

function_generator::function_generator(std::size_t depth, std::size_t size, std::size_t input_size)
    : depth(depth), max_depth(depth), input_size(input_size), size(size){
    occurrences.resize(depth - 1,std::vector<std::size_t>(1,0));
    current.resize(depth);
    std::size_t s = size;
    resize_floor(current.size() - 1,ceil(sqrt(s)));
    for(int i = current.size() - 2; i >= 0; --i){
        resize_floor(i,s);
        occurrences[i].resize(current[i+1].size());
        s = ceil(s/2.0);
    }
    if(depth > 1)
        completed_levels.insert(depth - 2);

    if(current[0].size() > 1){
        std::stringstream ss;
        ss <<"depth = "<<depth << " size = "<<size
            <<" input_size = "<<input_size<<"\n";
        throw std::invalid_argument(ss.str());
    }
    for(int i = 0; i < current.size() - 1; ++i){
        restart_level(i);
        reset_gate_value(i);
    }
    restart_level(current.size() - 1);
    completed_levels.erase(depth - 1);    
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
    occurrences[level].resize(current[level + 1].size());
    for(auto &i: occurrences[level]) i = 0;
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
void function_generator::resize_floor(std::size_t depth, std::size_t elems){
    //assert(depth > 0);
    current[depth].resize(elems);
    if(elems == size){
        completed_levels.insert(depth);
    }
    else{
        completed_levels.erase(depth);
    }
    //occurrences[depth - 1].resize(elems);
    for(int j = 0; j < current[depth].size(); ++j){
        //bottom_size sin calcular. 
        //Aproximamos por 2*elems (factible pero incorrecto).
        //Debe ser usada esta función con restart_level.
        current[depth][j] = node_info::get_default(depth,current.size(),elems,2*elems);
    }
    
}

void function_generator::complete_last_floors(std::size_t level){
    std::size_t reserved_levels = ceil(log2(size));
    resize_floor(level, occurrences[level - 1].size());

    if(level == current.size() - 1){
        restart_level(current.size() - 1);
        return ;
    }

    auto next_level = level + 1;
    while (next_level < current.size() - (reserved_levels + 1) || !completed_levels.empty()){
        if(next_level == current.size() - 1){
            resize_floor(next_level, ceil(sqrt(current[next_level - 1].size())));
            break;
        }
        resize_node_level(next_level);
        ++next_level;
    }
    if(completed_levels.empty()){
        //if level < depth - 3 => !completed_levels.empty()
        std::vector<std::size_t> max_vec(std::min(reserved_levels, depth - level - 2));
        std::size_t max_value = size;
        for(int i = 0; i < max_vec.size(); ++i){
            max_vec[i] = max_value;
            max_value /= 2;
        }
        max_value = std::ceil(sqrt(current[level].size()));
        for(int i = max_vec.size() - 1; i >= 0; --i){
            max_vec[i] = std::max(max_vec[i], max_value);
            max_value = std::ceil(std::sqrt(max_vec[i]));
        }
        for(int i = 0; i < max_vec.size(); ++i){
            resize_floor(depth - 2 - i, max_vec[i]);
        }
        resize_floor(current.size() - 1, ceil(sqrt(current[current.size() - 2].size())));
    }
    for(int i = level; i < current.size() - 1; ++i){
        restart_level(i);
        reset_gate_value(i);
    }
    restart_level(current.size() - 1);
    // int reserved_levels = ceil(log2(size));
    // while(level < current.size() - (reserved_levels + 1) || !completed_levels.empty()){
    //     if(level == current.size() - 1){
    //         restart_level(level);
    //         return;
    //     }
    //     resize_node_level(level);
    //     restart_level(level);
    //     reset_gate_value(level);
    //     ++level;
    // }
    // //completed_levels == empty()
    // //CUIDAO
    // //depth + reserved_levels == cable
    // std::size_t s = size;
    // resize_floor(current.size() - 1, ceil(sqrt(size)));
    // restart_level(current.size() - 1);
    // for(int i = 0; i < reserved_levels && current.size() - 2 - i >= level ; ++i){
    //     std::size_t floor = current.size() - 2 - i;
    //     resize_floor(floor, s);
    //     restart_level(floor);
    //     reset_gate_value(floor);
    //     s = ceil(s/2.0);
    //     //current[depth].resize(1 << reserved_levels);
    // }
    
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
    if(init == size)
        completed_levels.insert(level + 1);
    else
        completed_levels.erase(level + 1);
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
}

void function_generator::restart_cable_level(std::size_t level){
    assert(level == current.size() - 1);
    for(std::size_t j = 0; j < current[level].size(); ++j){
        current[level][j].set_cable_id(j);
        //current[level][j].se
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
        if(current[i][j].is_and_gate()) continue;
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

#define UPDATE_RIGHT(VAL) \
    *right_ = VAL; \
    s.erase(*right_); \
    occurrences[level][*right_]++; \
    left = *left_; \
    right = *right_; \
    delay_increase = true; \
    break;

#define UPDATE_LEFT(VAL) \
    *left_ = VAL; \
    ++occurrences[level][*left_]; \
    s.erase(*left_); \
    left = *left_; \
    right = left; \
    left_increased = true; \
    break;

inline bool is_valid_value(std::size_t value, std::size_t bottom_size) {
    return value < bottom_size;
}

inline bool enough_gaps(std::size_t value, const std::set<int> &set_, std::size_t gaps){
    auto s = set_;
    s.erase(value);
    return s.size() <= gaps;
}

inline bool too_many_gaps(std::size_t left, std::size_t right, std::size_t bottom_size, 
    std::size_t gaps){
    node_info n;
    n.set_bottom_size(bottom_size);
    n.set_left_cable_gate(left),n.set_right_cable_gate(right);
    if(left == right)
        n.set_as_and_gate();
    else
        n.set_as_or_gate();
    return 2*(bottom_size * bottom_size - 1 - n.get_logic_id()) < gaps;
}

inline bool left_test(std::size_t value, const std::set<int> &s){
    auto t = s;
    t.erase(value);
    if(t.empty() || value <= *t.begin()){
        return true;
    }
    else return false;
}

inline bool check_good_prefix_property(std::size_t left, std::size_t right, std::size_t value,
    const std::set<int> &s, std::size_t bottom_size, std::size_t gaps, bool left_mode = false){
    if(left_mode){
        auto t = s;
        t.erase(value);
        if(gaps > t.size()){
            return is_valid_value(value, bottom_size) && enough_gaps(value,s,gaps) 
                && !too_many_gaps(value, value ,bottom_size, gaps - 1) && left_test(value,s);
        }
        else if(t.upper_bound(left) != t.end()) {
            return is_valid_value(value, bottom_size) && enough_gaps(value,s,gaps) 
                && !too_many_gaps(value, *t.upper_bound(left) ,bottom_size, gaps - 1) 
                && left_test(value,s);
        }
        else return false;
    }
    else{
        return is_valid_value(value, bottom_size) && enough_gaps(value,s,gaps) 
            && !too_many_gaps(left, value, bottom_size, gaps);
    } 
    
}

bool function_generator::increase_gate_link_value(std::size_t level){
    std::size_t top_size = current[level].size(),bottom_size = current[level+1].size();
    std::set<int> s;
    int pos = 0, left = -1, right = -1;
    bool left_increased = false;
    bool delay_increase = false;
    std::size_t gaps = 0;
    std::size_t value;
    for(int j = top_size - 1; j >= 0; --j){
        pos = j + 1;
        auto n = &current[level][j];
        auto cables = n->get_cables();
        std::size_t * left_ = &cables->left.second;
        std::size_t * right_ = &cables->right.second;
        REDUCE_OCCURRENCE(level, *right_)
        value = *right_ + 1;
        if(check_good_prefix_property(*left_, *right_,value, s,bottom_size,gaps)){
            UPDATE_RIGHT(value)
        }
        else{
            if(s.upper_bound(*right_) != s.end()) value = *s.upper_bound(*right_);
            else value = -1;
            if(value != -1 && 
                check_good_prefix_property(*left_, *right_,value, s,bottom_size,gaps)){
                    UPDATE_RIGHT(value)
                }
            else *right_ = -1;
        }
        if(j == 0) {
            pos = 0; break;
        }
        ++gaps;
        value = *left_ + 1;
        REDUCE_OCCURRENCE(level, *left_);
        if(check_good_prefix_property(*left_, *right_,value, s,bottom_size,gaps, true)){
            UPDATE_LEFT(value)
        }
        else{
            if(s.upper_bound(*left_) != s.end()) value = *s.upper_bound(*left_);
            else value = -1;
            if(value != -1 && 
                check_good_prefix_property(*left_, *right_,value, s,bottom_size,gaps, true)){
                    UPDATE_LEFT(value)
                }
            else *left_ = -1;
        }
        ++gaps;
    }

    if(pos == 0) return false;

    //Complete gaps
    int real_pos = 2*pos;
    int empty_gaps = gaps;

    if(left_increased){
        if(empty_gaps > s.size()){
            current[level][real_pos/2 - 1].set_right_cable_gate(left);
            occurrences[level][left]++;
            s.erase(left); //Doesn't do anything.
            right = left;
            delay_increase = increase_right(left, right, bottom_size);
        }
        else{
            current[level][real_pos/2 - 1].set_right_cable_gate(*s.upper_bound(left));
            occurrences[level][*s.begin()]++;
            s.erase(*s.begin());

        }
        --empty_gaps;
    }

    
    assert(empty_gaps %2 == 0);
    assert(empty_gaps >= s.size());
    while(empty_gaps > 0){

        // Hay espacio para left y right sin problema.
        if(empty_gaps - 2 >= s.size()){
            current[level][real_pos/2].set_left_cable_gate(left);
            s.erase(left);
            occurrences[level][left]++;
            current[level][real_pos/2].set_right_cable_gate(right);
            s.erase(right);
            occurrences[level][right]++;
            
            if(delay_increase){
                delay_increase = increase_right(left, right, bottom_size);
            }
            else delay_increase = true;
            real_pos += 2;
            empty_gaps -= 2;
        }
        // Entra solo el valor izquierdo. ¡Cuidado!
        else if(empty_gaps - 1 == s.size()){
            auto past_node = current[level][real_pos/2 - 1].get_cables();
            // En este caso (left, *s.begin()) es la menor pareja posible a completar.
            if (left > past_node->left.second){
                /* CUIDADO en este caso*/
                right = left;
                if(s.find(right) == s.end()){
                    assert(s.upper_bound(right) != s.end());
                    right = *s.upper_bound(right);
                }

                current[level][real_pos/2].set_left_cable_gate(left);
                current[level][real_pos/2].set_right_cable_gate(right);
                occurrences[level][left]++;
                occurrences[level][right]++;
                s.erase(left);
                s.erase(right);

            }
            else if(past_node->right.second < *(--s.end())){
                int new_right = right;
                if(s.find(right) == s.end())
                    new_right = *s.upper_bound(right);
                current[level][real_pos/2].set_left_cable_gate(left);
                current[level][real_pos/2].set_right_cable_gate(new_right);
                occurrences[level][left]++;
                occurrences[level][new_right]++;
                s.erase(left);
                s.erase(new_right);
            }
            else{
                current[level][real_pos/2].set_left_cable_gate(left + 1);
                current[level][real_pos/2].set_right_cable_gate(*s.begin());
                occurrences[level][left+1]++;
                occurrences[level][*s.begin()]++;
                s.erase(s.begin());
            }
            real_pos += 2;
            empty_gaps -= 2;
        }
        // Hay que completar los gaps con los elementos no utlizados.
        else{
            while(empty_gaps > 0) {
                if(real_pos % 2 == 0) 
                    current[level][real_pos/2].set_left_cable_gate(*s.begin());
                else 
                    current[level][real_pos/2].set_right_cable_gate(*s.begin());
                occurrences[level][*s.begin()]++;
                s.erase(s.begin());
                ++real_pos;
                --empty_gaps;
            }     
            
        }
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
    if(current[level + 1].size() == size && level + 1 != depth - 1){
        completed_levels.insert(level + 1);
    }
    occurrences[level].resize(current[level+1].size());
    for(auto &i: occurrences[level]) i = 0;
}



std::shared_ptr<vvnode_info> function_generator::generate_next(){
    if(nullptr_flag) return nullptr;
    if(!cable_level_full_linked(current[current.size() -1].size())){
        increase_cabled_value(current[current.size() -1].size());
        return std::make_shared<vvnode_info>(current);
    }
    //completed_levels.erase(current.size() - 1);
    int depth = current.size() - 2;
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
            restart_level(depth);
            reset_gate_value(depth);
            ++depth;
            break;
        }
        else completed_levels.erase(depth);
    }
    if(depth == -1){
        nullptr_flag = true;
        return nullptr;
    }
    complete_last_floors(depth);
    return std::make_shared<vvnode_info>(current);

}

void function_generator::set_current(std::shared_ptr<vvnode_info> &curr){
    if(curr == nullptr){
        nullptr_flag = true;
        return;
    }
    depth = curr->size();
    size = 1;
    for(int i = 0; i < curr->size() -1; ++i) size = std::max(size, (*curr)[i].size());
    completed_levels.clear();
    occurrences.resize(depth - 1);

    for(int i = 0; i < curr->size() - 1; ++i){
        if((*curr)[i].size() == size)
            completed_levels.insert(i);
        occurrences[i].resize((*curr)[i + 1].size());
        for(auto &it : occurrences[i]) it = 0;
        for(int j = 0; j < (*curr)[i].size(); ++j){
            occurrences[i][(*curr)[i][j].get_cables()->left.second]++;
            occurrences[i][(*curr)[i][j].get_cables()->right.second]++;
        }

    }
    assert(!completed_levels.empty() || depth == 1);
    current = *curr;
}

std::shared_ptr<vvnode_info> function_generator::get_current() const{
    if(nullptr_flag) return nullptr;
    else return std::make_shared<vvnode_info>(current);
}

std::size_t function_generator::get_depth() const{
    return this->depth;
}

std::size_t function_generator::get_input_size() const{
    return this->input_size;
}

std::size_t function_generator::get_size() const{
    return this->size;
}