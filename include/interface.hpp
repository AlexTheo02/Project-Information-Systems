#pragma once

#include "config.hpp"
#include "util.hpp"
#include "types.hpp"
#include "id.hpp"
#include "node_and_query.hpp"
#include "graph_implementation.hpp"
#include "filtered_graph_implementation.hpp"
#include "evaluation.hpp"


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

// Evaluate given index based on its types and return average recall score
template <typename T>
float evaluateIndex(DirectedGraph<T>& DG, Args arguments){

    float total_recall = 0.0f;
    // Find neighbors for each query
    for (int i=0; i<arguments.n_queries; i++){

    }

}