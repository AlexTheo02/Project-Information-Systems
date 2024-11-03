#pragma once

#include "types.hpp"

// This file implements member functions of the DirectedGraph class template declared in the types.hpp header file.

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
    
    // At least one of the nodes is not present in nodeSet
    if (!setIn(from, this->nodes) || !setIn(to, this->nodes)){
        c_log << "Node is not present in nodeSet" << '\n';
        return false;
    }

    // No self-loops are allowed
    if (from == to){
        c_log << "Cannot add edge from self to self" << '\n';
        return false;
    }

    int previous_size = this->Nout[from].size(); 
    this->Nout[from].insert(to);
    int next_size = this->Nout[from].size(); 

    if(next_size == previous_size){
        c_log << "Cannot add edge. Edge already exists" << '\n';
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
    c_log << "WARNING: Trying to remove non-existing edge.\n" << '\n';
    return false;
}

// clears all neighbors for a specific node
template <typename T>
bool DirectedGraph<T>::clearNeighbors(const T& node){
    // Check if node exists before trying to access it
    if (!setIn(node,this->nodes)){
        c_log << "ERROR: Node does not exist in set" << '\n';
        return false;
    }

    // Node has outgoing neighbors
    if (mapKeyExists(node, this->Nout)){
        // For each outgoing neighbor, remove the edge
        set<T> noutCopy(this->Nout[node].begin(), this->Nout[node].end());
        for (const T& n : noutCopy){      
            if (!this->removeEdge(node,n)){
                c_log << "ERROR: Failed to remove edge, something went wrong" << '\n';
                return false;
            }          
        }      
        // Check if node has been removed from neighbors map
        if (mapKeyExists(node, this->Nout)){
            c_log << "ERROR: Something went wrong when clearing neighbors" << '\n';
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
            c_log << "ERROR: Failed to clear neighbors for node" << '\n';
            return false;
        }
    }

    if (this->n_edges != 0 || !this->Nout.empty()){
        c_log << "ERROR: Failed to clear edges in graph" << '\n';
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

    // Invalid N_THREADS
    if (N_THREADS <= 0) throw invalid_argument("N_THREADS constant is invalid. Value must be N_THREADS >= 1.\n");

    // avoid recalculation:
    if (this->_medoid.empty())
        this->_medoid = (N_THREADS == 1) ? this->_serial_medoid() : this->_parallel_medoid();

    // return saved or new medoid
    return this->_medoid;
    
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

    if (R <= log(this->n_nodes)){ c_log << "WARNING: R <= logn and therefore the graph will not be well connected.\n"; }
    
    if (R == 0){ c_log << "WARNING: R is set to 0 and therefore all nodes in the graph are cleared.\n"; }
    
    // clear all edges in the graph to create an R random graph anew.
    if (!this->clearEdges())
        return false;

    c_log << "Rgraph edges cleared" << '\n';

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
    

    c_log << "Initializing a random R-Regular Directed Graph with out-degree R = " << R << ". . ." << '\n';
    if (this->Rgraph(R) == false)
        return false;
    c_log << "Graph initialized successfully!" << '\n';


    c_log << "Searching for medoid node . . ." << '\n';
    T s = this->medoid();
    c_log << "Medoid node found successfully!" << '\n';

    c_log << "Finalizing Vamana Index using the Vamana Algorithm . . ." << '\n';
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

    c_log << "Graph Instance stored successfully in \"" << filename << '\"' << '\n';
}

// Loads a graph state from the specified file. A Graph instance must already be instantiated with the appropriate distance and isEmpty functions.
// IMPORTANT: makes use of overloaded >> operator to load the graph from a file
template<typename T>
void DirectedGraph<T>::load(const string& filename){

    fstream file;

    file.open(filename, ios::in);

    file >> this->n_edges;
    file.ignore(1);         // ignores \n
    file >> this->n_nodes;
    file.ignore(1);         // ignores \n
    file >> this->nodes;
    file.ignore(1);         // ignores \n

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
    c_log << "Graph Instance loaded successfully from \"" << filename << '\"' << '\n';
}