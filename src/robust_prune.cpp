#include "util.hpp"
// #include "types.hpp"
#include "util_types.hpp"

void DirectedGraph::robustPrune(Node p, set<Node> V, int a, int R){

    if (mapKeyExists(p, this->Nout))
        V = setUnion(V, this->Nout[p]);
    
    V.erase(p);
    Node p_opt;

    while (!V.empty()){
        p_opt = myArgMin(V, *p, this->d);
        
        this->Nout[p].insert(p_opt);

        if (this->Nout[p].size() == R)
            break;
        
        // n = p', p_opt = p*
        for (Node n : V){   
            if ( (a * this->d(p_opt, n)) <= this->d(p, n)){
                V.erase(n);
            }
        }
    }
    return;
}