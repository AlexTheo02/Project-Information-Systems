#pragma once

#include "config.hpp"
#include "util.hpp"
#include "types.hpp"
#include "id.hpp"
#include "node_and_query.hpp"
#include "graph_implementation.hpp"
#include "filtered_graph_implementation.hpp"


using namespace std;

// This file is an interface file that contains all the dependencies of the project. Simply include "interface.hpp" in your project and you're good to go!

// Creates the index on the graph based on the indexing type and return the duration in miliseconds
template <typename T>
chrono::milliseconds createIndex(DirectedGraph<T>& DG, Args arguments){
    chrono::milliseconds duration = (chrono::milliseconds) 0;
    chrono::high_resolution_clock::time_point startTime, endTime;

    vector<vector<float>> data;

    switch (arguments.index_type){
        case VAMANA:
            // Read base vectors file
            data = read_vecs<float>(args.data_path, args.n_data);

            // Populate the Graph
            for (auto& v : data){
                DG.createNode(v);
            }

            // Start the timer and create the index using vamanaAlgorithm
            startTime = chrono::high_resolution_clock::now();
            DG.vamanaAlgorithm(arguments.L, arguments.R, arguments.a);
            endTime = chrono::high_resolution_clock::now();

            // Calculate duration
            duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
            
            break;

        case FILTERED_VAMANA:
            // Read data
            
            ReadBin(args.data_path, args.dim_data, data);

            // Populate the Graph
            for (auto& v : data){
                DG.createNode(v);
            }

            // Start the timer and create the index using vamanaAlgorithm
            startTime = chrono::high_resolution_clock::now();
            DG.filteredVamanaAlgorithm(arguments.L, arguments.R, arguments.a, arguments.threshold);
            endTime = chrono::high_resolution_clock::now();

            // Calculate duration
            duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);

            break;

        case STITCHED_VAMANA:
            // Read data
            ReadBin(args.data_path, args.dim_data, data);

            // Populate the Graph
            for (auto& v : data){
                DG.createNode(v);
            }
            
            // Start the timer and create the index using vamanaAlgorithm
            startTime = chrono::high_resolution_clock::now();
            // DG.stitchedVamanaAlgorithm(arguments.a, arguments.Lsmall, arguments.Rsmall, arguments.R); // TODO
            endTime = chrono::high_resolution_clock::now();

            // Calculate duration
            duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);

            break;
        
        default:
            break;
    }
    return duration;
}


template <typename T>
unordered_set<Id> DirectedGraph<T>::findNeighbors(Query<T> q){

    // Set for storing the query's neighbors
    unordered_set<Id> queryNeighbors;
    
    // Check index_type
    if(args.index_type == VAMANA)
        queryNeighbors = this->greedySearch(this->medoid(), q.value, args.k, args.L).first;
    else if(args.index_type == FILTERED_VAMANA || args.index_type == STITCHED_VAMANA){                      // in both cases we call filteredGreedySearch

        unordered_set<Id> starting_nodes = (unordered_set<Id>) {findMedoids(args.threshold).at(q.category)};   // because each node belongs to at most one category.
        queryNeighbors = this->filteredGreedySearch(starting_nodes, q, args.k, args.L).first;
    }

    // Return the set containing the query's neighbors
    return queryNeighbors;
}

template <typename T>
vector<unordered_set<Id>> DirectedGraph<T>::findQueriesNeighbors(string queries_path, int read_arg){

    cout << "In Queries Neighbors" << endl;
    // Read queries
    vector<T> queries;
    
    // Check index type
    if(args.index_type == VAMANA){
        // If default value
        if(read_arg == -1)
            read_arg = args.n_queries;
        
        // Read query vectors file
        queries = read_vecs<float>(queries_path, read_arg);
    }else{
        // If default value
        if(read_arg == -1)
            read_arg = args.dim_query;
        // Read query vectors file
        ReadBin(queries_path, read_arg, queries);
    }

    vector<unordered_set<Id>> returnVec;

    cout << queries.size() << endl;
    cout << "Before for loop" << endl;
    // Iterate through the queries
    for(int i = 0; i < queries.size(); i++){
        
        if(args.index_type == VAMANA){
            Query<T> q(i, -1, false, queries[i], this->isEmpty);
            returnVec.push_back(findNeighbors(q));

        }else{
            T newValue(queries[i].begin() + 4, queries[i].end());
            Query<T> q(i, queries[i][1], queries[i][0], newValue, this->isEmpty);
            // Append each query's neighbors to the vector
            returnVec.push_back(findNeighbors(q));
        }
        
    }

    // Return the vector
    return returnVec;

}

// Evaluate given index based on its types and return average recall score
template <typename T>
float evaluateIndex(DirectedGraph<T>& DG, Args arguments){

    vector<vector<Id>> groundtruth;
    // If the groundtruth path is given, then read the specified file
    if(args.groundtruth_path != ""){

        if(args.index_type == VAMANA)
            groundtruth = read_vecs<Id>(args.groundtruth_path, args.n_groundtruths);
        else{
            // Open the file and read its contents
            fstream file;
            file.open(args.groundtruth_path, ios::in);
            
            // Read the file's contents
            file >> groundtruth;

            // Close the file
            file.close();
        }
    }

    vector<unordered_set<Id>> queriesNeighbors = DG.findQueriesNeighbors(args.queries_path);
    float total_recall = 0.f, query_recall;
    
    cout << "Found Neighbors\n";
    // cout << queriesNeighbors.size() << groundtruth.size() << endl;
    for(int i = 0; i < args.n_queries; i++){
        // Calculate the current recall and add it to the sum of all recall scores
        query_recall = k_recall(queriesNeighbors[i], groundtruth[i]);
        c_log << "Recall score for query: " << i+1 << "/" << args.n_queries << ":\t\t" << query_recall << "\n";
        total_recall += query_recall;
    }

    return total_recall / args.n_queries;

}

template <typename T>
vector<vector<Id>> generateGroundtruth(vector<vector<T>>& data, vector<vector<T>>& queries){

    // Create map category -> set of pair<nodeId, value>
    unordered_map<int, unordered_map<Id,vector<T>>> categories;
    for (int i=0; i<data.size(); i++){
        vector<T> vec = data[i];
        int category = vec[0];
        vector<T> value(vec.begin() + 2, vec.end());
        pair<Id, vector<T>> nodePair(i,value);
        categories[category].insert(nodePair);
    }

    // Initialize neighbors vector
    vector<vector<Id>> queryNeighbors;

    // For each query
    for(int i = 0; i < queries.size(); i++){
        cout << "Generating groundtruth for query: " << i << endl;

        vector<T> query = queries[i];
        vector<T> queryValue(query.begin() + 4, query.end());
        int category = query[1];

        T distance;
        unordered_map <Id, T> distances; 

        // Query is filtered
        if (category != -1){
            // Iterate over same-category nodes and calculate the distance for each of them
            for (pair<Id, vector<T>> vecPair : categories[category]){
                distances[vecPair.first] = euclideanDistance(queryValue,vecPair.second);
            }
        }
        // Query is unfiltered 
        else{
            // Iterate over all nodes and calculate the distance for each of them
            for (int i=0; i<data.size(); i++){
                vector<T> vec = data[i];
                vector<T> value(vec.begin() + 2, vec.end());
                distances[i] = euclideanDistance(queryValue, value);
            }
        }

        // Convert distances map to a vector of pairs
        vector<pair<Id, T>> distancesVec(distances.begin(), distances.end());

        // Sort distancesVec by the second element (distance) in ascending order
        sort(distancesVec.begin(), distancesVec.end(),
             [](const pair<Id, T>& a, const pair<Id, T>& b) {
                 return a.second < b.second;
             });

        // Extract sorted Ids based on distances and store in sortedIds
        vector<Id> neighbors;
        int minimum = (distancesVec.size() > 100) ? 100 : distancesVec.size();
        for (int k = 0; k < minimum; k++){
            auto& p = distancesVec[k];
            neighbors.push_back(p.first);
        }

        // Add sorted Ids for this query to the result
        queryNeighbors.push_back(neighbors);

    }

    return queryNeighbors;

}