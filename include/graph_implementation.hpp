#pragma once

#include "types.hpp"
#define tid std::this_thread::get_id() << " "
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
bool DirectedGraph<T>::addEdge(const Id from, const Id to, optional<bool> noLock){

    // extract the noLock value and initialize accordingly
    bool no_lock = (noLock == nullopt) ? false : noLock.value();
    unique_lock<mutex> _lock(this->_mx_edges, defer_lock);  // defer_lock => initialize unlocked.
    
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

    if (args.n_threads > 1 && !no_lock){
        _lock.lock();
    }

    int previous_size = this->Nout[from].size(); 
    if (previous_size == 0) this->Nout[from].reserve(args.R + 1);   // reserve space to avoid rehashing and unnecessary race conditions
    this->Nout[from].insert(to);
    int next_size = this->Nout[from].size(); 

    if(next_size == previous_size){
        c_log << "Cannot add edge. Edge already exists" << '\n';

        return false;
    }
    else this->n_edges++;


 
    return true;
}

// remove edge
template <typename T>
bool DirectedGraph<T>::removeEdge(const Id from, const Id to, optional<bool> noLock){

    // extract the noLock value and initialize accordingly
    bool no_lock = (noLock == nullopt) ? false : noLock.value();
    unique_lock<mutex> _lock(this->_mx_edges, defer_lock);  // defer_lock => initialize unlocked.

    if (args.n_threads > 1 && no_lock != true){
        _lock.lock();
    }

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

    unique_lock<mutex> _lock(this->_mx_edges, defer_lock);
    if (args.n_threads > 1)
        _lock.lock();

    // Node has outgoing neighbors
    if (mapKeyExists(id, this->Nout)){
        // For each outgoing neighbor, remove the edge
        unordered_set<Id> noutCopy(this->Nout[id].begin(), this->Nout[id].end());

        for (const Id n : noutCopy){      
            if (!this->removeEdge(id,n,true)){  // always call removeEdge with noLock = true. If parallel we have the lock, else (serial) lock is not needed
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

// Adds all nodes in the batch vector to as outgoing neighbors from specific node
template <typename T>
bool DirectedGraph<T>::addBatchNeigbors(const Id from, vector<Id> batch){

    // argument checks
    if (batch.empty()){
        c_log << "WARNING: batch is empty. No new edges\n";
        return true;
    }

    if (from >= this->n_nodes){
        c_log << "ERROR: Node does not exist in the graph" << '\n';
        return false;
    }

    unique_lock<mutex> _lock(this->_mx_edges, defer_lock);
    if (args.n_threads > 1)
        _lock.lock();

    bool rv = true;
    for (const Id& to : batch){
        if (!this->addEdge(from, to, true))
            rv = false;
    }

    return rv;
}

// clears all edges in the graph
template <typename T>
bool DirectedGraph<T>::clearEdges(void){
    if (this->n_nodes){
        for (Node<T> node : this->nodes){
            if (!this->clearNeighbors(node.id)){
                c_log << "ERROR: Failed to clear neighbors for node" << '\n';
                return false;
            }
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
    if(N >= S.size())
        return S;

    // transform the set to a vector for partitioning around a pivot
    vector<Id> Svec(S.begin(), S.end());

    // DistanceFromPointComparator<T> comparator(X, ASCENDING, this->d, cref(this->nodes));

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

// adds R randomly selected outgoing neighbors for each node in the graph. Return TRUE if successful, FALSE otherwise
template <typename T>
bool DirectedGraph<T>::Rgraph(int R){

    if (R < 0) { throw invalid_argument("R must be a positive integer.\n"); }

    if (R > this->n_nodes - 1){ throw invalid_argument("R cannot exceed N-1 (N = the total number of nodes in the Graph).\n"); }

    // avoid overflows
    unsigned long long fully_connected_capacity = this->n_nodes * (this->n_nodes - 1);
    unsigned long long expected_total_edges = this->n_edges + this->n_nodes*R;

    if (expected_total_edges > fully_connected_capacity) { throw invalid_argument("Total number of edges would exceed the fully connected capacity.\n"); }

    if (R <= log(this->n_nodes)){ c_log << "WARNING: R <= logn and therefore the graph will not be well connected.\n"; }
    
    if (R == 0){ c_log << "WARNING: R is set to 0. No edges will be added.\n"; }

    if (args.n_threads == 1){ return _serial_Rgraph(R); }
    else{ return _parallel_Rgraph(R); }
}

template<typename T>
bool DirectedGraph<T>::_serial_Rgraph(int R){

    for (Node<T>& n : this->nodes){     // for each node
        for (int i = 0; i < R; i++){    // repeat R times: sample from set and add until valid
            Node<T> nr;
            bool should_continue;
            int num_loops = 0;
            do{
                nr = sampleFromContainer(this->nodes);
                should_continue = this->addEdge(n.id, nr.id);
                num_loops++;
            }while(!should_continue && num_loops < 2*this->n_nodes); // addEdge fails when: 1. n == nr, 2. edge(n,nr) already exists
        }
    }
    return true;
}

template<typename T>
bool DirectedGraph<T>::_parallel_Rgraph(int R){
    int node_index = 0;
    mutex mx_index;

    vector<thread> threads;

    vector<char> rvs(args.n_threads, true);   // return values of threads - actually bool type

    for (int i = 0; i < args.n_threads; i++){

        threads.push_back(thread(
            &DirectedGraph::_thread_Rgraph_fn,
            this,
            ref(R),
            ref(node_index),
            ref(mx_index),
            ref(rvs[i])));
    }

    for (thread& th : threads)
        th.join();


    for (bool rv : rvs){
        if (rv == false){
            c_log << "Something went wrong in the Rgraph thread function.\n";
            return false;
        }
    }

    c_log << "Rgraph completed.\n";
    return true;

}

template<typename T>
void DirectedGraph<T>::_thread_Rgraph_fn(int& R, int& node_index, mutex& mx_index, char& rv){

    mx_index.lock();
    while(node_index < this->n_nodes){
        int my_index = node_index++;
        mx_index.unlock();
        
        Node<T>& n = this->nodes[my_index];
        for (int i = 0; i < R; i++){    // repeat R times: sample from set and add until valid
            Node<T> nr;
            int num_loops = 0; // timeout after sampling 2 * n_nodes
            bool should_continue;
            
            do{
                nr = sampleFromContainer(this->nodes);
                mx_index.lock();
                should_continue = this->addEdge(n.id, nr.id);
                
                mx_index.unlock();
                
                num_loops++;
            }while(!should_continue && num_loops < 2*this->n_nodes); // addEdge fails when: 1. n == nr, 2. edge(n,nr) already exists


            if(num_loops >= 2 * this->n_nodes){
                c_log << "WARNING: Rgraph sampling has timed out\n";
                rv = false;

            } 
        }

        mx_index.lock();
    }
    mx_index.unlock();

    rv = true;

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

    if (k < 0){ throw invalid_argument("K must be greater than or equal to 0.\n"); }

    if (L < k){ throw invalid_argument("L must be greater or equal to K.\n"); }

    if (args.n_threads > 1){


        unique_lock<mutex> _lock(this->_mx_cv);
        assert(_lock.owns_lock());
        while(this->_active_W == true){
            this->_cv_reader.wait(_lock);
        }
        assert(_lock.owns_lock());
        this->_active_GS++;
        assert(_lock.owns_lock());
        this->_cv_reader.notify_all();

    } // end of RAII scope => invalidation of _lock, and therefore releasing lock on mutex (automatically)

    pair<unordered_set<Id>, unordered_set<Id>> rv = (args.usePQueue)
        ? this->_pqueue_greedySearch(s, xq, k, L)
        : this->_set_greedySearch(s, xq, k, L);
    
    if (args.n_threads > 1){
        unique_lock<mutex> _lock(this->_mx_cv);
        assert(_lock.owns_lock());
        if (--this->_active_GS == 0){
            this->_cv_writer.notify_one();
        }
        assert(_lock.owns_lock());
    }

    return rv;
}

template <typename T>
const pair<unordered_set<Id>, unordered_set<Id>> DirectedGraph<T>::_set_greedySearch(Id s, T xq, int k, int L){

    // ofstream outFile = GS_costs_init();
    // float _cost = 0;

    // Create empty sets
    unordered_set<Id> Lc = {s}, V, diff; // Initialize Lc with s

    // _cost = 0;
    // GS_costs_write(outFile, _cost);

    
    while(!(diff = setSubtraction(Lc,V)).empty()){
        // _cost = 0;
        Id pmin = this->_myArgMin(diff, xq);    // pmin is the node with the minimum distance from query xq

        // If node has outgoing neighbors
        if (mapKeyExists(pmin, this->Nout)){
            // int sz_before = Lc.size();
            Lc.insert(this->Nout[pmin].begin(), this->Nout[pmin].end());
            // int sz_after = Lc.size();
            // _cost = sz_after - sz_before;    // how many successful insertions
        }
        
        V.insert(pmin);

        if (Lc.size() > L){
            // _cost += Lc.size();
            Lc = _closestN(L, Lc, xq);    // function: find N closest points from a specific xq from given set and return them
        }
        // GS_costs_write(outFile, _cost);
    }

    pair<unordered_set<Id>, unordered_set<Id>> ret;
    
    // if (k < Lc.size())
    //     _cost = Lc.size();
    // else
    //     _cost = 0;
    // GS_costs_write(outFile, _cost);

    ret.first = _closestN(k, Lc, xq);
    ret.second = V;

    // outFile.close();
    // greedySearchCount++;

    return ret;
}

template <typename T>
const pair<unordered_set<Id>, unordered_set<Id>> DirectedGraph<T>::_pqueue_greedySearch(Id s, T xq, int k, int L){

    // ofstream outFile = GS_costs_init();
    // float _cost = 0;
    
    // Create empty sets and initialize the priority queue
    unordered_set<Id> V, diff; 
    function<bool(Id, Id)> comparator = 
        [xq,this] (int id1, int id2) {
            return (this->d(xq, this->nodes[id1].value) < this->d(xq, this->nodes[id2].value));   // descending comparator (maxHeap)
        };

    priority_queue<Id, vector<Id>, function<bool(Id, Id)>> Lc(comparator);

    // Initialize Lc with s
    // _cost = log(1); // = 0
    // GS_costs_write(outFile, _cost);
    Lc.push(s);
        
    while(!(diff = PQSubtraction(Lc,V)).empty()){
        
        Id pmin = this->_myArgMin(diff, xq);    // pmin is the node with the minimum distance from query xq

        V.insert(pmin);

        // If node has outgoing neighbors
        if (mapKeyExists(pmin, this->Nout)){

            // _cost = 0;
            // if should insert
            for (const Id& neighbor : this->Nout[pmin]){
                if (Lc.size() < L){
                    // _cost += log(Lc.size());
                    Lc.push(neighbor);
                }
                else if (this->d(this->nodes[neighbor].value, xq) < this->d(this->nodes[Lc.top()].value, xq)){
                    Lc.pop();
                    // _cost += log(Lc.size());
                    Lc.push(neighbor);
                }
            }
            // GS_costs_write(outFile, _cost);
        }
    }

    pair<unordered_set<Id>, unordered_set<Id>> ret;

    // _cost = Lc.size();
    // GS_costs_write(outFile, _cost);

    int Lsize = Lc.size();
    for (int i = 0; i < Lsize - k; i++){
        if (!Lc.empty()){ Lc.pop(); }
    }

    Lsize = Lc.size();
    for (int i = 0; i < Lsize; i++){
        ret.first.insert(Lc.top());
        if (!Lc.empty()){ Lc.pop(); }
    }
    ret.second = V;

    // outFile.close();
    // greedySearchCount++;

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

    {   // RAII scope
        unique_lock<mutex> _lock(this->_mx_cv, defer_lock);

        // Entry Section
        if (args.n_threads > 1){
            _lock.lock();
            assert(_lock.owns_lock());
            while(this->_active_GS != 0 || this->_active_W == true){
                this->_cv_writer.wait(_lock);
            }
            assert(_lock.owns_lock());
            this->_active_W = true; // also critical operation but for synchronization. Is under lock.
        }

        // Critical Section
        if (mapKeyExists(p, this->Nout))
            V.insert(this->Nout[p].begin(), this->Nout[p].end());

        this->clearNeighbors(p);          // calls remove edge
        // End of Critical Section

        // Exit Section
        if (args.n_threads > 1){
            assert(_lock.owns_lock());
            this->_active_W = false; // also critical operation but for synchronization. Is under lock.
            this->_cv_writer.notify_one();
            this->_cv_reader.notify_all();
        }

    } // end of RAII scope => invalidation of _lock and freeing of mutex

    V.erase(p);

    Id p_opt;
    // assume neighbors have been cleared. They will be cleared afterwards for better synchronization between threads.
    // No effect in the final outcome.
    vector<Id> batch;
    
    while (!V.empty()){
        p_opt = this->_myArgMin(V, this->nodes[p].value);
        
        batch.push_back(p_opt); // store p_opts to a vector Id then addbatch 

        if (batch.size() == R)
            break;
        
        // *it = n = p', p_opt = p*
        for (auto it = V.begin(); it != V.end(); /*no increment here*/){    // safe set iteration with mutable set during the iteration
            if ( (a * this->d(this->nodes[p_opt].value, this->nodes[*it].value)) <= this->d(this->nodes[p].value, this->nodes[*it].value)){ 
                it = V.erase(it);
            }
            else { it++; }  // incrementing here because V.erase() returns the next iterator on successful deletion
        }
    }
    
    // synchronize with greedy search

    { // RAII scope
        unique_lock<mutex> _lock(this->_mx_cv, defer_lock);

        // Entry Section
        if (args.n_threads > 1){
            _lock.lock();
            assert(_lock.owns_lock());
            while(this->_active_GS != 0 || this->_active_W == true){
                this->_cv_writer.wait(_lock);
            }
            assert(_lock.owns_lock());
            this->_active_W = true;
        }

        // Critical Section
        this->addBatchNeigbors(p, batch);
        // End of Critical Section

        // Exit Section 
        if (args.n_threads > 1){
            assert(_lock.owns_lock());
            this->_active_W = false;
            this->_cv_writer.notify_one();
            this->_cv_reader.notify_all();
        }   
    }// end of RAII scope => invalidation of _lock and freeing of mutex


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
    this->clearEdges();


    if (args.useRGraph && this->Rgraph(R) == false)
        return false;
    c_log << "Graph initialized successfully!" << '\n';


    c_log << "Finalizing Vamana Index using the Vamana Algorithm . . ." << '\n';

    vector<Id> nodes_ids(this->n_nodes);
    iota(nodes_ids.begin(), nodes_ids.end(), 0);

    vector<Id> perm_id = permutation(nodes_ids);

    // bool rv = this->_serial_Vamana(L, R, a, perm_id);

    bool rv = (args.n_threads > 1)
        ? this->_parallel_Vamana(L, R, a, perm_id)
        : this->_serial_Vamana(L, R, a, perm_id);

    if (args.extraRandomEdges > 0){
        this->Rgraph(args.extraRandomEdges); // adds additional random edges
    }

    c_log << "Vamana Index Created!\n";
    return rv;
}

template <typename T>
bool DirectedGraph<T>::_serial_Vamana(int L, int R, float a, vector<Id>& permutation){

    for (const Id& si_id : permutation){
        Node<T>& si = this->nodes[si_id];
        pair<unordered_set<Id>, unordered_set<Id>> rv = greedySearch(this->startingNode(), si.value, 0, L); // k = 0 instead of 1, same as the filtered vamana

        // unordered_set<Id> Lc = rv.first; // not required.
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

template <typename T>
void DirectedGraph<T>::_thread_Vamana_fn(int& L, int& R, float& a, vector<Id>& permutation, int& current_index, mutex& mx_index, char& rv){

    mx_index.lock();
    while(current_index < permutation.size()){

        int my_index = current_index++;
        mx_index.unlock();

        Id si_id = permutation[my_index];
        Node<T>& si = this->nodes[si_id];
        pair<unordered_set<Id>, unordered_set<Id>> rv_gs = greedySearch(this->startingNode(), si.value, 0, L); // k = 0 instead of 1, same as the filtered vamana

        // unordered_set<Id> Lc = rv_gs.first; // not required.
        unordered_set<Id> V = rv_gs.second;

        this->robustPrune(si.id, V, a, R);
        
        // Another critical section?
        // Lock for check
        {
            // RAII scope
            unique_lock<mutex> _lock(this->_mx_edges);
            if (mapKeyExists(si.id, this->Nout)){

                unordered_set<Id> noutCopy_si(this->Nout[si.id].begin(), this->Nout[si.id].end());
                // We have the lock, get a copy of nout[si.id]
                for (const Id j : noutCopy_si){  // for every neighbor j of si

                    unordered_set<Id> result(this->Nout[j].begin(), this->Nout[j].end());
                    // this->addEdge(j, si.id, true);   // does it in either case (simpler code, robust prune clears all neighbors after copying to candidate set V anyway)
                    result = unorderedSetUnion(result, unordered_set<Id>(si.id));
                    _lock.unlock();
                    if (result.size() > R){
                        robustPrune(j, result, a, R);
                    }
                    else{
                        this->addEdge(j, si.id);
                    }

                    _lock.lock();
                    
                }
            }

        }
        // ^

        mx_index.lock();
    }
    mx_index.unlock();
}

template <typename T>
bool DirectedGraph<T>::_parallel_Vamana(int L, int R, float a, vector<Id>& permutation){

    int current_index = 0;
    mutex mx_index;

    vector<char> rvs(args.n_threads, true);
    vector<thread> threads;

    
    for (int i = 0; i < args.n_threads; i++){

        threads.push_back(thread(
            &DirectedGraph::_thread_Vamana_fn,
            this,
            ref(L),
            ref(R),
            ref(a),
            ref(permutation),
            ref(current_index),
            ref(mx_index),
            ref(rvs[i])));
    }

    for (thread& th : threads)
        th.join();

    for (bool rv : rvs){
        if (rv == false){
            c_log << "Something went wrong in the Vamana Index Creation.\n";
            return false;
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

    this->clearEdges();
    this->n_edges = 0;
    this->n_nodes = 0;
    this->nodes.clear();
    this->_medoid = -1;
    this->filteredMedoids.clear();
    this->categories.clear();
    this->Nout.clear();

    this->_active_W = 0;
    this->_active_GS = 0;

    c_log << "Graph Successfully initialized to default values apart from function arguments.\n";
}