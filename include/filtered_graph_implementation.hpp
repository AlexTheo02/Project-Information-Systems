#pragma once

#include "types.hpp"

// This file implements member functions of the DirectedGraph class template declared in the types.hpp header file.


// medoids
template <typename T>
const unordered_map<int, int> DirectedGraph<T>::filteredMedoids(float threshold){ // threshold is (0 - 1]

    // argument checks
    if (threshold <= 0 || threshold > 1){ throw invalid_argument("threshold must be in (0,1]\n"); }

    // check Nthreads and pass to _serialFilteredMedoids or _parallelFilteredMedoids

    unordered_map<int, int> M;
    vector<int> T_counter(this->n_nodes, 0);

    for (pair<int, unordered_set<int>> cpair : this->categories){
    
        unordered_set<int> Rf, remaining(cpair.second.begin(), cpair.second.end()); // remaining is a copy that holds all the remaining values to be sampled from

        while (!remaining.empty() && Rf.size() < (threshold * cpair.second.size())){
            int sample = sampleFromContainer(remaining);
            Rf.insert(sample);
            remaining.erase(sample);
        }

        // pmin = argmin p \in Rf (T[p])
        int min_val = numeric_limits<int>::max();
        int pmin = -1;

        for (int p : Rf){
            if (T_counter[p] < min_val){
                pmin = p;
                min_val = T_counter[p];
            }
        }

        M[cpair.first] = pmin;  // medoid candidate for specific category
        T_counter[pmin]++;      // incremenet pmin's counter (it has been found as the min that much times)
    }

    return M;
}




// Returns a filtered set
template <typename T>
unordered_set<int> DirectedGraph<T>::filterSet(unordered_set<int> S, int filter){
    unordered_set<int> filtered;
    for (int s : S){
        if (this->nodes[s].category == filter){
            filtered.insert(s);
        }
    }
    return filtered;
}

template <typename T>
const pair<unordered_set<int>, unordered_set<int>> DirectedGraph<T>::filteredGreedySearch(unordered_set<int>& S, Query<T> q, int k, int L){

    // Argument checks

    if (S.empty()){ throw invalid_argument("No start node was provided.\n"); }

    if (k <= 0){ throw invalid_argument("K must be greater than 0.\n"); }

    if (L < k){ throw invalid_argument("L must be greater or equal to K.\n"); }

    // No filters are present, perform unfiltered greedy search
    if (q.empty()) { return this->greedySearch(this->medoid(), q.value, k, L); }

    // Create empty sets
    unordered_set<int> Lc, V, diff;
    Node<T> sNode;

    for (int s : S){
        // Get node using id from S
        sNode = this->getNodes()[s];
        
        // Category match
        if (sNode.category == q.category) {
            Lc.insert(s);
        }

        while (!(diff = setSubtraction(Lc,V)).empty()){
            int pmin = _myArgMin(diff, q.value);     // pmin is the node with the minimum distance from query xq

            V.insert(pmin);

            unordered_set<int> filteredNoutPmin = this->filterSet(setSubtraction(this->Nout[pmin], V), q.category);

            Lc.insert(filteredNoutPmin.begin(), filteredNoutPmin.end());

            if (Lc.size() > L){
                Lc = _closestN(L, Lc, q.value); // function: find N closest points from a specific xq from given set and return them
            }
        }
    }
    pair<unordered_set<int>, unordered_set<int>> ret;
    
    ret.first = _closestN(k, Lc, q.value);
    ret.second = V;

    return ret;
}

template <typename T>
void DirectedGraph<T>::filteredRobustPrune(int p, unordered_set<int> V, float a, int R){


    V.insert(this->Nout[p].begin(), this->Nout[p].end());
    V.erase(p);
    this->clearNeighbors(p);

    while (!V.empty()){
        int pmin = this->_myArgMin(V, this->nodes[p].value);
        this->addEdge(p, pmin);

        if (this->Nout[p].size() == R)  break;

        // pmin = p*, pv = p', p = p (as seen in paper)
        for (int pv : V){
            if (this->nodes[pv].category != this->nodes[pmin].category && this->nodes[p].category != this->nodes[pmin].category) continue;

            if (a * this->d(this->nodes[pmin].value, this->nodes[pv].value) <= this->d(this->nodes[p].value, this->nodes[pv].value)){
                V.erase(pv);
            }
        }
    }
}

template <typename T>
bool DirectedGraph<T>::filteredVamanaAlgorithm(int L, int R, float a, float t){

    // initialize G as an empty graph => clear all edges
    if(this->clearEdges() == false)
        return false;

    // what is a good value of T for filteredMedoids?
    unordered_map<int, int> st = this->filteredMedoids(t);  // paper says starting points should be the medoids found in [algorithm 2]

    vector<Node<T>> perm = permutation(this->nodes);

    for (Node<T>& si : perm){
        unordered_set<int> starting_nodes_i = (unordered_set<int>) {st[si.category]};   // because each node belongs to at most one category.

        // create query with si value to pass to filteredGreedySearch
        Query<T> q(si.id, si.category, true, si.value, this->isEmpty);

        unordered_set<int> Vi = this->filteredGreedySearch(starting_nodes_i, q, 0, L).second;

        filteredRobustPrune(si.id, Vi, a, R);

        if (mapKeyExists(si.id, this->Nout)){
        
            for (const int j : this->Nout[si.id]){  // for every neighbor j of si

                this->addEdge(j, si.id);   // does it in either case (simpler code, robust prune clears all neighbors after copying to candidate set V anyway)
                if (this->Nout[j].size() > R)
                    filteredRobustPrune(j, this->Nout[j], a, R);
            }
        }
    }
}