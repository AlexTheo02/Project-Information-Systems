#include "util.hpp"
#include "util_types.hpp"


// creates a random R graph with the existing nodes. Return TRUE if successful, FALSE otherwise
bool DirectedGraph::Rgraph(int R){
    
    // clear all edges in the graph to create an R random graph anew.
    if (!this->clearEdges())
        return false;

    if (R <= log(this->n_nodes)){
        cout << "WARNING: R <= logn and therefore the graph will not be well connected.\n";
    }
    if (R > this->n_nodes - 1){
        cout << "ERROR: R cannot exceed N-1 (N = the total number of nodes in the Graph)";
        return false;
    }

    int r;
    for (Node n : this->getNodes()){
        
        // Copy of nodes in the graph
        set<Node> remaining = this->getNodes();
        // Remove self from remaining nodes
        if (remaining.erase(n) <= 0){
            cout << "ERROR: Failed to remove self from remaining nodes" << endl;
            return false;
        }

        for (int i = 0; i < R; i++){
            
            Node nr;

            nr = sampleFromSet(remaining);
            
            this->addEdge(n,nr); // add the node as neighbor
            remaining.erase(nr);
        }
    }
    return true;
}