#pragma once

#include <iostream>
#include <iterator>
#include <cstdlib>
#include <map>
#include <vector>
#include <set>
#include <cmath>
#include <functional>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <thread>
#include <list>

#include "util.hpp"

using namespace std;

// Directed Graph Class Template:
// This implementation of a Directed Graph Class makes use of dictionaries/maps for adjacency lists.
// To instantiate such a Directed Graph Object, you will need to specify the Content Type T, as well as provide:
// 1. Distance Function: T x T -> float => float distance_function(T,T)
// 2. (Optional) Content type T Valid Check Function: T -> bool => bool isEmpty(T) (Default is an AlwaysValid function that returns false for any input)
//
// IMPORTANT: If you are planning to use DirectedGraph::store and DirectedGraph::load methods, you must have the operators "<<" and ">>" overloaded FOR I/O OPERATIONS
// for your specific content type T.
// See more on: https://stackoverflow.com/questions/476272/how-can-i-properly-overload-the-operator-for-an-ostream
//              https://stackoverflow.com/questions/69803296/overloading-istream-operator
template <typename T>
class DirectedGraph{

    private:
    int n_edges;                            // number of edges present in the graph
    int n_nodes;                            // number of nodes present in the graph
    set<T> nodes;                           // a set containing all the nodes in the graph
    vector<T> _nodes;                       // vector representation of nodes used temporarily for medoid (if parallel approach is chosen)
    map<T, set<T>> Nout;                    // key: node, value: set of outgoing neighbors 
    function<float(const T&, const T&)> d;  // Graph's distance function
    function<bool(const T&)> isEmpty;       // typename T valid check

    
    public:

        // Constructor: Initialize an empty graph
        DirectedGraph(function<float(const T&, const T&)> distance_function, function<bool(const T&)> is_Empty = alwaysValid<T>, vector<T> values = {}) {
            this->n_edges = 0;
            this->n_nodes = values.size();
            this->d = distance_function;
            this->isEmpty = is_Empty;
            cout << "Graph created!" << endl;

            for (const T& value : values){
                this->createNode(value);
            }
        }

        // Return a set of all Nodes in the graph
        const set<T>& getNodes() const { return this->nodes; }

        // Return the number of edges in the graph
        const int& get_n_edges() const { return this->n_edges; }

        // Return the number of nodes in the graph
        const int& get_n_nodes() const { return this->n_nodes; }

        // Return Nout map
        const map<T, set<T>>& get_Nout() const { return this->Nout; }

        // Creates a node, adds it in the graph and returns it
        typename set<T>::iterator createNode(const T& value);

        // Adds an directed edge (from->to). Updates outNeighbors(from) and inNeighbors(to)
        bool addEdge(const T& from, const T& to);

        // Remove edge
        bool removeEdge(const T& from, const T& to);

        // Clears all neighbors for a specific node
        bool clearNeighbors(const T& node);

        // Clears all edges in the graph
        bool clearEdges(void);

        // Calculates the medoid of the nodes in the graph based on the given distance function
        const T medoid(void);

        // implements medoid function using serial programming.
        const T _serial_medoid(void);

        // Implements medoid function using parallel programming with threads. Concurrency is set by the global constant N_THREADS.
        const T _parallel_medoid(void);

        // Thread function for parallel medoid. Work inside the range defined by [start_index, end_index). Update minima by reference for the merging of the results.
        void _thread_medoid_fn(int start_index, int end_index, T& local_minimum, float& local_dmin);

        // creates a random R graph with the existing nodes. Return TRUE if successful, FALSE otherwise
        bool Rgraph(int R);

        // Greedily searches the graph for the k nearest neighbors of query xq (in an area of size L), starting the search from the node s.
        // Returns a set with the k closest neighbors (returned_vector[0]) and a set of all visited nodes (returned_vector[1]).
        const vector<set<T>> greedySearch(const T& s, T xq, int k, int L);

        // Prunes out-neighbors of node p up until a minimum threshold R of out-neighbors for node p, based on distance criteria with parameter a.
        void robustPrune(const T& p, set<T> V, float a, int R);

        // Transforms the graph into a Directed Graph such that it makes the finding of nearest neighbors easier.
        // Parameters:
        // + R the out-degree of each node in the graph (R >= 1)
        // + L the area parameter for searching (L >= k >= 1, where k is the desired number of neighbors)
        // + a the parameter for robust pruning (a >=1)
        bool vamanaAlgorithm(int L, int R, float a);


        // Stores the current state of a graph into the specified file.
        // IMPORTANT: makes use of overloaded << operator to store the graph into a file.
        // Make sure SHOULD_OMIT flag in config.hpp file is set to 0
        void store(const string& filename) const;

        // Loads a graph state from the specified file. A Graph instance must already be instantiated with the appropriate distance and isEmpty functions.
        // IMPORTANT: makes use of overloaded >> operator to load the graph from a file
        void load(const string& filename);
};

// Implementation of already declared Graph Template: MAIN FUNCTIONALITY ----------------------------------- //

// Creates a node, adds it in the graph and returns it
template<typename T>
typename set<T>::iterator DirectedGraph<T>::createNode(const T& value){
    // https://cplusplus.com/reference/set/set/insert/ - return values of insert

    // Add the value to graph's set of nodes
    pair<typename set<T>::iterator, bool> ret;
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
    OMIT_OUTPUT;
    // At least one of the nodes is not present in nodeSet
    if (!setIn(from, this->nodes) || !setIn(to, this->nodes)){
        cout << "Node is not present in nodeSet" << endl;
        return false;
    }

    // No self-loops are allowed
    if (from == to){
        cout << "Cannot add edge from self to self" << endl;
        return false;
    }

    int previous_size = this->Nout[from].size(); 
    this->Nout[from].insert(to);
    int next_size = this->Nout[from].size(); 

    if(next_size == previous_size){
        cout << "Cannot add edge. Edge already exists" << endl;
        return false;
    }
    this->n_edges++;
    return true;
}

// remove edge
template <typename T>
bool DirectedGraph<T>::removeEdge(const T& from, const T& to){

    // Check if keys exist before accessing them (and removing them)
    if (mapKeyExists(from, this->Nout)) {
        // Key exists, access the value, if successfully removed, return true
        if(this->Nout[from].erase(to)){
            // Check if outgoing neighbors are empty, if so, remove entry from unordered map
            if(this->Nout[from].empty()){
                this->Nout.erase(from);
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
bool DirectedGraph<T>::clearEdges(void){
    for (T n : this->nodes){
        if (!this->clearNeighbors(n)){
            cout << "ERROR: Failed to clear neighbors for node" << endl;
            return false;
        }
    }

    if (this->n_edges != 0 || !this->Nout.empty()){
        cout << "ERROR: Failed to clear edges in graph" << endl;
        return false;
    }
    
    return true;
}


// Implementation of already declared Graph Template: Vamana Indexing Dependencies ------------------------- //


// ------------------------------------------------------------------------------------------------ MEDOID

// Calculates the medoid of the nodes in the graph based on the given distance function
template<typename T>
const T DirectedGraph<T>::medoid(void){

    // empty set case
    if (this->nodes.empty()){ throw invalid_argument("Set is empty.\n"); }

    // if |s| = 1 or 2, return the first element of the set (metric distance is symmetric)
    if (this->nodes.size() <= 2){ return *(this->nodes.begin()); }

    // Serial Case
    if (N_THREADS == 1) return this->_serial_medoid();

    // Parallel Case
    if (N_THREADS > 1)  return this->_parallel_medoid();

    // Invalid N_THREADS
    throw invalid_argument("N_THREADS constant is invalid. Value must be N_THREADS >= 1.\n");
}

// Implements medoid function using serial programming.
template<typename T>
const T DirectedGraph<T>::_serial_medoid(void){

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

// Thread function for parallel medoid. Work inside the range defined by [start_index, end_index). Update minima by reference for the merging of the results.
template<typename T>
void DirectedGraph<T>::_thread_medoid_fn(int start_index, int end_index, T& local_minimum, float& local_dmin){

    // There is no need for synchronization between threads, as the shared resources (this->_nodes) is accessed in a read-only manner.

    // local_dmin is already initialized from the thread_caller function: T DirectedGraph<T>::_parallel_medoid(void)
    float dsum, dist;

    for (int i = start_index; i < end_index; i++){
        dsum = 0;
        const T& node = this->_nodes[i];    // makes use of temporary _nodes field (vectorized copy of the nodeset for instant access)

        // we don't need to check if the other element is the same, because one's distance to itself is zero
        for (const T& other_node : this->_nodes)
            dsum += this->d(node, other_node);

        // updating best medoid if current total distance is smaller than the minimum total distance yet in the working range
        if (dsum < local_dmin){
            local_dmin = dsum;
            local_minimum = node;
        }
    }
}

// Implements medoid function using parallel programming with threads. Concurrency is set by the global constant N_THREADS.
template<typename T>
const T DirectedGraph<T>::_parallel_medoid(void){

    // transforming unordered set into vector to allow random access and direct indexing.
    this->_nodes.assign(this->nodes.begin(), this->nodes.end());

    int chunk_size = (int) this->nodes.size() / N_THREADS;          // how many nodes each thread will handle
    int remainder = this->nodes.size() - N_THREADS*chunk_size;      // amount of remaining nodes to be distributed evenly among threads

    // each thread will handle chunk_size nodes. Any remaining nodes will be distributed evenly among threads
    // for example 53 nodes among 5 threads => chunk_size = 10, remainder = 3.
    // This means that 3 threads will be responsible for chunk_size + 1 nodes while the other 2 threads will be responsible for chunk_size nodes. 

    int start_index = 0, end_index;

    // initializing the threads
    vector<thread> threads(N_THREADS);                                  // a vector of size N_THREADS holding all the threads
    vector<T> local_minima(N_THREADS);                                  // a vector of size N_THREADS all initialized with default-constructed T
    vector<float> local_dmin(N_THREADS, numeric_limits<float>::max());  // a vector of size N_THREADS all initialized with float_max

    for (int i = 0; i < N_THREADS; i++){
        end_index = start_index + chunk_size + ((remainder-- > 0) ? 1 : 0); // if any remaining left, add + 1 and decrement the remainder

        // load and launch thread 
        threads[i] = thread(&DirectedGraph::_thread_medoid_fn, this, start_index, end_index, ref(local_minima[i]), ref(local_dmin[i]));
        // ref( ) sends the vectors by reference to update their values.

        start_index = end_index;    // update index for next thread
    }

    for (thread& th : threads){ th.join(); }    // collecting all threads
    
    // threads are joined.
    this->_nodes.clear();   // temporary use of _nodes vector for instant access for parallelization. We don't need it anymore.

    // All dsums have been calculated in parallel and we have the local minimum of each range/thread.

    // find the minimum distance among the vector of minimums returned by the threads
    float dmin = *min_element(local_dmin.begin(), local_dmin.end());
    // find the index of the minimum element
    int min_index = getIndex(dmin, local_dmin);

    // return the corresponding total minimum element of type T from the parallel vector.
    return local_minima[min_index];
}


// ------------------------------------------------------------------------------------------------ RGRAPH

// creates a random R graph with the existing nodes. Return TRUE if successful, FALSE otherwise
template <typename T>
bool DirectedGraph<T>::Rgraph(int R){

    if (R < 0) { throw invalid_argument("R must be a positive integer.\n"); }

    if (R > this->n_nodes - 1){ throw invalid_argument("R cannot exceed N-1 (N = the total number of nodes in the Graph).\n"); }

    if (R <= log(this->n_nodes)){ cout << "WARNING: R <= logn and therefore the graph will not be well connected.\n"; }
    
    if (R == 0){ cout << "WARNING: R is set to 0 and therefore all nodes in the graph are cleared.\n"; }
    
    // clear all edges in the graph to create an R random graph anew.
    if (!this->clearEdges())
        return false;

    cout << "Rgraph edges cleared" << endl;

    for (T n : this->nodes){            // for each node
        for (int i = 0; i < R; i++){    // repeat R times: sample from set and add until valid
            T nr;
            do{
                nr = sampleFromSet(this->nodes);
            }while(!this->addEdge(n,nr)); // addEdge fails when: 1. n == nr, 2. edge(n,nr) already exists
        }
    }
    return true;
}

// ------------------------------------------------------------------------------------------------ GREEDY SEARCH

// Greedily searches the graph for the k nearest neighbors of query xq (in an area of size L), starting the search from the node s.
// Returns a set with the k closest neighbors (returned_vector[0]) and a set of all visited nodes (returned_vector[1]).
template <typename T>
const vector<set<T>> DirectedGraph<T>::greedySearch(const T& s, T xq, int k, int L) {

    // argument checks
    if (this->isEmpty(s)){ throw invalid_argument("No start node was provided.\n"); }

    if (!setIn(s, this->nodes)) { throw invalid_argument("Starting node not in nodeSet.\n"); }

    if (this->isEmpty(xq)){ throw invalid_argument("No query was provided.\n"); }

    if (k <= 0){ throw invalid_argument("K must be greater than 0.\n"); }

    if (L < k){ throw invalid_argument("L must be greater or equal to K.\n"); }

    // Create empty sets
    set<T> Lc = {s}, V, diff; // Initialize Lc with s
    
    while(!(diff = setSubtraction(Lc,V)).empty()){
        T pmin = myArgMin(diff, xq, this->d, this->isEmpty);    // pmin is the node with the minimum distance from query xq

        // If node has outgoing neighbors
        if (mapKeyExists(pmin, this->Nout)){
            Lc.insert(this->Nout[pmin].begin(), this->Nout[pmin].end());
        }
        V.insert(pmin);

        if (Lc.size() > L){
            Lc = closestN(L, Lc, xq, this->d, this->isEmpty);    // function: find N closest points from a specific xq from given set and return them
        }
    }

    vector<set<T>> ret;
    
    ret.insert(ret.begin(), closestN(k, Lc, xq, this->d, this->isEmpty));
    ret.insert(ret.end(), V);

    return ret;
}

// ------------------------------------------------------------------------------------------------ ROBUST PRUNE

// Prunes out-neighbors of node p up until a minimum threshold R of out-neighbors for node p, based on distance criteria with parameter a.
template <typename T>
void DirectedGraph<T>::robustPrune(const T& p, set<T> V, float a, int R){

    // Argument Checks
    if (this->isEmpty(p)) { throw invalid_argument("No node was provided.\n"); }

    if (!setIn(p, this->nodes)) { throw invalid_argument("Node not in nodeSet\n"); };

    if (a < 1) { throw invalid_argument("Parameter a must be >= 1.\n"); }

    if (R <= 0) {throw invalid_argument("Parameter R must be > 0.\n"); }


    if (mapKeyExists(p, this->Nout))
        V.insert(this->Nout[p].begin(), this->Nout[p].end());
    V.erase(p);

    this->clearNeighbors(p);    // calls remove edge

    T p_opt;
    
    while (!V.empty()){
        p_opt = myArgMin(V, p, this->d, this->isEmpty);
        
        this->addEdge(p, p_opt);

        if (this->Nout[p].size() == R)
            break;
        
        // *it = n = p', p_opt = p*
        for (auto it = V.begin(); it != V.end(); /*no increment here*/){    // safe set iteration with mutable set during the iteration
            if ( (a * this->d(p_opt, *it)) <= this->d(p, *it)){ 
                it = V.erase(it);
            }
            else { it++; }  // incrementing here because V.erase() returns the next iterator on successful deletion
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
bool DirectedGraph<T>::vamanaAlgorithm(int L, int R, float a){
    // check parameters if they are in legal range, for example R > 0 

    if (R <= 0){ throw invalid_argument("R must be a positive, non-zero integer.\n"); }

    if (L < 1) { throw invalid_argument("Parameter L must be >= 1.\n"); }

    if (a < 1) { throw invalid_argument("Parameter a must be >= 1.\n"); }
    

    cout << "Initializing a random R-Regular Directed Graph with out-degree R = " << R << ". . ." << endl;
    if (this->Rgraph(R) == false)
        return false;
    cout << "Graph initialized successfully!" << endl;


    cout << "Searching for medoid node . . ." << endl;
    T s = this->medoid();
    cout << "Medoid node found successfully!" << endl;

    cout << "Finalizing Vamana Index using the Vamana Algorithm . . ." << endl;
    vector<T> perm = permutation(this->nodes);

    for (const T& si : perm){
        vector<set<T>> rv = greedySearch(s, si, 1, L);
        set<T> Lc = rv[0];
        set<T> V = rv[1];
  
        this->robustPrune(si, V, a, R);
        
        if (mapKeyExists(si, this->Nout)){
        
            for (const T& j : this->Nout[si]){  // for every neighbor j of si

                this->addEdge(j, si);   // does it in either case (simpler code, robust prune clears all neighbors after copying to candidate set V anyway)
                if (this->Nout[j].size() > R)
                    robustPrune(j, this->Nout[j], a, R);
            }
        }
    }
    return true;
}


// Stores the current state of a graph into the specified file.
// IMPORTANT: makes use of overloaded << operator to store the graph into a file.
// Make sure SHOULD_OMIT flag in config.hpp file is set to 0
template<typename T>
void DirectedGraph<T>::store(const string& filename) const{

    fstream file;

    // create a new file if it did not exist, or replace any contents existing before
    file.open(filename, ios::out | ios::trunc);  // mode flags: https://cplusplus.com/reference/fstream/fstream/open/

    file << this->n_edges;
    file << '\n';
    file << this->n_nodes;
    file << '\n';
    file << this->nodes;
    file << '\n';

    // Map each element to its corresponding index
    int index = 0;
    map<T, int> elemIndexMap;
    for (const T& elem: this->nodes){
        elemIndexMap[elem] = index++; 
    }

    // Create output map
    map<int, set<int>> outNout;

    for (auto& pair : this->Nout){

        // Get element's index
        index = elemIndexMap[pair.first];

        // Out neighbours set
        for (const T& neighbor : pair.second){
            outNout[index].insert(elemIndexMap[neighbor]);
        }
    }

    file << outNout;

    file.close();
}

// Loads a graph state from the specified file. A Graph instance must already be instantiated with the appropriate distance and isEmpty functions.
// IMPORTANT: makes use of overloaded >> operator to load the graph from a file
template<typename T>
void DirectedGraph<T>::load(const string& filename){

    fstream file;

    file.open(filename, ios::in);

    file >> this->n_edges;
    file.ignore(1);
    file >> this->n_nodes;
    file.ignore(1);
    file >> this->nodes;
    file.ignore(1);

    // Map each index to its corresponding element
    int index = 0;
    map<int, T> indexElemMap;
    for (const T& elem: this->nodes){
        indexElemMap[index++] = elem;
    }



    map <int, set<int>> inNout;

    file >> inNout;

    T elem;
    // Translate int key to T
    for (auto& pair : inNout){
        elem = indexElemMap[pair.first];

        // Translate int neighbors to T
        for (int neighbor: pair.second){
            this->Nout[elem].insert(indexElemMap[neighbor]);
        }
    }

    file.close();
}