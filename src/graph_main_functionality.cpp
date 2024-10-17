#include "util.hpp"
#include "util_types.hpp"

// Creates a node and adds it in the graph
void DirectedGraph::createNode(vector<float> value){
    // Create new node
    Node n = new vector<float>(value);

    // Add it to graph's set of nodes
    this->nodes.insert(n);

    // Increment the number of nodes in graph
    this->n_nodes++;
}

// Adds an directed edge (from->to). Updates outNeighbors(from) and inNeighbors(to)
void DirectedGraph::addEdge(Node from, Node to){
    this->Nout[from].insert(to);
    this->Nin[to].insert(from);
    this->n_edges++;
}

// remove edge
bool DirectedGraph::removeEdge(Node from, Node to){
    // Check if key exists before accessing it (and removing it)
    if (mapKeyExists(from, this->Nout) && mapKeyExists(to, this->Nin)) {
        // Key exists, access the value, if successfully removed, return true
        if (this->Nout[from].erase(to) && this->Nin[to].erase(from)){
            
            // Check if outgoing neighbors are empty, if so, remove entry from unordered map
            if (this->Nout[from].empty())
                this->Nout.erase(from);

            // Check if incoming neighbors are empty, if so, remove entry from unordered map
            if (this->Nin[to].empty())
                this->Nin.erase(to);
            
            // Decrement the number of edges in graph
            this->n_edges--;
            return true;
        }
    }
    return false;
}


// clears all neighbors for a specific node
bool DirectedGraph::clearNeighbors(Node node){

    // Check if node exists before trying to access it
    if (setIn(node,this->nodes)){
        cout << "ERROR: Node does not exist" << endl;
        return false;
    }

    // Node has outgoing neighbors
    if (mapKeyExists(node, this->Nout)){
        for (Node n : this->Nout[node]){
            if (!this->removeEdge(node,n)){
                cout << "ERROR: Failed to remove edge, something went wrong" << endl;
                return false;
            }
        }
        // Check if node has been removed from neighbors map
        if (mapKeyExists(node, this->Nout)){
            cout << "ERROR: Something went wrong when clearing neighbors" << endl;
            return false;
        }
    }
    return true;
}

// clears all edges in the graph
bool DirectedGraph::clearEdges(){
    for (Node n : this->nodes){
        if (!this->clearNeighbors(n)){
            cout << "ERROR: Failed to clear neighbors for node" << endl;
            return false;
        }
    }

    if (this->n_edges != 0 || !this->Nout.empty() || !this->Nin.empty()){
        cout << "ERROR: Failed to clear edges in graph" << endl;
        return false;
    }
    
    return true;
}