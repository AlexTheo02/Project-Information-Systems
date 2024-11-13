#pragma once

#include "types.hpp"

// This file implements member functions of the DirectedGraph class template declared in the types.hpp header file.

// Creates a node, adds it in the graph and returns it
template<typename T>
Id DirectedGraph<T>::createNode(const T& value, int category){
    // https://cplusplus.com/reference/set/set/insert/ - return values of insert

    Node<T> node(this->n_nodes, category, value, this->isEmpty);

    // Add the value to graph's set of nodes
    this->nodes.push_back(node);

    // if is valid category, add node belonging to it to corresponding map entry
    if (category >= 0) this->categories[category].insert(node.id);

    // Increment the number of nodes in graph (if insertion was successful)
    this->n_nodes++;
    
    // return the node's id
    return node.id;
}

// Adds a directed edge (from->to). Updates outNeighbors(from) and inNeighbors(to)
template <typename T>
bool DirectedGraph<T>::addEdge(const Id from, const Id to){
    
    // At least one of the nodes is not present in nodeSet
    if (from >= this->n_nodes || to >= this->n_nodes){
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
bool DirectedGraph<T>::removeEdge(const Id from, const Id to){

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
bool DirectedGraph<T>::clearNeighbors(const Id id){
    // Check if node exists before trying to access it
    if (id >= this->n_nodes){
        c_log << "ERROR: Node does not exist in the graph" << '\n';
        return false;
    }

    // Node has outgoing neighbors
    if (mapKeyExists(id, this->Nout)){
        // For each outgoing neighbor, remove the edge
        unordered_set<Id> noutCopy(this->Nout[id].begin(), this->Nout[id].end());

        for (const Id n : noutCopy){      
            if (!this->removeEdge(id,n)){
                c_log << "ERROR: Failed to remove edge, something went wrong" << '\n';
                return false;
            }          
        }      
        // Check if node has been removed from neighbors map
        if (mapKeyExists(id, this->Nout)){
            
            c_log << "ERROR: Something went wrong when clearing neighbors" << '\n';
            return false;
        }
    }
    return true;
}

// clears all edges in the graph
template <typename T>
bool DirectedGraph<T>::clearEdges(void){
    for (Node<T> node : this->nodes){
        if (!this->clearNeighbors(node.id)){
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
const Id DirectedGraph<T>::medoid(optional<vector<Node<T>>> nodes_arg, optional<bool> update_stored){
    c_log << "Medoid\n";

    // unrwapping from the "optional" template with appropriate values
    vector<Node<T>>& nodes = (nodes_arg == nullopt) ? this->nodes : nodes_arg.value();
    bool to_store;

    if (update_stored != nullopt)
        to_store = update_stored.value();
    else
        to_store = (nodes_arg == nullopt) ? true : false;
    

    // empty set case
    if (nodes.empty()){ throw invalid_argument("Vector is empty.\n"); }

    // if |s| = 1 or 2, return the first element of the set (metric distance is symmetric)
    if (nodes.size() <= 2){ return Id{0}; }

    // Invalid args.n_threads
    if (args.n_threads <= 0) throw invalid_argument("args.n_threads constant is invalid. Value must be args.n_threads >= 1.\n");

    // avoid recalculation: (if nodes argument is the this->nodes vector)
    if (nodes == this->nodes && this->_medoid != -1){
        c_log << "Medoid already exists, returning.\n";
        return this->_medoid;
    }

    // store if asked to (or if default state)
    if (to_store){
        this->_medoid = (args.n_threads == 1) ? this->_serial_medoid(nodes) : this->_parallel_medoid(nodes);
        return this->_medoid;
    }
    
    // else calculate and return it
    return (args.n_threads == 1) ? this->_serial_medoid(nodes) : this->_parallel_medoid(nodes);
    
}

// Implements medoid function using serial programming.
template<typename T>
const Id DirectedGraph<T>::_serial_medoid(vector<Node<T>>& nodes){

    Id med;
    float dmin = numeric_limits<float>::max(), dsum, dist;

    for (const Node<T>& node : nodes){
        dsum = 0;

        for (const Node<T>& other_node : nodes){
            // we don't need to check if the other element is the same, because one's distance to itself is zero
            dsum += this->d(node.value, other_node.value);
        }

        // updating best medoid if current total distance is smaller than the minimum total distance yet
        if (dsum < dmin){
            dmin = dsum;
            med = node.id;
        }
    }
    return med;
}

// Thread function for parallel medoid. Work inside the range defined by [start_index, end_index). Update minima by reference for the merging of the results.
template<typename T>
void DirectedGraph<T>::_thread_medoid_fn(vector<Node<T>>& nodes, int start_index, int end_index, Id& local_minimum, float& local_dmin){

    // There is no need for synchronization between threads, as the shared resources (nodes) is accessed in a read-only manner.

    // local_dmin is already initialized from the thread_caller function: T DirectedGraph<T>::_parallel_medoid(void)
    float dsum, dist;

    for (int i = start_index; i < end_index; i++){
        dsum = 0;
        const Node<T>& node = nodes[i];
        // we don't need to check if the other element is the same, because one's distance to itself is zero
        for (const Node<T>& other_node : nodes)
            dsum += this->d(node.value, other_node.value);

        // updating best medoid if current total distance is smaller than the minimum total distance yet in the working range
        if (dsum < local_dmin){
            local_dmin = dsum;
            local_minimum = node.id;
        }
    }
}

// Implements medoid function using parallel programming with threads. Concurrency is set by the global constant args.n_threads.
template<typename T>
const Id DirectedGraph<T>::_parallel_medoid(vector<Node<T>>& nodes){

    int chunk_size = nodes.size() / args.n_threads;          // how many nodes each thread will handle
    int remainder = nodes.size() - args.n_threads*chunk_size;      // amount of remaining nodes to be distributed evenly among threads

    // each thread will handle chunk_size nodes. Any remaining nodes will be distributed evenly among threads
    // for example 53 nodes among 5 threads => chunk_size = 10, remainder = 3.
    // This means that 3 threads will be responsible for chunk_size + 1 nodes while the other 2 threads will be responsible for chunk_size nodes. 

    int start_index = 0, end_index;

    // initializing the threads
    vector<thread> threads(args.n_threads);                                  // a vector of size args.n_threads holding all the threads
    vector<Id> local_minima(args.n_threads);                                 // a vector of size args.n_threads to hold ids of local medoids
    vector<float> local_dmin(args.n_threads, numeric_limits<float>::max());  // a vector of size args.n_threads all initialized with float_max

    for (int i = 0; i < args.n_threads; i++){
        end_index = start_index + chunk_size + ((remainder-- > 0) ? 1 : 0); // if any remaining left, add + 1 and decrement the remainder

        // load and launch thread 
        threads[i] = thread(&DirectedGraph::_thread_medoid_fn, this, ref(nodes), start_index, end_index, ref(local_minima[i]), ref(local_dmin[i]));
        // ref( ) sends the vectors by reference to update their values.

        start_index = end_index;    // update index for next thread
    }

    for (thread& th : threads){ th.join(); }    // collecting all threads
    
    // threads are joined.

    // All dsums have been calculated in parallel and we have the local minimum of each range/thread.

    // find the minimum distance among the vector of minimums returned by the threads
    float dmin = *min_element(local_dmin.begin(), local_dmin.end());
    // find the index of the minimum element
    int min_index = getIndex(dmin, local_dmin);

    // return the corresponding total minimum element of type T from the parallel vector.
    return local_minima[min_index];
}

// Returns the node from given nodeSet with the minimum distance from a specific point in the nodespace (node is allowed to not exist in the graph)
template<typename T>
Id DirectedGraph<T>::_myArgMin(const unordered_set<Id>& nodeSet, T t){

    if (nodeSet.empty()) { throw invalid_argument("Set is Empty.\n"); }

    if (isEmpty(t)) { throw invalid_argument("Query container is empty.\n"); }

    if (nodeSet.size() == 1) { return *nodeSet.begin(); }

    float minDist = numeric_limits<float>::max(), dist;
    Id minId;

    for (const Id id : nodeSet){
        dist = this->d(this->nodes[id].value, t);

       
        if (dist <= minDist){    // New minimum distance found
            minId = id;
            minDist = dist;
        }
    }
    return minId;
}

// Retains the N closest elements of S to X based on distance d
template<typename T>
unordered_set<Id> DirectedGraph<T>::_closestN(int N, const unordered_set<Id>& S, T X){

    // check if the set is empty
    if (S.empty()){
        c_log << "WARNING: Set is empty. There exist no neighbors inside the given set.\n";
        return S;
    }

    // check if the vector is empty
    if (isEmpty(X)) { throw invalid_argument("Query X is empty.\n"); }

    // check if N is a valid number (size of set > N > 0)
    if (N < 0){ throw invalid_argument("N must be greater than 0.\n"); } 

    // if N is equal to 0 return the empty set
    if (N == 0){
        c_log << "WARNING: N is 0. Returning the empty set.\n";
        unordered_set<Id> nullset;
        return nullset;
    }
    
    // if N is greater than the set size, return the whole set
    if(S.size() <= N)
        return S;

    // transform the set to a vector for partitioning around a pivot
    vector<Id> Svec(S.begin(), S.end());

    // partition the vector based on the distance from point X up around the N-th element
    nth_element(Svec.begin(), Svec.begin() + N, Svec.end(),
                [X,this] (int id1, int id2) {return (this->d(X, this->nodes[id1].value) < d(X, this->nodes[id2].value));});
                // lambda(id1,id2) = determines which of the two points corresponding to id1 and id2 is closest to X given metric distance d.

    // the vector after the use of nth_element has the following properties:
    // the N-th element is in the position that it would've been if the vector was sorted in its entirety
    // elements before the N-th element are < than the N-th element, and elements after the N-th element are > than the N-th element. (< and > defined by compare function)
    // the elements in the left and right subvectors are not themselves sorted, but for this task we don't need them sorted. 
    // https://en.cppreference.com/w/cpp/algorithm/nth_element

    // keep N first
    unordered_set<Id> closest_nodes(Svec.begin(), Svec.begin() + N);

    return closest_nodes;
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

    for (Node<T>& n : this->nodes){            // for each node
        for (int i = 0; i < R; i++){    // repeat R times: sample from set and add until valid
            Node<T> nr;
            do{
                nr = sampleFromContainer(this->nodes);
            }while(!this->addEdge(n.id,nr.id)); // addEdge fails when: 1. n == nr, 2. edge(n,nr) already exists
        }
    }
    return true;
}

// ------------------------------------------------------------------------------------------------ GREEDY SEARCH

// Greedily searches the graph for the k nearest neighbors of query xq (in an area of size L), starting the search from the node s.
// Returns a set with the k closest neighbors (returned_vector[0]) and a set of all visited nodes (returned_vector[1]).
template <typename T>
const pair<unordered_set<Id>, unordered_set<Id>> DirectedGraph<T>::greedySearch(Id s, T xq, int k, int L) {

    c_log << "Greedy Search\n";

    // argument checks
    if (s < 0 || s >= this->n_nodes){ throw invalid_argument("Invalid Index was provided.\n"); }

    if (this->nodes[s].empty()){ throw invalid_argument("No start node was provided.\n"); }

    if (this->isEmpty(xq)){ throw invalid_argument("No query was provided.\n"); }

    if (k <= 0){ throw invalid_argument("K must be greater than 0.\n"); }

    if (L < k){ throw invalid_argument("L must be greater or equal to K.\n"); }

    // Create empty sets
    unordered_set<Id> Lc = {s}, V, diff; // Initialize Lc with s
    
    while(!(diff = setSubtraction(Lc,V)).empty()){
        Id pmin = this->_myArgMin(diff, xq);    // pmin is the node with the minimum distance from query xq

        // If node has outgoing neighbors
        if (mapKeyExists(pmin, this->Nout)){
            Lc.insert(this->Nout[pmin].begin(), this->Nout[pmin].end());
        }
        V.insert(pmin);

        if (Lc.size() > L){
            Lc = _closestN(L, Lc, xq);    // function: find N closest points from a specific xq from given set and return them
        }
    }

    pair<unordered_set<Id>, unordered_set<Id>> ret;
    
    ret.first = _closestN(k, Lc, xq);
    ret.second = V;

    return ret;
}

// ------------------------------------------------------------------------------------------------ ROBUST PRUNE

// Prunes out-neighbors of node p up until a minimum threshold R of out-neighbors for node p, based on distance criteria with parameter a.
template <typename T>
void DirectedGraph<T>::robustPrune(Id p, unordered_set<Id> V, float a, int R){

    // argument checks
    if (p < 0 || p >= this->n_nodes){ throw invalid_argument("Invalid Index was provided.\n"); }

    if (this->nodes[p].empty()) { throw invalid_argument("No node was provided.\n"); }

    if (a < 1) { throw invalid_argument("Parameter a must be >= 1.\n"); }

    if (R <= 0) {throw invalid_argument("Parameter R must be > 0.\n"); }


    if (mapKeyExists(p, this->Nout))
        V.insert(this->Nout[p].begin(), this->Nout[p].end());
    V.erase(p);

    this->clearNeighbors(p);    // calls remove edge

    Id p_opt;
    
    while (!V.empty()){
        p_opt = this->_myArgMin(V, this->nodes[p].value);
        
        this->addEdge(p, p_opt);

        if (this->Nout[p].size() == R)
            break;
        
        // *it = n = p', p_opt = p*
        for (auto it = V.begin(); it != V.end(); /*no increment here*/){    // safe set iteration with mutable set during the iteration
            if ( (a * this->d(this->nodes[p_opt].value, this->nodes[*it].value)) <= this->d(this->nodes[p].value, this->nodes[*it].value)){ 
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
    // check parameters if they are in legal range
    if (this->nodes.size() == 1) return true;   // no edges possible

    if (R <= 0){ throw invalid_argument("R must be a positive, non-zero integer.\n"); }

    if (L < 1) { throw invalid_argument("Parameter L must be >= 1.\n"); }

    if (a < 1) { throw invalid_argument("Parameter a must be >= 1.\n"); }

    

    c_log << "Initializing a random R-Regular Directed Graph with out-degree R = " << R << ". . ." << '\n';
    if (this->Rgraph(R) == false)
        return false;
    c_log << "Graph initialized successfully!" << '\n';


    c_log << "Searching for medoid node . . ." << '\n';
    Id s = this->medoid();
    c_log << "Medoid node found successfully!" << '\n';

    c_log << "Finalizing Vamana Index using the Vamana Algorithm . . ." << '\n';
    vector<Node<T>> perm = permutation(this->nodes);

    for (Node<T>& si : perm){
        pair<unordered_set<Id>, unordered_set<Id>> rv = greedySearch(this->nodes[s].id, si.value, 1, L); 

        unordered_set<Id> Lc = rv.first;
        unordered_set<Id> V = rv.second;

        this->robustPrune(si.id, V, a, R);
        
        if (mapKeyExists(si.id, this->Nout)){
        
            for (const Id j : this->Nout[si.id]){  // for every neighbor j of si

                this->addEdge(j, si.id);   // does it in either case (simpler code, robust prune clears all neighbors after copying to candidate set V anyway)
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
    if (filename == ""){ return; }
    fstream file;

    // create a new file if it did not exist, or replace any contents existing before
    file.open(filename, ios::out | ios::trunc);  // mode flags: https://cplusplus.com/reference/fstream/fstream/open/

    file << this->n_edges;
    file << '\n';
    file << this->n_nodes;
    file << '\n';
    file << this->nodes;
    file << '\n';
    file << this->_medoid;
    file << '\n';
    file << this->filteredMedoids;
    file << "\n";
    file << this->categories;
    file << '\n';
    file << this->Nout;
    
    file.close();

    c_log << "Graph Instance stored successfully in \"" << filename << '\"' << '\n';
}

// Loads a graph state from the specified file. A Graph instance must already be instantiated with the appropriate distance and isEmpty functions.
// IMPORTANT: makes use of overloaded >> operator to load the graph from a file
template<typename T>
void DirectedGraph<T>::load(const string& filename){
    if (filename == ""){ return; }
    fstream file;

    file.open(filename, ios::in);

    file >> this->n_edges;
    file.ignore(1);         // ignores \n
    file >> this->n_nodes;
    file.ignore(1);
    file >> this->nodes;
    file.ignore(1);
    file >> this->_medoid;
    file.ignore(1);
    file >> this->filteredMedoids;
    file.ignore(1);
    file >> this->categories;
    file.ignore(1); 
    file >> this->Nout;

    file.close();

    for (Node<T>& node : this->nodes)    // update isEmpty function (not serialized, argument in graph)
        node.isEmpty = this->isEmpty;
    
    c_log << "Graph Instance loaded successfully from \"" << filename << '\"' << '\n';
}


template<typename T>
void DirectedGraph<T>::init(){

    this->n_edges = 0;
    this->n_nodes = 0;
    this->nodes.clear();
    this->_medoid = -1;
    this->filteredMedoids.clear();
    this->categories.clear();
    this->Nout.clear();

    c_log << "Graph Successfully initialized to default values apart from function arguments.\n";
}