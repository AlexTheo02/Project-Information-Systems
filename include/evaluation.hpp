#pragma once

#include "config.hpp"
#include "util.hpp"
#include "types.hpp"
#include "graph_implementation.hpp"

// This file contains helper evaluation functions specific for types

void profileVamana(
    int L, int R, float a, int k,
    DirectedGraph<vector<float>>& DG,
    const vector<vector<float>>& vectors,
    const vector<vector<float>>& queries,
    const vector<vector<int>>& groundtruth,
    map<vector<float>, int> v2id
    ){
    
    // ignores the SHOULD_OMIT flag. Forces output through cout (if available).

    auto startTime = chrono::high_resolution_clock::now();
    // Vamana indexing
    DG.vamanaAlgorithm(L, R, a);
    auto endTime = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);

    printf("Time to execute vamanaAlgorithm(L=%d, R=%d, a=%f, k=%d): ", L,R,a,k);
    
    printFormatMiliseconds(duration);

    cout << "Vamana Index Created!" << endl;

    // Initialize total recall variable
    float total_recall = 0;

    // get the already calculated medoid
    Node<vector<float>> medoid = DG.medoid();

    // Greedy search for all queries with start node the medoid of all nodes (already calculated)
    for (int i=0; i<queries.size(); i++){

        // Get neighbors for i-th query
        pair<unordered_set<int>, unordered_set<int>> GS_return =  DG.greedySearch(medoid, queries[i], k, L);
        unordered_set<int> neighbors = GS_return.first;
        
        // Calculate the current recall and add it to the sum of all recall scores
        total_recall += k_recall(neighbors, groundtruth[i]);

    }
    // Print out average recall score
    cout << "FINISHED!\nAverage recall score is: " << total_recall / queries.size() << endl;
}