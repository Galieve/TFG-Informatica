#include <random>
#include <vector>
#include <string>
#include <sstream>
#include <cassert>
#include <utility>
#include <algorithm>
#include <functional>
#include <fstream>
#include <iomanip>
#include <sstream>
#include "../function/node_info.cpp"
#include "../function/function.cpp"
#include "../function/node.cpp"
#include <unistd.h>

void sort_vector(std::vector<std::pair<int,int>> &v){
    std::sort(v.begin(), v.end(), [](std::pair<int,int> &a, std::pair<int,int> &b){
        return a.first < b.first || (a.first == b.first && a.second < b.second);
    });
}

void generate_input_vector(std::vector<int> &v){
    for(int i = 0; i < v.size(); ++i){
        v[i] = i;
    }
}

//2k >= p, k <= 2p <=> k/2 <= p <= 2k
std::vector<std::pair<int,int>> random_connection_top(int p, int k){
    std::vector<int> list;
    std::vector<int>v(k);
    generate_input_vector(v);
    bool finished = false;
    std::vector<std::pair<int,int>> pv;
    while(!finished){
        list.clear();
        pv.clear();
        while(list.size() < 2*p){
            std::random_shuffle ( v.begin(), v.end() );
            int h = 2*p - list.size();
            list.insert(list.end(), v.begin(), v.begin() + 
                std::min(k, h));
        }   
        for(int i = 0; i < list.size() / 2; ++i){
            std::pair<int,int> p = 
            {std::min(list[2*i], list[2*i+1]), std::max(list[2*i], list[2*i+1])};
            pv.push_back(p);
        }
        sort_vector(pv);
        int count = 0;
        auto p = pv[0];
        for(int i = 1; i < pv.size(); ++i){
            if(pv[i] == p) ++count;
            else{
                count = 0;
                p = pv[i];
            }
            if(count == 2) break;
        }
        if(count < 2) finished = true;
    }
    
    return pv;
}

 inline bool increase_right(int &left, int &right, int bottom_size){
    bool delay = true;
    ++right;
    if(right == bottom_size) {
        ++left, right = left; 
        delay = false;
    }
    return delay;
}

std::vector<std::pair<int,int>> random_connection_bottom(int p, int k){
    assert(k*k == p);
    int l = 0, r = 0;
    bool delay = false;
    std::vector<std::pair<int,int>> pv;
    pv.push_back({l, r});
    for(int i = 1; i < p; ++i){
        if(!delay){
            delay = increase_right(l, r, k);
        }
        else{
            delay = !delay;
        }
        pv.push_back({l, r});
    }
    return pv;
}


std::vector<char> random_gates(const std::vector<std::pair<int,int>> &vp){
    std::random_device dev;
    std::mt19937 generator(dev());
    std::uniform_int_distribution<int> distribution(0,1);
    std::vector<int> g(vp.size());
    for(auto & i: g){
        i = distribution(generator);
    }
    std::vector<char> vc(g.size(), 'b');
    for(int i = 0; i < vc.size(); ++i){
        if(g[i] == 0)
            vc[i] = 'o';
        else
            vc[i] = 'a';
    }
    assert(vc.size() == vp.size());
    for(int i = 0; i < vc.size() - 1; ++i){
        auto p = vp[i];
        auto q = vp[i+1];
        if(p.first == p.second){
            vc[i] = 'b';
        }
        else if(p.first == q.first && p.second == q.second){
            vc[i] = 'o';
            vc[i+1] = 'a';
        }
    }
    return vc;
}

void fill_diamond_circuit(std::vector<std::vector<std::string>> &v, int l){
    
    int n = v.size();
    std::string sp = " ";
    std::vector<std::pair<int,int>> vp;
    for(int i = 0; i < n - 1; ++i){
        if(i < l)
            vp = random_connection_top(v[i].size(), v[i+1].size());
        else
            vp = random_connection_bottom(v[i].size(), v[i+1].size());
        ////std::cout << "Nivel "<<i<<"\n";
        auto vg = random_gates(vp);
        for(int j = 0; j < vg.size(); ++j){
            v[i][j] = "" + std::string(1,vg[j]) + sp + std::to_string(vp[j].first) 
                + sp +std::to_string(vp[j].second);
        }
        
    }
    auto m = v.back().size();
    for(int j = 0; j < m; ++j){
        v[n-1][j] = "c "+std::to_string(j);
    }

    
}

void set_random_sizes_diamond_circuit(
    std::vector<std::vector<std::string>> &v, int w, int l, int m){
    std::random_device dev;
    std::mt19937 generator(dev());
    
    int s = m;
    int n = v.size();
    for(int i = n-1; i >= l; --i){
        v[i] = std::vector<std::string>(s, "");
        s = s*s;
    }
    auto eta = w;
    for(int i = l - 1; i >= 0; --i){
        eta = std::ceil(eta / 2.0);
        std::uniform_int_distribution<int> distribution(eta,pow(2,i));
        auto k = distribution(generator);
        v[i] = std::vector<std::string> (k, "");
    }
}

std::vector<std::string> get_random_diamond_circuit(int m, int l){
    
    auto w = pow(m, pow(2, l));
    auto d = ceil(log2(w));
    int n = l + d + 1;
    //std::cout << n << " " << w << "\n";
    std::vector<std::vector<std::string>> vc(n);
    set_random_sizes_diamond_circuit(vc,w,n-l-1,m);
    //std::cout << "tamaños" << "\n";
    if(vc[0].size() != 1){
        //std::cout << ":(" << "\n";
        return {};
    }
    /*for(int i = 0; i < n; ++i){
        //std::cout << vc[i].size() << " - ";
    }
    //std::cout << "\n";*/
    fill_diamond_circuit(vc, n-l-1);
    std::vector<std::string> vs;
    for(int i = 0; i < vc.size(); ++i){
        std::string s = vc[i][0];
        for(int j = 1; j < vc[i].size(); ++j){
            s += " " + vc[i][j];
        }
        vs.push_back(s);
    }
    return vs;
}

void generate_diamond_circuits(){

    int m = 10;
    for(int idx =0; idx < 10; ++idx){
        std::ofstream out("random_circuits/diamond_circuits"+
            std::to_string(idx)+".out");


        std::random_device dev;
        std::mt19937 generator(dev());
        std::uniform_int_distribution<int> distribution(1,2);

        int l = distribution(generator);
        
        auto w = pow(m, pow(2, l));
        auto d = ceil(log2(w));
        int n = l + d + 1;
        
        int m = 2*5;
        std::vector<int> ops(m);
        generate_input_vector(ops);

        for(int i = 0; i < 1000; ++i){
            if(i % 47 == 0)
                std::cout << "Vamos por un:"<< std::fixed << 
                    std::setprecision(2) << (i+1000*idx)/100.0 << "% \n";
            auto vs = get_random_diamond_circuit(m, l);
            if(vs.size() == 0)
                continue;
            auto vvni = node_info::generate_vvnode_info(vs);
            function f = function::build(*vvni, w, m);
            f.evaluate_all();
            out << f.get_id() << "\n";
            ////std::cout << f.get_id() << "\n";
            for(int i = 0; i < vs.size(); ++i){
                out << vs[i] << "\n";
                ////std::cout << vs[i] << "\n";
            }
            out <<"\n";
        }
        out.close();
    }
    
}

std::string add_palm_size(std::string &s, int &palm){
    std::stringstream ss(s);
    std::string r = "";
    char c;
    int a, b;
    int maxi = 0;
    std::string sp = "";
    while(ss >> c){
        ss >> a >> b;
        r += sp + std::string(1,c) + " "+ std::to_string(a+palm) 
            + " "+ std::to_string(b+palm);
        maxi = std::max(maxi, b);
        sp = " ";
    }
    palm += maxi + 1;
    return r;
}


std::vector<std::string> combine_palm(std::vector<std::vector<std::string>>&v){
    std::vector<std::string> vs;
    auto depth = v.size() - 1;
    //std::cout << v.size() << "\n";
    for(int j = 0; j < v[0].size() - 1; ++j){
        int palm_size = 0;
        std::string level= "";
        std::string space = "";
        //std::cout << "Nivel: "<<j <<" tam palma"<<v.size()<<"\n";
        for(int i = 0; i < v.size(); ++i){
            //std::cout<<"palma: "<<palm_size<<"\n";
            auto s = add_palm_size(v[i][j], palm_size);
            level += space + s;
            space = " ";
        }
        //std::cout << level << "\n";
        // std::cout << palm_size<<"\n";
        vs.push_back(level);
    }
    return vs;
}

std::vector<std::string> get_random_palm_diamond_circuit(){
    int l = 1;

    int m = 25;
    std::vector<std::vector<std::string>> vvs;
    //std::cout << "Vamos por aquí\n";
    for(int pid = 0; pid < 4; ++pid){
        auto vs_ = get_random_diamond_circuit(m, l);
        while(vs_.size() == 0)
            vs_ = get_random_diamond_circuit(m, l);
        vvs.push_back(vs_);
    }
    //std::cout << "4 hojas creadas\n";
    std::vector<std::string> vs = combine_palm(vvs);
    // std::cout <<"Nivel 3 " << vs[0]<<"\n";

    auto vp = random_connection_top(2, 4);
    auto vg = random_gates(vp);
    sort_vector(vp);

    std::string new_str ="";
    std::string sp = "";
    
    
    for(int j = 0; j < vg.size(); ++j){
        new_str += sp + std::string(1,vg[j]) + " " + std::to_string(vp[j].first) 
            + " " +std::to_string(vp[j].second);
        sp = " ";
    }
    // std::cout <<"Nivel 2" << new_str<<"\n\n";
    vs.insert(vs.begin() + 0, new_str);

    new_str = "";
    sp = "";
    vp = random_connection_top(1, 2);
    vg = random_gates(vp);
    sort_vector(vp);


    for(int j = 0; j < vg.size(); ++j){
        new_str += sp + std::string(1,vg[j]) + " " + std::to_string(vp[j].first) 
            + " " +std::to_string(vp[j].second);
        sp = " ";
    }
    // std::cout <<"Cabecera"  << new_str<<"\n";
    vs.insert(vs.begin() + 0,new_str);


    vp = random_connection_bottom(100, 10);
    vg = random_gates(vp);
    sort_vector(vp);


    new_str = "";
    sp = "";
    for(int j = 0; j < vg.size(); ++j){
        new_str += sp + std::string(1,vg[j]) + " " + std::to_string(vp[j].first) 
            + " " +std::to_string(vp[j].second);
        assert(vp[j].first <= vp[j].second);
        sp = " ";
    }
    vs.push_back(new_str);

    new_str = "";
    sp = "";
    for(int j = 0; j < 10; ++j){
        new_str += sp + "c "+std::to_string(j);
        sp = " ";
    }
    vs.push_back(new_str);
    int i = 0;
    // for(auto s: vs){
    //     std::cout <<"Linea " <<i <<": "<< s << "\n";
    //     ++i;
    // }
    // std::cout << "-----\n\n";
    return vs;
}

void generate_palm_diamond_circuits(){
    for(int idx =0; idx < 20; ++idx){
        std::ofstream out("random_circuits/palm_diamond"+
            std::to_string(idx)+".out");
        
        int m = 2*5;
        int w = 625*4;
        std::vector<int> ops(m);
        generate_input_vector(ops);

        for(int i = 0; i < 500; ++i){
            if(i % 23 == 0)
                std::cout << "Vamos por un:"<< std::fixed << 
                    std::setprecision(2) << (i+500*idx)/100.0 << "% \n";

            auto vs = get_random_palm_diamond_circuit();
            auto vvni = node_info::generate_vvnode_info(vs);
            function f = function::build(*vvni, w, m);
            f.evaluate_all();
            out << f.get_id() << "\n";
            ////std::cout << f.get_id() << "\n";
            for(int i = 0; i < vs.size(); ++i){
                out << vs[i] << "\n";
                ////std::cout << vs[i] << "\n";
            }
            out <<"\n";
        }
        out.close();
    }
}


int main(){
    generate_palm_diamond_circuits();
    return 0;
}

