#include "circuit.h"

void circuit::calculate_logic_gates(){
    
    logic_gates = 0;
    for(int i = 0; i < (*logic_circuit_ptr).size(); ++i){
        logic_gates += (*logic_circuit_ptr)[i].size();
    }
}