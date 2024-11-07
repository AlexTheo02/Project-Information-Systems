#pragma once

#include "types.hpp"

// This file implements member functions of the DirectedGraph class template declared in the types.hpp header file.

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
            Node<T> pmin = _myArgMin(diff, q.value);     // pmin is the node with the minimum distance from query xq

            V.insert(pmin.id);

            unordered_set<int> filteredNoutPmin = this->filterSet(setSubtraction(this->Nout[pmin.id], V), q.category);

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