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

    // // check Nthreads and pass to _serialFilteredMedoids or _parallelFilteredMedoids
    // if (N_THREADS == 1){
    //     return _filtered_serial_medoid(threshold);
    // }
    // return _filtered_parallel_medoid(threshold);

    return _filtered_serial_medoid(threshold);
}

template <typename T>
const unordered_map<int, Id> DirectedGraph<T>::_filtered_serial_medoid(float threshold){

    vector<int> T_counter(this->n_nodes, 0);

    for (pair<int, unordered_set<Id>> cpair : this->categories){
        
        vector<Node<T>> Rf;
        unordered_set<Id> remaining(cpair.second.begin(), cpair.second.end()); // remaining is a copy that holds all the remaining values to be sampled from

        while (!remaining.empty() && Rf.size() < (threshold * cpair.second.size())){
            int sample = sampleFromContainer(remaining);
            Rf.push_back(this->nodes[sample]);
            remaining.erase(sample);
        }

        this->filteredMedoids[cpair.first] = this->medoid(Rf);
        // // pmin = argmin p \in Rf (T[p])
        // int min_val = numeric_limits<int>::max();
        // Id pmin;

        // for (Id p : Rf){
        //     if (T_counter[p] < min_val){
        //         pmin = p;
        //         min_val = T_counter[p];
        //     }
        // }

        // this->filteredMedoids[cpair.first] = pmin;  // medoid candidate for specific category
        // T_counter[pmin]++;      // incremenet pmin's counter (it has been found as the min that much times)
    }
    cout << "MEDOIDS FOUND" << endl;
    return this->filteredMedoids;
}

template <typename T>
void DirectedGraph<T>::_filtered_thread_medoid_fn(
    mutex& T_counterMutex,
    mutex& categoryMutex,
    vector<int>& T_counter,
    int& categoryIndex,
    int maxCategoryIndex,
    vector<pair<int, unordered_set<Id>>>& categoryPairs,
    float threshold
    ){
    
    categoryMutex.lock();
    while (categoryIndex <= maxCategoryIndex){
        int myCategory = categoryIndex++; // save and increment
        cout << "Thread assigned category: " << myCategory << endl;
        categoryMutex.unlock();

        // Sampling
        vector<Node<T>> Rf;
        unordered_set<Id> remaining(categoryPairs[myCategory].second.begin(), categoryPairs[myCategory].second.end()); // remaining is a copy that holds all the remaining values to be sampled from

        while (!remaining.empty() && Rf.size() < (threshold * categoryPairs[myCategory].second.size())){
            int sample = sampleFromContainer(remaining);
            Rf.push_back(this->nodes[sample]);
            remaining.erase(sample);
        }

        // pmin = argmin p \in Rf (T[p])
        // int min_val = numeric_limits<int>::max();
        // Id pmin;

        // for (Id p : Rf){
        //     T_counterMutex.lock();
        //     if (T_counter[p] < min_val){
        //         pmin = p;
        //         min_val = T_counter[p];
        //     }
        //     T_counterMutex.unlock();
        // }

        // this->filteredMedoids[categoryPairs[myCategory].first] = pmin;  // medoid candidate for specific category

        this->filteredMedoids[categoryPairs[myCategory].first] = this->medoid(Rf);

        // T_counterMutex.lock();
        // T_counter[pmin]++;      // incremenet pmin's counter (it has been found as the min that much times)
        // T_counterMutex.unlock();

        categoryMutex.lock();
        cout << "Thread exiting category: " << myCategory << endl;
    }
    categoryMutex.unlock();
}

template <typename T>
const unordered_map<int, Id> DirectedGraph<T>::_filtered_parallel_medoid(float threshold){

    c_log << "Filtered Parallel Medoid\n";

    vector<int> T_counter(this->n_nodes, 0); // Shared resource
    vector<thread> threads(N_THREADS);  // a vector of size N_THREADS holding all the threads
    int threadIndex = 0;
    
    // Create a vector of category pairs
    vector<pair<int, unordered_set<Id>>> categoryPairs(this->categories.begin(), this->categories.end());

    int categoryIndex = 0; // Shared resource
    int maxCategoryIndex = categoryPairs.size() -1;

    // Create mutexes for shared resources
    mutex categoryMutex;
    mutex T_counterMutex;

    // Create the threads
    for (int i=0; i< min(N_THREADS, maxCategoryIndex); i++){

        threads[i] = thread(
            &DirectedGraph<T>::_filtered_thread_medoid_fn,
            this,
            ref(T_counterMutex),
            ref(categoryMutex),
            ref(T_counter),
            ref(categoryIndex),
            maxCategoryIndex,
            ref(categoryPairs),
            threshold
        );

        cout << "Created thread " << i << endl;
    }

    // Join the threads
    for (thread& th : threads){ th.join(); }

    cout << "MEDOIDS FOUND" << endl;
    return this->filteredMedoids;
}



// Returns a filtered set
template <typename T>
unordered_set<Id> DirectedGraph<T>::filterSet(unordered_set<Id> S, int filter){
    unordered_set<Id> filtered;
    for (Id s : S){
        if (this->nodes[s].category == filter){
            filtered.insert(s);
        }
    }
    return filtered;
}

template <typename T>
const pair<unordered_set<Id>, unordered_set<Id>> DirectedGraph<T>::filteredGreedySearch(unordered_set<Id>& S, Query<T> q, int k, int L){

    c_log << "Filtered Greedy Search\n";

    // Argument checks

    if (S.empty()){ throw invalid_argument("No start node was provided.\n"); }

    if (k < 0){ throw invalid_argument("K must be greater than or equal to 0.\n"); }

    if (L < k){ throw invalid_argument("L must be greater or equal to K.\n"); }

    // No filters are present, perform unfiltered greedy search
    if (q.empty()) { return this->greedySearch(this->medoid(), q.value, k, L); }

    // Create empty sets
    unordered_set<Id> Lc, V, diff;
    Node<T> sNode;

    for (Id s : S){
        // Get node using id from S
        sNode = this->nodes[s];
        
        // Category match
        if (sNode.category == q.category) {
            Lc.insert(s);
        }
    }

    while (!(diff = setSubtraction(Lc,V)).empty()){
        Id pmin = _myArgMin(diff, q.value);     // pmin is the node with the minimum distance from query xq

        V.insert(pmin);

        unordered_set<Id> filteredNoutPmin;
        if (mapKeyExists(pmin, this->Nout))
            filteredNoutPmin = this->filterSet(setSubtraction(this->Nout[pmin], V), q.category);

        Lc.insert(filteredNoutPmin.begin(), filteredNoutPmin.end());

        if (Lc.size() > L){
            Lc = _closestN(L, Lc, q.value); // function: find N closest points from a specific xq from given set and return them
        }
    }
    pair<unordered_set<Id>, unordered_set<Id>> ret;
    
    ret.first = _closestN(k, Lc, q.value);
    ret.second = V;

    return ret;
}

template <typename T>
void DirectedGraph<T>::filteredRobustPrune(Id p, unordered_set<Id> V, float a, int R){

    c_log << "Filtered Robust Prune\n";

    if (mapKeyExists(p, this->Nout))
        V.insert(this->Nout[p].begin(), this->Nout[p].end());
    
    V.erase(p);
    this->clearNeighbors(p);

    while (!V.empty()){
        Id pmin = this->_myArgMin(V, this->nodes[p].value);
        this->addEdge(p, pmin);

        if (this->Nout[p].size() == R)  break;

        // pmin = p*, pv = p', p = p (as seen in paper)
        // *it = pv
        
        // safe iteration on V for deletions
        for (auto it = V.begin(); it != V.end(); /*no increment here*/){ // safe set iteration with mutable set during the iteration
            if (this->nodes[*it].category != this->nodes[pmin].category && this->nodes[p].category != this->nodes[pmin].category) {it++; continue;}
            if (a * this->d(this->nodes[pmin].value, this->nodes[*it].value) <= this->d(this->nodes[p].value, this->nodes[*it].value)){
                it = V.erase(it);
            }
            else { it++; }
        }
    }
}

template <typename T>
bool DirectedGraph<T>::filteredVamanaAlgorithm(int L, int R, float a, float t){

    c_log << "Filtered Vamana\n";

    // initialize G as an empty graph => clear all edges
    if(this->clearEdges() == false)
        return false;



    // what is a good value of T for filteredMedoids?
    unordered_map<int, Id> st = this->findMedoids(t);  // paper says starting points should be the medoids found in [algorithm 2]
    vector<Node<T>> perm = permutation(this->nodes);
    

    for (Node<T>& si : perm){
        unordered_set<Id> starting_nodes_i = (unordered_set<Id>) {st[si.category]};   // because each node belongs to at most one category.

        // create query with si value to pass to filteredGreedySearch
        Query<T> q(si.id, si.category, true, si.value, this->isEmpty);

        unordered_set<Id> Vi = this->filteredGreedySearch(starting_nodes_i, q, 0, L).second;

        filteredRobustPrune(si.id, Vi, a, R);

        if (mapKeyExists(si.id, this->Nout)){
        
            for (const Id j : this->Nout[si.id]){  // for every neighbor j of si

                this->addEdge(j, si.id);   // does it in either case (simpler code, robust prune clears all neighbors after copying to candidate set V anyway)
                if (this->Nout[j].size() > R)
                    filteredRobustPrune(j, this->Nout[j], a, R);
            }
        }
    }
    return true;
}