#include "util.hpp"
#include "types.hpp"

// Creates a node, adds it in the graph and returns it
template<typename T>
typename set<T>::iterator DirectedGraph<T>::createNode(const T& value){
    // https://cplusplus.com/reference/set/set/insert/

    // Add the value to graph's set of nodes
    pair<typename set<T>::iterator, bool> ret;
    ret = this->nodes.insert(value);

    // Increment the number of nodes in graph (if insertion was successful)
    if (ret.second)
        this->n_nodes++;
    
    // return an iterator to the inserted element (or the already existing one)
    return ret.first;
}

// Adds an directed edge (from->to). Updates outNeighbors(from) and inNeighbors(to)
template <typename T>
bool DirectedGraph<T>::addEdge(const T& from, const T& to){
    
    if (from == to)
        return false;
    this->Nout[from].insert(to);
    this->Nin[to].insert(from);
    this->n_edges++;
    return true;
}

// remove edge
template <typename T>
bool DirectedGraph<T>::removeEdge(const T& from, const T& to){
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
template <typename T>
bool DirectedGraph<T>::clearNeighbors(const T& node){
    
    // Check if node exists before trying to access it
    if (!setIn(node,this->nodes)){
        cout << "ERROR: Node does not exist in set" << endl;
        return false;
    }

    // Node has outgoing neighbors
    if (mapKeyExists(node, this->Nout)){
        for (T n : this->Nout[node]){
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
template <typename T>
bool DirectedGraph<T>::clearEdges(){
    for (T n : this->nodes){
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
