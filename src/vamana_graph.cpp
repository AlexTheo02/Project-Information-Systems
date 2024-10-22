#include "util.hpp"
// #include "types.hpp"
#include "util_types.hpp"

bool DirectedGraph::vamanaAlgorithm(int L, int R){  // should "a" be added as a parameter?


    // check parameters if they are in legal range, for example R > 0 
    if (R <= 0){    // ADD MORE
        cout << "ERROR: Invalid Parameter Values on vamanaAlgorithm.\n";
        return false;
    }

    if (this->Rgraph(R) == false)    // Initializing graph to a random R-Regular Directed Graph
        return false;

    Node s = medoid(this->nodes, this->d);
    vector<Node> perm = permutation(this->nodes);

    for (Node si : perm){
        vector<set<Node>> rv = greedySearch(s, *si, 1, L);
        set<Node> Lc = rv[0];
        set<Node> V = rv[1];

        float a = 1.0f;  // WHAT IS a? a >= 1 ?

        if (this->robustPrune(si, V, a, R) == false)
            return false;

        for (Node j : this->Nout[si]){
            
            
            set<Node> noutJsi;
            if (mapKeyExists(j, this->Nout)){   // if node j has no neighbors the set is the empty set U {σ(i)}
                noutJsi.insert(this->Nout[j].begin(), this->Nout[j].end());
            }
            
            noutJsi.insert(si);

            if (noutJsi.size() > R){
                if(this->robustPrune(j, noutJsi, a, R))
                    return false;
            }
            else{
                this->addEdge(j, si);
            }
        }
    }
    return true;
}