#include "util.hpp"
// #include "types.hpp"
#include "util_types.hpp"

bool DirectedGraph::robustPrune(Node p, set<Node> V, float a, int R){

    if (a < 1 || R <= 0){
        cout << "ERROR: Invalid Parameters in robustPrune.\n";
        return false;
    }

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
    return true;
}