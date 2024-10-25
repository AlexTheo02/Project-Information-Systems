#include "util.hpp"
// #include "types.hpp"
#include "util_types.hpp"

template <typename T>
vector<set<T>> DirectedGraph<T>::greedySearch(T s, T xq, int k, int L){
    // Create empty sets
    set<Node> Lc,V;

    // Initialize Lc with s
    Lc.insert(s);
    
    set<Node> diff;
    while(!(diff = setSubtraction(Lc,V)).empty()){
        Node pmin = myArgMin(diff, xq, this->d);

        // Error checking 
        if (pmin == NULL) { 
            cout << "ERROR: No closest point found (greedySearch)" << endl;
            break;
        }
        // If node has outgoing neighbors
        if (mapKeyExists(pmin, this->Nout)){
            Lc = setUnion(Lc, this->Nout[pmin]);
        }
        V.insert(pmin);
    }

    if (Lc.size() > L){
        Lc = closestN(L, Lc, xq, this->d);    // function: find N closest points from a specific Xq from given set and return them
    }

    vector<set<Node>> ret;
    
    ret.insert(ret.begin(), closestN(k, Lc, xq, this->d));
    ret.insert(ret.end(), V);

    return ret;
}