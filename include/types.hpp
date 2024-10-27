#pragma once

#include <iostream>
#include <iterator>
#include <cstdlib>  // for rand()
#include <ctime>    // for rand seed (not yet used DO IT)
#include <list>
#include <unordered_map>
#include <vector>
#include <set>
#include <cmath>
#include <functional>
#include <algorithm>
#include <limits>
#include <stdexcept>

#include "util.hpp"

using namespace std;

// Handle for nodes
// typedef vector<float> Node;
// same vector = DUPLICATE??

// Graph Documentation Here
// Attributes required for Constructor are marked with *
// + Number of Edges
// + Number of Nodes
// + Set containing all nodes in the graph
// + Dictionary{node : OutNeighbors}
// + Dictionary{node : InNeighbors}
// + *Distance Function: Node x Node -> float
// IMPORTANT: T must be hashable. If T is not hashable by default, user is required to specialize std::hash function for the desired data type.
template <typename T>
class DirectedGraph{

    private:
    int n_edges;                        // number of edges present in the graph
    int n_nodes;                        // number of nodes present in the graph
    set<T> nodes;                       // a set containing all the nodes in the graph
    unordered_map<T, set<T>> Nout;      // key: node, value: set of outgoing neighbors 
    unordered_map<T, set<T>> Nin;       // key: node, value: set of incoming neighbors
    function<float(T, T)> d;            // Graph's distance function
    
    public:

        // Constructor: Initialize an empty graph
        DirectedGraph(function<float(T, T)> distance_function) {
            this->n_edges = 0;
            this->n_nodes = 0;
            this->d = distance_function;
            cout << "Graph created!" << endl;
        }

        // Return a set of all Nodes in the graph
        const set<T>& getNodes() const { return this->nodes; }

        // Return the number of edges in the graph
        const int& get_n_edges() const { return this->n_edges; }

        // Return the number of nodes in the graph
        const int& get_n_nodes() const { return this->n_nodes; }

        // Return Nout map
        const unordered_map<T, set<T>>& get_Nout() const { return this->Nout; }

        // Return Nin map
        const unordered_map<T, set<T>>& get_Nin() const { return this->Nin; }

        // Creates a node, adds it in the graph and returns it
        typename set<T>::iterator createNode(const T& value);

        // Adds an directed edge (from->to). Updates outNeighbors(from) and inNeighbors(to)
        bool addEdge(const T& from, const T& to);

        // remove edge
        bool removeEdge(const T& from, const T& to);

        // clears all neighbors for a specific node
        bool clearNeighbors(const T& node);

        // clears all edges in the graph
        bool clearEdges();

        // creates a random R graph with the existing nodes. Return TRUE if successful, FALSE otherwise
        bool Rgraph(int R);

        // Greedy search algorithm
        const vector<set<T>> greedySearch(const T& s, T xq, int k, int L);

        // Robust Prune algorithm
        bool robustPrune(T p, set<T> V, float a, int R);

        // Vamana Graph creation algorithm
        bool vamanaAlgorithm(int L, int R);
};

// ------------------------------------------------------------------------------------------------ FUNCTIONALITY

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
    // At least one of the nodes is not present in nodeSet
    if (!setIn(from, this->nodes) || !setIn(to, this->nodes)){
        cout << "Node is not present in nodeSet" << endl;
        return false;
    }

    // Verify that both nodes exist in nodeset
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

    // Check if keys exist before accessing them (and removing them)
    if (mapKeyExists(from, this->Nout) && mapKeyExists(to, this->Nin)) {
        // Key exists, access the value, if successfully removed, return true
        set<T>& nout = this->Nout[from];
        set<T>& nin = this->Nin[to];
        if (nout.erase(to) && nin.erase(from)){
            // Check if outgoing neighbors are empty, if so, remove entry from unordered map
            if (nout.empty()){
                this->Nout.erase(from);
            }

            // Check if incoming neighbors are empty, if so, remove entry from unordered map
            if (nin.empty()){
                this->Nin.erase(to);
            }
            
            // Decrement the number of edges in graph
            this->n_edges--;
            return true;
        }
    }
    cout << "WARNING: Trying to remove non-existing edge" << endl;
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
        // For each outgoing neighbor, remove the edge
        set<T> noutCopy(this->Nout[node].begin(), this->Nout[node].end());
        for (const T& n : noutCopy){
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

// ------------------------------------------------------------------------------------------------ RGRAPH

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
    set<vector<float>> nodesCopy(this->nodes.begin(), this->nodes.end());
    for (T n : nodesCopy){
        
        // Copy of nodes in the graph
        set<T> remaining(nodesCopy.begin(), nodesCopy.end());
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

// ------------------------------------------------------------------------------------------------ GREEDY SEARCH

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

// ------------------------------------------------------------------------------------------------ ROBUST PRUNE

template <typename T>
bool DirectedGraph<T>::robustPrune(T p, set<T> V, float a, int R){

    if (a < 1 || R <= 0){
        cout << "ERROR: Invalid Parameters in robustPrune.\n";
        return false;
    }

    if (mapKeyExists(p, this->Nout))
        V = setUnion(V, this->Nout[p]);
    
    V.erase(p);
    T p_opt;

    while (!V.empty()){
        p_opt = myArgMin(V, *p, this->d);
        
        this->Nout[p].insert(p_opt);

        if (this->Nout[p].size() == R)
            break;
        
        // n = p', p_opt = p*
        for (T n : V){   
            if ( (a * this->d(p_opt, n)) <= this->d(p, n)){
                V.erase(n);
            }
        }
    }
    return true;
}

// ------------------------------------------------------------------------------------------------ VAMANA GRAPH

template <typename T>
bool DirectedGraph<T>::vamanaAlgorithm(int L, int R){  // should "a" be added as a parameter?


    // check parameters if they are in legal range, for example R > 0 
    if (R <= 0){    // ADD MORE
        cout << "ERROR: Invalid Parameter Values on vamanaAlgorithm.\n";
        return false;
    }

    if (this->Rgraph(R) == false)    // Initializing graph to a random R-Regular Directed Graph
        return false;

    T s = medoid(this->nodes, this->d);
    vector<T> perm = permutation(this->nodes);

    for (T si : perm){
        vector<set<T>> rv = greedySearch(s, *si, 1, L);
        set<T> Lc = rv[0];
        set<T> V = rv[1];

        float a = 1.0f;  // WHAT IS a? a >= 1 ? should we check in arguments?

        if (this->robustPrune(si, V, a, R) == false)
            return false;

        for (T j : this->Nout[si]){
            
            set<T> noutJsi;
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