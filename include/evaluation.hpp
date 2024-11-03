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
    
    auto startTime = chrono::high_resolution_clock::now();
    // Vamana indexing
    DG.vamanaAlgorithm(L, R, a);
    auto endTime = chrono::high_resolution_clock::now();

    // clear failbit (to change when new cout object)
    cout.clear();

    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);

    printf("Time to execute vamanaAlgorithm(L=%d, R=%d, a=%f, k=%d): ", L,R,a,k);
    
    printFormatMiliseconds(duration);

    cout << "Vamana Index Created!" << endl;

    float total_recall = 0;


    // get the already calculated medoid
    vector<float> medoid = DG.medoid();

    // Greedy search for all queries with start node the medoid of all nodes (already calculated)
    for (int i=0; i<queries.size(); i++){

        vector<set<vector<float>>> GS_return =  DG.greedySearch(medoid, queries[i], k, L);

        set<vector<float>> neighbors = GS_return[0];

        vector<int> neighborIds;

        for (vector<float> v : neighbors){
            neighborIds.push_back(v2id[v]);
        }

        total_recall += k_recall(neighborIds, groundtruth[i]);

    }
    cout << "FINISHED!\nAverage recall score is: " << total_recall / queries.size() << endl;
}