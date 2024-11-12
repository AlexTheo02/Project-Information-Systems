#pragma once

#include "config.hpp"
#include "util.hpp"
#include "types.hpp"
#include "graph_implementation.hpp"

// This file contains helper evaluation functions specific for types

void profileUnfilteredVamana(
    int L, int R, float a, int k,
    DirectedGraph<vector<float>>& DG,
    const vector<vector<float>>& vectors,
    const vector<vector<float>>& queries,
    const vector<vector<int>>& groundtruth
    ){

    auto startTime = chrono::high_resolution_clock::now();

    DG.vamanaAlgorithm(L, R, a);

    auto endTime = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);

    printf("Time to execute vamanaAlgorithm(L=%d, R=%d, a=%f, k=%d): ", L,R,a,k);
    printFormatMiliseconds(duration);

    cout << "Vamana Index Created!" << endl;

    // Initialize total recall variable
    float total_recall = 0;

    // get the already calculated medoid
    Id medoid = DG.medoid();

    // Greedy search for all queries with start node the medoid of all nodes (already calculated)
    for (int i=0; i<queries.size(); i++){

        // Get neighbors for i-th query
        pair<unordered_set<Id>, unordered_set<Id>> GS_return =  DG.greedySearch(medoid, queries[i], k, L);
        unordered_set<Id> neighbors = GS_return.first;
        
        // Calculate the current recall and add it to the sum of all recall scores
        total_recall += k_recall(neighbors, groundtruth[i]);

    }
    // Print out average recall score
    cout << "FINISHED!\nAverage recall score is: " << total_recall / queries.size() << endl;
}

void profileFilteredVamana(
    int L, int R, float a, int k, float t,
    DirectedGraph<vector<float>>& DG,
    const vector<vector<float>>& vectors,
    const vector<vector<float>>& queries,
    const vector<vector<Id>>& groundtruth,
    string filepath
    ){
    
    // auto startTime = chrono::high_resolution_clock::now();
    // DG.filteredVamanaAlgorithm(L, R, a, t);
    // auto endTime = chrono::high_resolution_clock::now();
    // DG.store(filepath); // store the vamana Index

    // auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);

    // printf("Time to execute filteredVamanaAlgorithm(L=%d, R=%d, a=%f, t=%f): ", L,R,a,t);
    // printFormatMiliseconds(duration);

    cout << "Loading the graph from \"" << filepath <<"\"" << endl;
    DG.load(filepath);

    cout << "Vamana Index Created!" << endl;

    // Initialize total recall variable
    float total_recall = 0;

    // Find medoid nodes for each category
    unordered_map<int,Id> medoidMap = DG.findMedoids(t);


    // (uncomment to estimate medoid for all points by finding the medoid of the medoids of each category. Used for storing DG._medoid for graph instance)
    // // create set with all medoid Nodes (not ids)
    // vector<Node<vector<float>>> medoid_set;
    // for (pair<int, Id> cpair : medoidMap){
    //     medoid_set.push_back(DG.getNodes()[cpair.second]);
    // }
    // // call DG.medoid(set_medoids, store = true)
    // DG.medoid(medoid_set, true);
    // // call DG.store()
    // DG.store(filepath);
    // return;
    







    cout << "Calculating average recall score" << endl;
    // Greedy search for all queries with start node the medoid of all nodes (already calculated)
    for (int i=0; i<queries.size(); i++){
        vector<float> newValue(queries[i].begin() + 4, queries[i].end());
        Query<vector<float>> q(i, queries[i][1], queries[i][0], newValue, vectorEmpty<float>);

        // finding start nodes for greedy search
        unordered_set<Id> start_nodes = (unordered_set<Id>) {medoidMap[q.category]};
        // Get neighbors for i-th query
        pair<unordered_set<Id>, unordered_set<Id>> GS_return =  DG.filteredGreedySearch(start_nodes, q, k, L);
        unordered_set<Id> neighbors = GS_return.first;

        float recall = k_recall(neighbors, groundtruth[i]);
        cout << "Recall score for query: " << i+1 << "/" << queries.size() << " | category = " << q.category <<": \t\t" << recall << endl;
        
        // Calculate the current recall and add it to the sum of all recall scores
        total_recall += recall;
    }
    // Print out average recall score
    cout << "FINISHED!\nAverage recall score is: " << total_recall / queries.size() << endl;
}