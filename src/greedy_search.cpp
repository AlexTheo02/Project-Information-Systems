#include "util.hpp"
#include "types.hpp"

template <typename T>
const vector<set<T>> DirectedGraph<T>::greedySearch(const T& s, T xq, int k, int L) {
    // Create empty sets
    set<T> Lc,V;

    // Initialize Lc with s
    Lc.insert(s);
    
    set<T> diff;
    while(!(diff = setSubtraction(Lc,V)).empty()){
        T pmin = myArgMin(diff, xq, this->d);

        // If node has outgoing neighbors
        if (mapKeyExists(pmin, this->Nout)){
            Lc = setUnion(Lc, this->Nout[pmin]);
        }
        V.insert(pmin);
    }

    if (Lc.size() > L){
        Lc = closestN(L, Lc, xq, this->d);    // function: find N closest points from a specific Xq from given set and return them
    }

    vector<set<T>> ret;
    
    ret.insert(ret.begin(), closestN(k, Lc, xq, this->d));
    ret.insert(ret.end(), V);

    return ret;
}