#include "util.hpp"
// #include "types.hpp"
#include "util_types.hpp"

vector<set<Node>> DirectedGraph::greedySearch(Node s, vector<float> xq, int k, int L){
    // Create empty sets
    set<Node> Lc,V;

    // Initialize Lc with s
    Lc.insert(s);
    
    set<Node> diff;
    while(!(diff = setSubtraction(Lc,V)).empty()){
        Node pmin = myArgMin(diff, xq, euclideanDistance);

        // Error checking 
        if (pmin.getId() < 0) { break; }

        Lc = setUnion(Lc, pmin.getOutNeighbors());
        V.insert(pmin);
    }

    if (Lc.size() > L){
        Lc = closestN(L, Lc, xq, euclideanDistance);    // function: find N closest points from a specific Xq from given set and return them
    }

    vector<set<Node>> ret;
    
    ret.insert(ret.begin(), closestN(k, Lc, xq, euclideanDistance));
    ret.insert(ret.end(), V);

    return ret;
}