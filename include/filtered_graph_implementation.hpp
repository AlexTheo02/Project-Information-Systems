#pragma once

#include "types.hpp"

// This file implements member functions of the DirectedGraph class template declared in the types.hpp header file.


// medoids
template <typename T>
const unordered_map<int, int> DirectedGraph<T>::filteredMedoids(int threshold){

    // argument checks

    // check Nthreads and pass to _serialFilteredMedoids or _parallelFilteredMedoids

    unordered_map<int, int> M;
    vector<int> T_counter(this->n_nodes, 0);

    for (pair<int, unordered_set<int>> cpair : this->categories){
    
        unordered_set<int> Rf;
        while (Rf.size() < threshold){ Rf.insert(sampleFromContainer(cpair.second)); }

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
bool DirectedGraph<T>::filteredVamanaAlgorithm(int L, int R, float a){

    // initialize G as an empty graph => clear all edges
    if(this->clearEdges() == false)
        return false;

    // int all_medoid = this->medoid();
    unordered_map<int, int> st = this->filteredMedoids(L);

}