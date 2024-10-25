#include "util.hpp"
#include "util_types.hpp"


// creates a random R graph with the existing nodes. Return TRUE if successful, FALSE otherwise
template <typename T>
bool DirectedGraph<T>::Rgraph(int R){
    
    // clear all edges in the graph to create an R random graph anew.
    if (!this->clearEdges())
        return false;

    if (R <= log(this->n_nodes)){
        cout << "WARNING: R <= logn and therefore the graph will not be well connected.\n";
    }
    if (R > this->n_nodes - 1){
        cout << "ERROR: R cannot exceed N-1 (N = the total number of nodes in the Graph)\n";
        return false;
    }

    if (R == 0){
        cout << "WARNING: R is set to 0 and therefore all nodes in the graph are cleared.\n";
    }
    
    if (R < 0) {
        cout << "ERROR: R must be a positive integer.\n";
        return false;
    }

    int r;
    for (T n : this->getNodes()){
        
        // Copy of nodes in the graph
        set<T> remaining(this->getNodes().begin(), this->getNodes().end());
        // Remove self from remaining nodes
        if (remaining.erase(n) <= 0){
            cout << "ERROR: Failed to remove self from remaining nodes\n" << endl;
            return false;
        }

        for (int i = 0; i < R; i++){
            
            T nr;

            nr = sampleFromSet(remaining);
            
            this->addEdge(n,nr); // add the node as neighbor
            remaining.erase(nr);
        }
    }
    return true;
}