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
    chrono::milliseconds duration = 0;

    switch (arguments.index_type){
        case VAMANA:
            // Read base vectors file
            vector<vector<float>> vectors = read_vecs<float>(args.data_path, args.n_data);

            // Populate the Graph
            for (auto& v : vectors){
                DG.createNode(v);
            }

            // Start the timer and create the index using vamanaAlgorithm
            chrono::milliseconds startTime = chrono::high_resolution_clock::now();
            DG.vamanaAlgorithm(arguments.L, arguments.R, arguments.a);
            chrono::milliseconds endTime = chrono::high_resolution_clock::now();

            // Calculate duration
            duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
            
            break;

        case FILTERED_VAMANA:
            // Read data
            vector<vector<float>> data;
            ReadBin(args.data_path, args.dim_data, data);

            // Populate the Graph
            for (auto& v : vectors){
                DG.createNode(v);
            }

            // Start the timer and create the index using vamanaAlgorithm
            chrono::milliseconds startTime = chrono::high_resolution_clock::now();
            DG.filteredVamanaAlgorithm(arguments.L, arguments.R, arguments.a, arguments.threshold);
            chrono::milliseconds endTime = chrono::high_resolution_clock::now();

            // Calculate duration
            duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);

            break;

        case STITCHED_VAMANA:
            // Read data
            vector<vector<float>> data;
            ReadBin(args.data_path, args.dim_data, data);

            // Populate the Graph
            for (auto& v : vectors){
                DG.createNode(v);
            }
            
            // Start the timer and create the index using vamanaAlgorithm
            chrono::milliseconds startTime = chrono::high_resolution_clock::now();
            DG.stitchedVamanaAlgorithm(arguments.a, arguments.Lsmall, arguments.Rsmall, arguments.R); // TODO
            chrono::milliseconds endTime = chrono::high_resolution_clock::now();

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

        unordered_set<Id> starting_nodes = (unordered_set<Id>) {findMedoids(args.threshold)[q.category]};   // because each node belongs to at most one category.
        queryNeighbors = this->filteredGreedySearch(starting_nodes, q, args.k, args.L).first;
    }

    // Return the set containing the query's neighbors
    return queryNeighbors;
}

template <typename T>
vector<unordered_set<Id>> DirectedGraph<T>::findQueriesNeighbors(string queries_path, int read_arg){

    // Read queries
    vector<vector<float>> queries;
    
    // Check index type
    if(args.index_type == VAMANA){
        // If default value
        if(read_arg == -1)
            read_arg == args.n_queries;
        
        // Read query vectors file
        read_vecs<float>(queries_path, read_arg);
    }else{
        // If default value
        if(read_arg == -1)
            read_arg = args.dim_query;
        // Read query vectors file
        ReadBin(queries_path, read_arg, queries);
    }

    vector<unordered_set<Id>> returnVec;

    // Iterate through the queries
    for(Query<T>& q : queries){
        // Append each query's neighbors to the vector
        returnVec.push_back(findNeighbors(q));
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

        // Open the file and read its contents
        fstream file;
        file.open(args.groundtruth_path, ios::in);
        
        // Read the file's contents
        file >> groundtruth;

        // Close the file
        file.close();
    }

    vector<unordered_set<Id>> queriesNeighbors = DG.findQueriesNeighbors(args.queries_path);
    float total_recall = 0.f, query_recall;

    for(int i = 0; i < args.n_queries; i++){
        // Calculate the current recall and add it to the sum of all recall scores
        query_recall = k_recall(queriesNeighbors[i], groundtruth[i]);
        c_log << "Recall score for query: " << i+1 << "/" << args.n_queries << ":\t\t" << query_recall << "\n";
        total_recall += query_recall;
    }

    return total_recall / args.n_queries;

}