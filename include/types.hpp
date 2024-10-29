#pragma once

#include <iostream>
#include <iterator>
#include <cstdlib>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <cmath>
#include <functional>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <thread>

#include "util.hpp"

using namespace std;

// Directed Graph Class Template:
// This implementation of a Directed Graph Class makes use of dictionaries/maps for adjacency lists.
// To instantiate such a Directed Graph Object, you will need to specify the Content Type T, as well as provide:
// 1. Distance Function: T x T -> float => float distance_function(T,T)
// 2. (Optional) Content type T Valid Check Function: T -> bool => bool isEmpty(T) (Default is an AlwaysValid function that returns false for any input)
//
// IMPORTANT: You are also required to specialize the std namespace with an appropriate implementation of std::hash<T> for your specific content type.
// see more on specializing std::hash<T> on: https://en.cppreference.com/w/cpp/utility/hash/operator()
template <typename T>
class DirectedGraph{

    private:
    int n_edges;                            // number of edges present in the graph
    int n_nodes;                            // number of nodes present in the graph
    unordered_set<T> nodes;                 // an unordered_set containing all the nodes in the graph
    vector<T> _nodes;                       // vector representation of nodes used for medoid
    unordered_map<T, unordered_set<T>> Nout;// key: node, value: unordered_set of outgoing neighbors 
    unordered_map<T, unordered_set<T>> Nin; // key: node, value: unordered_set of incoming neighbors
    function<float(T, T)> dst;              // Graph's distance function
    function<bool(T)> isEmpty;              // typename T valid check
    unordered_map<T,unordered_map<T,float>> _distances;
    
    
    public:

        // Constructor: Initialize an empty graph
        DirectedGraph(function<float(T, T)> distance_function, function<bool(T)> is_Empty = alwaysValid<T>) {
            this->n_edges = 0;
            this->n_nodes = 0;
            this->dst = distance_function;
            this->isEmpty = is_Empty;
            cout << "Graph created!" << endl;
        }

        // Return a unordered_set of all Nodes in the graph
        const unordered_set<T>& getNodes() const { return this->nodes; }

        // Return the number of edges in the graph
        const int& get_n_edges() const { return this->n_edges; }

        // Return the number of nodes in the graph
        const int& get_n_nodes() const { return this->n_nodes; }

        // Return Nout map
        const unordered_map<T, unordered_set<T>>& get_Nout() const { return this->Nout; }

        // Return Nin map
        const unordered_map<T, unordered_set<T>>& get_Nin() const { return this->Nin; }

        // Creates a node, adds it in the graph and returns it
        typename unordered_set<T>::iterator createNode(const T& value);

        // Adds an directed edge (from->to). Updates outNeighbors(from) and inNeighbors(to)
        bool addEdge(const T& from, const T& to);

        // remove edge
        bool removeEdge(const T& from, const T& to);

        // clears all neighbors for a specific node
        bool clearNeighbors(const T& node);

        // clears all edges in the graph
        bool clearEdges();

        // distance function that checks if the distances are precomputed or else computes it and updates the precomputed distances
        const float d(T t1,T t2);

        // calculates the medoid of the nodes in the graph. Also updates the precomputed distance matrix (for optimization purposes)
        T medoid(void);

        T parallel_medoid(int n_threads);

        void _thread_medoid_fn(int start_index, int end_index, T& local_minimum, float& local_dmin);

        // creates a random R graph with the existing nodes. Return TRUE if successful, FALSE otherwise
        bool Rgraph(int R);

        // Greedily searches the graph for the k nearest neighbors of query xq (in an area of size L), starting the search from the node s.
        // Returns a unordered_set with the k closest neighbors (returned_vector[0]) and a unordered_set of all visited nodes (returned_vector[1]).
        const vector<unordered_set<T>> greedySearch(const T& s, T xq, int k, int L);

        // Prunes out-neighbors of node p up until a minimum threshold R of out-neighbors for node p, based on distance criteria with parameter a.
        void robustPrune(T p, unordered_set<T> V, float a, int R);

        // Transforms the graph into a Directed Graph such that it makes the finding of nearest neighbors easier.
        // Parameters:
        // + R the out-degree of each node in the graph (R >= 1)
        // + L the area parameter for searching (L >= k >= 1, where k is the desired number of neighbors)
        // + a the parameter for robust pruning (a >=1)
        bool vamanaAlgorithm(int L, int R, float a);
};

// Implementation of already declared Graph Template: MAIN FUNCTIONALITY ----------------------------------- //

// Creates a node, adds it in the graph and returns it
template<typename T>
typename unordered_set<T>::iterator DirectedGraph<T>::createNode(const T& value){
    // https://cplusplus.com/reference/unordered_set/unordered_set/insert/ - return values of insert

    // Add the value to graph's unordered_set of nodes
    pair<typename unordered_set<T>::iterator, bool> ret;
    ret = this->nodes.insert(value);

    // Increment the number of nodes in graph (if insertion was successful)
    if (ret.second)
        this->n_nodes++;
    
    // return an iterator to the inserted element (or the already existing one)
    return ret.first;
}

// Adds a directed edge (from->to). Updates outNeighbors(from) and inNeighbors(to)
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
        unordered_set<T>& nout = this->Nout[from];
        unordered_set<T>& nin = this->Nin[to];
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
    cout << "WARNING: Trying to remove non-existing edge.\n" << endl;
    return false;
}

// clears all neighbors for a specific node
template <typename T>
bool DirectedGraph<T>::clearNeighbors(const T& node){
    // Check if node exists before trying to access it
    if (!setIn(node,this->nodes)){
        cout << "ERROR: Node does not exist in unordered_set" << endl;
        return false;
    }

    // Node has outgoing neighbors
    if (mapKeyExists(node, this->Nout)){
        // For each outgoing neighbor, remove the edge
        unordered_set<T> noutCopy(this->Nout[node].begin(), this->Nout[node].end());
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

// distance function that checks if the distances are precomputed or else computes it and updates the precomputed distances
template <typename T>
const float DirectedGraph<T>::d(T t1,T t2){

    return this->dst(t1,t2);
    
    float dist;
    if (mapKeyExists(t1, this->_distances) && mapKeyExists(t2, this->_distances[t1])){
        dist = this->_distances[t1][t2];
    }
    else{
        float dist = this->dst(t1,t2);
        this->_distances[t1][t2] = dist;
        this->_distances[t2][t1] = dist;
    }
    return dist;
}


// Implementation of already declared Graph Template: VAMANA INDEXING DEPENDENCIES ------------------------- //


// ------------------------------------------------------------------------------------------------ MEDOID


template<typename T>
T DirectedGraph<T>::medoid(void){

    // empty unordered_set case
    if (this->nodes.empty()){ throw invalid_argument("Set is empty.\n"); }

    // if |s| = 1 or 2, return the first element of the unordered_set (metric distance is symmetric)
    if (this->nodes.size() <= 2){ return *(this->nodes.begin()); }

    T med;
    float dmin = numeric_limits<float>::max(), dsum, dist;

    for (const T& t : this->nodes){
        dsum = 0;

        for (const T& t_other : this->nodes){
            // we don't need to check if the other element is the same, because one's distance to itself is zero
            dsum += this->d(t,t_other);
        }

        // updating best medoid if current total distance is smaller than the minimum total distance yet
        if (dsum < dmin){
            dmin = dsum;
            med = t;
        }
    }
    return med;
}


template<typename T>
void DirectedGraph<T>::_thread_medoid_fn(int start_index, int end_index, T& local_minimum, float& local_dmin){

    local_dmin = numeric_limits<float>::max();
    float dsum, dist;

    for (int i = start_index; i < end_index; i++){
        dsum = 0;
        const T& node = this->_nodes[i];

        for (const T& other_node : this->_nodes)
            dsum += this->d(node, other_node);

        if (dsum < local_dmin){
            local_dmin = dsum;
            local_minimum = node;
        }
    }
}


template<typename T>
T DirectedGraph<T>::parallel_medoid(int n_threads){

    // empty unordered_set case
    if (this->nodes.empty()){ throw invalid_argument("Set is empty.\n"); }

    // if |s| = 1 or 2, return the first element of the unordered_set (metric distance is symmetric)
    if (this->nodes.size() <= 2){ return *(this->nodes.begin()); }

    // transforming unordered set into vector to allow random access and direct indexing.
    this->_nodes.assign(this->nodes.begin(), this->nodes.end());

    int chunk_size = (int) this->nodes.size() / n_threads;
    int remainder = this->nodes.size() - n_threads*chunk_size;

    // each thread will handle chunk_size nodes. Any remaining nodes will be split evenly among threads
    // for example 53 nodes among 5 threads => chunk_size = 10, remainder = 3. => 3 threads will do chunk_size + 1 while 2 threads will do chunk_size. 

    int start_index = 0, end_index;

    // initializing the threads
    vector<thread> threads(n_threads);                                  // a vector of size n_threads holding all the threads
    vector<T> local_minima(n_threads);                                  // a vector of size n_threads all initialized with default-constructed T
    vector<float> local_dmin(n_threads, numeric_limits<float>::max());  // a vector of size n_threads all initialized with float_max

    for (int i = 0; i < n_threads; i++){
        end_index = start_index + chunk_size + ((remainder-- > 0) ? 1 : 0); // if remaining exist, add + 1

        // load and launch thread 
        threads[i] = thread(&DirectedGraph::_thread_medoid_fn, this, start_index, end_index, ref(local_minima[i]), ref(local_dmin[i]));
        // ref( ) sends the vectors by reference to update their values.

        start_index = end_index;    // update index for next thread
    }

    // collecting all threads
    for (thread& th : threads){
        th.join();
    }

    // threads are joined. All dsums have been calculated in parallel and we have the local minimum of each.
    float dmin = *min_element(local_dmin.begin(), local_dmin.end());

    int min_index = getIndex(dmin, local_dmin);

    this->_nodes.clear();   // temporary use of _nodes vector for instant access for parallelization. We don't need it anymore.
    return local_minima[min_index];
}


// ------------------------------------------------------------------------------------------------ RGRAPH

// creates a random R graph with the existing nodes. Return TRUE if successful, FALSE otherwise
template <typename T>
bool DirectedGraph<T>::Rgraph(int R){

    if (R < 0) { throw invalid_argument("R must be a positive integer.\n"); }

    if (R > this->n_nodes - 1){ throw invalid_argument("R cannot exceed N-1 (N = the total number of nodes in the Graph).\n"); }

    if (R <= log(this->n_nodes)){ cout << "WARNING: R <= logn and therefore the graph will not be well connected.\n"; }
    
    if (R == 0){ cout << "WARNING: R is unordered_set to 0 and therefore all nodes in the graph are cleared.\n"; }
    
    // clear all edges in the graph to create an R random graph anew.
    if (!this->clearEdges())
        return false;

    cout << "Rgraph edges cleared" << endl;

    // unordered_set<vector<float>> nodesCopy(this->nodes.begin(), this->nodes.end());
    for (T n : this->nodes){
        
        // // Copy of nodes in the graph
        // unordered_set<T> remaining(this->nodes.begin(), this->nodes.end());
        // // Remove self from remaining nodes
        // if (remaining.erase(n) <= 0){
        //     cout << "ERROR: Failed to remove self from remaining nodes\n" << endl;
        //     return false;
        // }

        for (int i = 0; i < R; i++){
            
            T nr;

            do{
                nr = sampleFromSet(this->nodes);
            }while(!this->addEdge(n,nr)); // add the node as neighbor)
            
            // remaining.erase(nr);
        }
    }
    return true;
}

// ------------------------------------------------------------------------------------------------ GREEDY SEARCH

// Greedily searches the graph for the k nearest neighbors of query xq (in an area of size L), starting the search from the node s.
// Returns a unordered_set with the k closest neighbors (returned_vector[0]) and a unordered_set of all visited nodes (returned_vector[1]).
template <typename T>
const vector<unordered_set<T>> DirectedGraph<T>::greedySearch(const T& s, T xq, int k, int L) {

    // argument checks
    if (this->isEmpty(s)){ throw invalid_argument("No start node was provided.\n"); }

    if (!setIn(s, this->nodes)) { throw invalid_argument("Starting node not in nodeSet.\n"); }

    if (this->isEmpty(xq)){ throw invalid_argument("No query was provided.\n"); }

    if (k <= 0){ throw invalid_argument("K must be greater than 0.\n"); }

    if (L < k){ throw invalid_argument("L must be greater or equal to K.\n"); }


    // Create empty sets
    unordered_set<T> Lc,V;

    // Initialize Lc with s
    Lc.insert(s);
    
    unordered_set<T> diff;
    while(!(diff = setSubtraction(Lc,V)).empty()){
        T pmin = myArgMin(diff, xq, this->dst);

        // If node has outgoing neighbors
        if (mapKeyExists(pmin, this->Nout)){
            Lc.insert(this->Nout[pmin].begin(), this->Nout[pmin].end());
            // Lc = setUnion(Lc, this->Nout[pmin]);
        }
        V.insert(pmin);

        if (Lc.size() > L){
            Lc = closestN(L, Lc, xq, this->dst, this->isEmpty);    // function: find N closest points from a specific Xq from given unordered_set and return them
        }
    }

    vector<unordered_set<T>> ret;
    
    ret.insert(ret.begin(), closestN(k, Lc, xq, this->dst, this->isEmpty));
    ret.insert(ret.end(), V);

    return ret;
}

// ------------------------------------------------------------------------------------------------ ROBUST PRUNE

// Prunes out-neighbors of node p up until a minimum threshold R of out-neighbors for node p, based on distance criteria with parameter a.
template <typename T>
void DirectedGraph<T>::robustPrune(T p, unordered_set<T> V, float a, int R){

    // Argument Checks
    if (this->isEmpty(p)) { throw invalid_argument("No node was provided.\n"); }

    if (!setIn(p, this->nodes)) { throw invalid_argument("Node not in nodeSet\n"); };

    if (a < 1) { throw invalid_argument("Parameter a must be >= 1.\n"); }

    if (R <= 0) {throw invalid_argument("Parameter R must be > 0.\n"); }


    if (mapKeyExists(p, this->Nout))
        V.insert(this->Nout[p].begin(), this->Nout[p].end());// = setUnion(V, this->Nout[p]);
    
    V.erase(p);
    T p_opt;
    
    while (!V.empty()){
        p_opt = myArgMin(V, p, this->dst);
        
        this->Nout[p].insert(p_opt);

        if (this->Nout[p].size() == R)
            break;
        
        // n = p', p_opt = p*
        unordered_set<T> copyV(V.begin(), V.end());
        for (T n : copyV){
            if ( (a * this->d(p_opt, n)) <= this->d(p, n)){
                V.erase(n);
            }
        }
    }
}

// ------------------------------------------------------------------------------------------------ VAMANA GRAPH

// Transforms the graph into a Directed Graph such that it makes the finding of nearest neighbors easier.
// Parameters:
// + R the out-degree of each node in the graph (R >= 1)
// + L the area parameter for searching (L >= k >= 1, where k is the desired number of neighbors)
// + a the parameter for robust pruning (a >=1)
template <typename T>
bool DirectedGraph<T>::vamanaAlgorithm(int L, int R, float a){  // should "a" be added as a parameter?


    // check parameters if they are in legal range, for example R > 0 

    if (R <= 0){ throw invalid_argument("R must be a positive, non-zero integer.\n"); }

    if (L < 1) { throw invalid_argument("Parameter L must be >= 1.\n"); }

    if (a < 1) { throw invalid_argument("Parameter a must be >= 1.\n"); }
    

    if (this->Rgraph(R) == false)    // Initializing graph to a random R-Regular Directed Graph
        return false;
    
    // ERROR CHECK
    cout << "Graph randomized successfully with out-degree: " << R << endl;
    cout << "Searching for medoid node . . ." << endl;
    // T s = this->medoid();
    T s = this->parallel_medoid(8);

    cout << "Medoid node found" << endl;

    vector<T> perm = permutation(this->nodes);

    for (T si : perm){
        vector<unordered_set<T>> rv = greedySearch(s, si, 1, L);
        unordered_set<T> Lc = rv[0];
        unordered_set<T> V = rv[1];

        this->robustPrune(si, V, a, R);

        unordered_set<T> siNoutCopy;
        // Create a copy of s1 nout neighbors
        if (mapKeyExists(si, this->Nout)){
            siNoutCopy.insert(this->Nout[si].begin(), this->Nout[si].end());
        }

        for (T j : siNoutCopy){
            
            unordered_set<T> noutJsi;
            if (mapKeyExists(j, this->Nout)){   // if node j has no neighbors the unordered_set is the empty unordered_set U {σ(i)}
                noutJsi.insert(siNoutCopy.begin(), siNoutCopy.end());
            }
            
            noutJsi.insert(si);

            if (noutJsi.size() > R){
                this->robustPrune(j, noutJsi, a, R);
            }
            
            else{
                this->addEdge(j, si);
            }
        }
    }
    return true;
}

// MyArgMin and ClosestN functions should become methods to leverage the advantage of precomputed distances