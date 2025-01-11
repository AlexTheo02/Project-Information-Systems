#pragma once

#include "types.hpp"

// This file implements member functions of the DirectedGraph class template declared in the types.hpp header file.


// medoids
template <typename T>
const unordered_map<int, Id> DirectedGraph<T>::findMedoids(float threshold){
    
    c_log << "Filtered Medoids\n";

    // argument checks
    if (threshold <= 0 || threshold > 1){ throw invalid_argument("threshold must be in (0,1]\n"); }
    if (!this->filteredMedoids.empty()) { return this->filteredMedoids; }
    
    return _filtered_medoid(threshold);
}

template <typename T>
const unordered_map<int, Id> DirectedGraph<T>::_filtered_medoid(float threshold){

    // Initialize T_counter (counts how many times a specific node has been selected as a medoid)
    vector<int> T_counter(this->n_nodes, 0);

    // For each category
    for (pair<int, unordered_set<Id>> cpair : this->categories){
        
        vector<Node<T>> Rf;
        unordered_set<Id> remaining(cpair.second.begin(), cpair.second.end()); // remaining is a copy that holds all the remaining values to be sampled from

        while (!remaining.empty() && Rf.size() < (ceil(threshold * cpair.second.size()))){
            int sample = sampleFromContainer(remaining);
            Rf.push_back(this->nodes[sample]);
            remaining.erase(sample);
        }

        this->filteredMedoids[cpair.first] = this->medoid(Rf);
    }
    c_log << "MEDOIDS FOUND" << "\n";
    
    return this->filteredMedoids;
}

// Returns a filtered set
template <typename T>
unordered_set<Id> DirectedGraph<T>::filterSet(unordered_set<Id> S, int filter){
    if (filter == -1){
        return S;
    }
    if(S.empty()) return S;
    unordered_set<Id> filtered;

    for (Id s : S){
        if (this->nodes[s].category == filter){
            filtered.insert(s);
        }
    }
    
    return filtered;
}

template <typename T>
const pair<unordered_set<Id>, unordered_set<Id>> DirectedGraph<T>::filteredGreedySearch(Id s, Query<T> q, int k, int L){

    c_log << "Filtered Greedy Search\n";

    // Argument checks

    // No filters are present, perform unfiltered greedy search
    if (q.empty()) { return this->greedySearch(this->startingNode(), q.value, k, L); }

    if (s == -1){ throw invalid_argument("No start node was provided.\n"); }

    if (k < 0){ throw invalid_argument("K must be greater than or equal to 0.\n"); }

    if (L < k){ throw invalid_argument("L must be greater or equal to K.\n"); }

    if (args.n_threads > 1){


        unique_lock<mutex> _lock(this->_mx_cv);
        // cout << "In Greedy Search. Lock Acquired 1" << endl;
        assert(_lock.owns_lock());
        while(this->_active_W == true){
            // cout << "In Greedy Search. Active Writer. Should Wait" << endl;
            this->_cv_reader.wait(_lock);
        }
        assert(_lock.owns_lock());
        this->_active_GS++;
        assert(_lock.owns_lock());
        this->_cv_reader.notify_all();
        // cout << "In Greedy Search. Notified and Exiting 1" << endl;

    } // end of RAII scope => invalidation of _lock, and therefore releasing lock on mutex (automatically)

    pair<unordered_set<Id>, unordered_set<Id>> rv = (args.usePQueue)
        ? this->_pqueue_filteredGreedySearch(s, q, k, L)
        : this->_set_filteredGreedySearch(s, q, k, L);

    if (args.n_threads > 1){
        unique_lock<mutex> _lock(this->_mx_cv);
        // cout << "In Greedy Search 2 Acquired Lock" << endl;
        assert(_lock.owns_lock());
        if (--this->_active_GS == 0){
            this->_cv_writer.notify_one();
        }
        assert(_lock.owns_lock());
        // cout << "In Greedy Search. Notified and Exiting 2" << endl;
    }
    return rv;
}

// Set Filtered Greedy Search
template <typename T>
const pair<unordered_set<Id>, unordered_set<Id>> DirectedGraph<T>::_set_filteredGreedySearch(Id s, Query<T> q, int k, int L){

    // ofstream outFile = GS_costs_init();
    // float _cost = 0;

    // Create empty sets
    unordered_set<Id> Lc, V, diff;
    Node<T> sNode;

    // Get node using id from s
    sNode = this->nodes[s];
    
    // Category match
    if (sNode.category == q.category) {
        // _cost = 0;
        Lc.insert(s);
    }

    // GS_costs_write(outFile, _cost);

    while (!(diff = setSubtraction(Lc,V)).empty()){
        Id pmin = _myArgMin(diff, q.value);     // pmin is the node with the minimum distance from query xq

        V.insert(pmin);

        unordered_set<Id> filteredNoutPmin;
        if (mapKeyExists(pmin, this->Nout))
            filteredNoutPmin = this->filterSet(setSubtraction(this->Nout[pmin], V), q.category);

        // int sz_before = Lc.size();
        Lc.insert(filteredNoutPmin.begin(), filteredNoutPmin.end());
        // int sz_after = Lc.size();
        // _cost = sz_after - sz_before;    // how many successful insertions

        if (Lc.size() > L){
            // _cost += Lc.size();
            Lc = _closestN(L, Lc, q.value); // function: find N closest points from a specific xq from given set and return them
        }
        // GS_costs_write(outFile, _cost);
    }
    
    pair<unordered_set<Id>, unordered_set<Id>> ret;

    // if (k < Lc.size())
    //     _cost = Lc.size();
    // else
    //     _cost = 0;
    // GS_costs_write(outFile, _cost);

    ret.first = _closestN(k, Lc, q.value);
    ret.second = V;

    // outFile.close();
    // greedySearchCount++;

    return ret;
}

// Pqueue Filtered Greedy Search
template <typename T>
const pair<unordered_set<Id>, unordered_set<Id>> DirectedGraph<T>::_pqueue_filteredGreedySearch(Id s, Query<T> q, int k, int L){

    // ofstream outFile = GS_costs_init();
    // float _cost = 0;

    // Create empty sets and initialize the priority queue
    unordered_set<Id> V, diff; 
    function<bool(Id, Id)> comparator = 
        [q,this] (int id1, int id2) {
            return (this->d(q.value, this->nodes[id1].value) < this->d(q.value, this->nodes[id2].value));   // descending comparator (maxHeap)
        };

    priority_queue<Id, vector<Id>, function<bool(Id, Id)>> Lc(comparator);

    Node<T> sNode;

    // Get node using id from s
    sNode = this->nodes[s];
    
    // Category match
    if (sNode.category == q.category) {
        // _cost += log(1); // = 0 ...
        Lc.push(s);
    }
    // GS_costs_write(outFile, _cost);

    while (!(diff = PQSubtraction(Lc,V)).empty()){
        Id pmin = _myArgMin(diff, q.value);     // pmin is the node with the minimum distance from query xq

        V.insert(pmin);

        unordered_set<Id> filteredNoutPmin;
        if (mapKeyExists(pmin, this->Nout)){
            filteredNoutPmin = this->filterSet(setSubtraction(this->Nout[pmin], V), q.category);
            // _cost = 0;
        }

        // if should insert
        for (const Id& neighbor : filteredNoutPmin){
            if (Lc.size() < L){
                // _cost += log(Lc.size());
                Lc.push(neighbor);
            }
            else if (this->d(this->nodes[neighbor].value, q.value) < this->d(this->nodes[Lc.top()].value, q.value)){
                Lc.pop();
                // _cost += log(Lc.size());
                Lc.push(neighbor);
            }
        }
        // GS_costs_write(outFile, _cost);
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

template <typename T>
void DirectedGraph<T>::filteredRobustPrune(Id p, unordered_set<Id> V, float a, int R){

    c_log << "Filtered Robust Prune\n";

    // argument checks
    if (a < 1) { throw invalid_argument("Parameter a must be >= 1.\n"); }

    if (R <= 0) {throw invalid_argument("Parameter R must be > 0.\n"); }

    if (p < 0 || p >= this->n_nodes){ throw invalid_argument("Invalid Index was provided.\n"); }

    if (this->nodes[p].empty()) { throw invalid_argument("No node was provided.\n"); }

    {   // RAII scope
        unique_lock<mutex> _lock(this->_mx_cv, defer_lock);

        // cout << "In RB 1." << endl;

        // Entry Section
        if (args.n_threads > 1){
            _lock.lock();
            // cout << "In RB 1. Acquired Lock" << endl;
            assert(_lock.owns_lock());
            while(this->_active_GS != 0 || this->_active_W == true){
                // cout << "In RB 1. Active Reader or Writer. Should Wait" << endl;
                this->_cv_writer.wait(_lock);
            }
            // cout << "In RB 1. Done Waiting" << endl;
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
            // cout << "In RB 1. CS done" << endl;
            assert(_lock.owns_lock());
            this->_active_W = false; // also critical operation but for synchronization. Is under lock.
            this->_cv_writer.notify_one();
            this->_cv_reader.notify_all();
            // cout << "In RB 1. Notified and Exiting" << endl;
        }

    } // end of RAII scope => invalidation of _lock and freeing of mutex

    V.erase(p);

    vector<Id> batch;

    while (!V.empty()){
        Id pmin = this->_myArgMin(V, this->nodes[p].value);

        batch.push_back(pmin); // store pmin to a vector Id then addbatch 

        if (batch.size() == R)
            break;

        // pmin = p*, pv = p', p = p (as seen in paper)
        // *it = pv
        
        // safe iteration on V for deletions
        for (auto it = V.begin(); it != V.end(); /*no increment here*/){ // safe set iteration with mutable set during the iteration
            if (this->nodes[*it].category == this->nodes[p].category && this->nodes[p].category != this->nodes[pmin].category) {it++; continue;}
            if (a * this->d(this->nodes[pmin].value, this->nodes[*it].value) <= this->d(this->nodes[p].value, this->nodes[*it].value)){
                it = V.erase(it); // it now points to the next element in the set
            }
            else { it++; }
        }
    }

    { // RAII scope
        unique_lock<mutex> _lock(this->_mx_cv, defer_lock);

        // cout << "In RB 2." << endl;
        // Entry Section
        if (args.n_threads > 1){
            _lock.lock();
            // cout << "In RB 2. Acquired Lock" << endl;
            assert(_lock.owns_lock());
            while(this->_active_GS != 0 || this->_active_W == true){
                // cout << "In RB 2. Active Reader or Writer. Should Wait" << endl;
                this->_cv_writer.wait(_lock);
            }
            // cout << "In RB 2. Done Waiting" << endl;
            assert(_lock.owns_lock());
            this->_active_W = true;
        }

        // Critical Section
        this->addBatchNeigbors(p, batch);
        // End of Critical Section

        // Exit Section 
        if (args.n_threads > 1){
            // cout << "In RB 2. CS done" << endl;
            assert(_lock.owns_lock());
            this->_active_W = false;
            this->_cv_writer.notify_one();
            this->_cv_reader.notify_all();
            // cout << "In RB 2. Notified and Exiting" << endl;
        }   
    }// end of RAII scope => invalidation of _lock and freeing of mutex
}

template <typename T>
bool DirectedGraph<T>::filteredVamanaAlgorithm(int L, int R, float a, float t){

    if (R <= 0){ throw invalid_argument("R must be a positive, non-zero integer.\n"); }

    if (L < 1) { throw invalid_argument("Parameter L must be >= 1.\n"); }

    if (a < 1) { throw invalid_argument("Parameter a must be >= 1.\n"); }

    if (t <= 0 || t > 1) { throw invalid_argument("Parameter t must be between (0,1]"); }

    if (this->nodes.size() == 1) return true;   // no edges possible

    c_log << "Filtered Vamana\n";
    bool rv;

    // initialize G as an empty graph => clear all edges
    if(this->clearEdges() == false)
        return false;
    
    vector<Id> nodes_ids(this->n_nodes);
    iota(nodes_ids.begin(), nodes_ids.end(), 0);

    vector<Id> perm_id = permutation(nodes_ids);

    if (args.n_threads == 1){
        rv =  this->_serial_filteredVamana(L, R, a, t, ref(perm_id));
    }
    else{
        
        rv =  this->_parallel_filteredVamana(L, R, a, t, ref(perm_id));
    }
    
    if (!rv) { return false; }

    if (args.extraRandomEdges > 0) rv = this->Rgraph(args.extraRandomEdges); // adds additional random edges
    
    c_log << "Filtered Vamana Index Created.\n";
    return rv;
}

template <typename T>
bool DirectedGraph<T>::_serial_filteredVamana(int L, int  R, float a, float t, vector<Id>& perm){

    for (Id& si_id : perm){

        Node<T> si = this->nodes[si_id];
        // Id starting_node_i = this->startingNode(); // st[si.category];   // because each node belongs to at most one category.
        // unordered_map<int, Id> st = this->findMedoids(t);  // paper says starting points should be the medoids found in [algorithm 2]
        

        // create query with si value to pass to filteredGreedySearch
        Query<T> q(si.id, si.category, true, si.value, this->isEmpty);

        unordered_set<Id> Vi = this->filteredGreedySearch(this->startingNode(q.category), q, 0, L).second;

        filteredRobustPrune(si.id, Vi, a, R);

        if (mapKeyExists(si.id, this->Nout)){
            
            unordered_map<Id, unordered_set<Id>> NoutCopy(this->Nout.begin(), this->Nout.end());
            for (const Id j : NoutCopy[si.id]){  // for every neighbor j of si

                this->addEdge(j, si.id);   // does it in either case (simpler code, robust prune clears all neighbors after copying to candidate set V anyway)
                int noutSize = this->Nout[j].size();
                if (noutSize > R)
                    filteredRobustPrune(j, this->Nout[j], a, R);
            }
        }
    }
    return true;
}

template <typename T>
bool DirectedGraph<T>::_parallel_filteredVamana(int L, int  R, float a, float t, vector<Id>& perm){

    int current_index = 0;
    mutex mx_index;

    vector<thread> threads;

    vector<char> rvs(args.n_threads, true);   // return values of threads - actually bool type

    if (!args.randomStart)
        this->findMedoids(t);   // pre-computing medoids for sync issues

    this->Nout.reserve(this->n_nodes);  // avoid unnecessary rehashing

    for (int i = 0; i < args.n_threads; i++){

        threads.push_back(thread(
            &DirectedGraph::_thread_filteredVamana_fn,
            this,
            ref(L),
            ref(R),
            ref(a),
            ref(t),
            ref(current_index),
            ref(mx_index),
            ref(rvs[i]),
            ref(perm)));
    }

    for (thread& th : threads)
        th.join();

    // Verify that all threads completed successfully
    for (bool rv : rvs){
        if (rv == false){
            c_log << "Something went wrong in the Filtered Vamana Index Creation.\n";
            return false;
        }
    }

    return true;

}

template <typename T>
void DirectedGraph<T>::_thread_filteredVamana_fn(int& L, int& R, float& a, float& t, int& current_index, mutex& mx, char& rv, vector<Id>& perm){
    
    mx.lock();
    while(current_index < perm.size()){
        int my_index = current_index++;
        mx.unlock();

        Id si_id = perm[my_index];
        Node<T>& si = this->nodes[si_id];
        // Id starting_node_i = this->startingNode(); // st[si.category];   // because each node belongs to at most one category.
        // unordered_map<int, Id> st = this->findMedoids(t);  // paper says starting points should be the medoids found in [algorithm 2]
        

        // create query with si value to pass to filteredGreedySearch
        Query<T> q(si.id, si.category, true, si.value, this->isEmpty);

        // cout << "Before Greedy Search" << endl;
        unordered_set<Id> Vi = this->filteredGreedySearch(this->startingNode(q.category), q, 0, L).second;
        // cout << "After Greedy Search" << endl;

        // mx.lock();
        filteredRobustPrune(si.id, Vi, a, R);
        // mx.unlock();

        {
            // RAII scope
            unique_lock<mutex> _lock(this->_mx_edges);
            if (mapKeyExists(si.id, this->Nout)){
                
                // mx.lock();
                unordered_set<Id> noutCopy_si(this->Nout[si.id].begin(), this->Nout[si.id].end());
                // mx.unlock();

                for (const Id j : noutCopy_si){  // for every neighbor j of si

                    // mx.lock();

                    unordered_set<Id> result(this->Nout[j].begin(), this->Nout[j].end());
                    // this->addEdge(j, si.id, true);   // does it in either case (simpler code, robust prune clears all neighbors after copying to candidate set V anyway)
                    result = unorderedSetUnion(result, unordered_set<Id>(si.id));
                    _lock.unlock();
                    if (result.size() > R)
                        filteredRobustPrune(j, this->Nout[j], a, R);
                    else{
                        this->addEdge(j, si.id);
                    }

                    _lock.lock();
                    // mx.unlock();
                }
            }
        }
        
        mx.lock();
    }
    mx.unlock();

}

template <typename T>
bool DirectedGraph<T>::stitchedVamanaAlgorithm(int L, int Rstitched, int Rsmall, float a){

    c_log << "Stitched Vamana\n";

    if (L < 1) { throw invalid_argument("Parameter L must be >= 1.\n"); }
    
    if (Rstitched <= 0){ throw invalid_argument("Rstitched must be a positive, non-zero integer.\n"); }

    if (Rsmall <= 0){ throw invalid_argument("Rsmall must be a positive, non-zero integer.\n"); }

    if (a < 1) { throw invalid_argument("Parameter a must be >= 1.\n"); }

    if (args.n_threads <= 0) throw invalid_argument("args.n_threads constant is invalid. Value must be args.n_threads >= 1.\n");

    int extraRandomEdges = args.extraRandomEdges;
    if (args.extraRandomEdges > 0) args.extraRandomEdges = 0;   // Vamana for specific category should not add additional random edges

    bool rv =  (args.n_threads == 1)
        ? this->_serial_stitchedVamana(L, Rstitched, Rsmall, a)
        : this->_parallel_stitchedVamana(L, Rstitched, Rsmall, a);
    
    if (!rv) { return false; }

    args.extraRandomEdges = extraRandomEdges;
    if (args.extraRandomEdges > 0) rv = this->Rgraph(args.extraRandomEdges); // adds additional random edges (only in the completed stitched graph, not subgraphs)

    c_log << "Stitched Vamana Index Created!\n";
    return rv;
}

template <typename T>
bool DirectedGraph<T>::_serial_stitchedVamana(int L, int Rstitched, int Rsmall, float a){

    // initialize G as an empty graph => clear all edges
    if(this->clearEdges() == false)
        return false;

    DirectedGraph<T> DGf(this->d, this->isEmpty);
    for (pair<int, unordered_set<Id>> cpair : this->categories){
        
        vector<Id> original_id;                         // a vector that maps DGf node ids to the original this ids

        for (Id node : cpair.second){
            DGf.createNode(this->nodes[node].value);    // create nodes as unfiltered data (specific category)
            original_id.push_back(node);
        }

        c_log << "Creating Index for Category: " << cpair.first << '\n';

        // Creating Index for nodes of specific category as unfiltered data
        int Rsmall_f = min(Rsmall, (int)cpair.second.size() - 1);    // handle case when Rsmall > |Pf| - 1 for certain filters f
        if (!DGf.vamanaAlgorithm(L, Rsmall_f, a)){
            c_log << "Something went wrong in vamana algorithm.\n";
            return false;
        }
        c_log << "Creating Index for Category: " << cpair.first << " created.\n";
        
        
        c_log << "Pruning.\n";
        for (Node<T>& node : DGf.nodes){
            DGf.robustPrune(node.id, DGf.Nout[node.id], a, Rstitched);
        }
        
        c_log << "Pruning complete.Stitching with main graph.\n";

        // union of edges: this->Nout ∪= DGf.Nout
        for (pair<Id, unordered_set<Id>> edge : DGf.get_Nout()){
            Id from = edge.first;
            for (Id to : edge.second)
                this->addEdge(original_id[from], original_id[to]);
        }
        this->filteredMedoids[cpair.first] = original_id[DGf.medoid()];
        c_log << "Stitching successful.\n";

        // cleanup for next category
        original_id.clear();
        DGf.init();
    }

    return true;
}


template <typename T>
void DirectedGraph<T>::_thread_stitchedVamana_fn(int& L, int& Rstitched, int& Rsmall, float& a, int& category_index, mutex& mx_category_index, mutex& mx_merge, vector<int>& category_names, char& rv){
    

    mx_category_index.lock();
    while(category_index < this->categories.size()){
        int my_category_index = category_index++;
        mx_category_index.unlock();

        int my_category = category_names[my_category_index];

        vector<Id> original_id;                         // a vector that maps DGf node ids to the original this ids

        DirectedGraph<T> DGf(this->d, this->isEmpty);

        for (Id node : this->categories[my_category]){
            DGf.createNode(this->nodes[node].value);    // create nodes as unfiltered data (specific category)
            original_id.push_back(node);
        }

        c_log << "Creating Index for Category: " << my_category << '\n';

        // Creating Index for nodes of specific category as unfiltered data
        int Rsmall_f = min(Rsmall, (int)this->categories[my_category].size() - 1);    // handle case when Rsmall > |Pf| - 1 for certain filters f
        if (!DGf.vamanaAlgorithm(L, Rsmall_f, a)){
            c_log << "Something went wrong in vamana algorithm.\n";
            rv = false;
            return;
        }
        c_log << "Creating Index for Category: " << my_category << " created.\n";

        c_log << "Pruning.\n";
        for (Node<T>& node : DGf.nodes){
            DGf.robustPrune(node.id, DGf.Nout[node.id], a, Rstitched);
        }

        c_log << "Pruning complete.Stitching with main graph.\n";
        
        mx_merge.lock();
        // union of edges: this->Nout ∪= DGf.Nout
        for (pair<Id, unordered_set<Id>> edge : DGf.get_Nout()){
            Id from = edge.first;
            for (Id to : edge.second)
                this->addEdge(original_id[from], original_id[to]);
        }
        this->filteredMedoids[my_category] = original_id[DGf.medoid()];
        mx_merge.unlock();
        c_log << "Stitching successful.\n";

        // cleanup for next category
        original_id.clear();
        DGf.init();

        mx_category_index.lock();
        cout << "Finished: " << my_category << " with " << (int) this->categories[my_category].size() << " points. " << this->n_edges << endl;
    }
    mx_category_index.unlock();
}

template <typename T>
bool DirectedGraph<T>::_parallel_stitchedVamana(int L, int Rstitched, int Rsmall, float a){

    // initialize G as an empty graph => clear all edges
    if(this->clearEdges() == false)
        return false;

    vector<thread> threads;
    vector<char> rvs;   // return values of threads - actually bool type

    // vector containing all unique categories, sorted by their workload in descending order


    // Heuristic for better thread job scheduling: sort based on diminishing workload (Longest Processing Time First) (REFERENCE HERE) TODO
    // Copy elements into a vector of pairs to sort
    vector<pair<int, vector<Id>>> sorted_categories;

    // Convert the categories into a vector containing a pair of an int(category id) and a vector containing all the ids of that category's nodes
    for (pair<int, unordered_set<Id>> cpair : this->categories) {
        sorted_categories.emplace_back(cpair.first, vector<Id>(cpair.second.begin(), cpair.second.end()));
    }

    // Sort the vector based on the size of the vector in the second element
    sort(sorted_categories.begin(), sorted_categories.end(),
            [](const pair<int, vector<Id>>& cpair1, const pair<int, vector<Id>>& cpair2) {
                return cpair1.second.size() > cpair2.second.size();
            });


    vector<int> category_names;

    for (pair<int, vector<Id>> cpair : sorted_categories)
        category_names.push_back(cpair.first);

    int category_index = 0;

    mutex mx_category_index, mx_merge;

    for (int i = 0; i < args.n_threads; i++){
        rvs.push_back(true);
        threads.push_back(thread(
            &DirectedGraph::_thread_stitchedVamana_fn,
            this,
            ref(L),
            ref(Rstitched),
            ref(Rsmall),
            ref(a),
            ref(category_index),
            ref(mx_category_index),
            ref(mx_merge),
            ref(category_names),
            ref(rvs[i])));
    }

    for (thread& th : threads)
        th.join();

    // Verify that all threads completed successfully
    for (bool rv : rvs){
        if (rv == false){
            c_log << "Something went wrong in the Stitched Vamana Index Creation.\n";
            return false;
        }
    }

    return true;
}

template <typename T>
const Id DirectedGraph<T>::startingNode(optional<int> category){

    if (category == nullopt){  // category doesn't matter. Medoid or Sample from all nodes in graph
        if (args.randomStart) return sampleFromContainer(this->nodes).id;
        else return this->medoid();
    }
    else{   // category matters. Specific Category Medoid or Sample from specific category

        if (!mapKeyExists(category.value(), this->categories)) { 
            c_log << "WARNING: Category not found. No nodes of that category exist in the graph. Returning starting node of any category\n";
            return this->startingNode();
        }

        if (args.randomStart) return sampleFromContainer(this->categories[category.value()]);
        else { unordered_map<int, Id> medoids = this->findMedoids(args.threshold);
                return medoids[category.value()]; }
    }
}