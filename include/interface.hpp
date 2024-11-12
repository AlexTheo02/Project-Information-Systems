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

// Reindex the graph based on the indexing type and return the duration in miliseconds
template <typename T>
chrono::milliseconds reindex(DirectedGraph<T>& DG, Args arguments){
    chrono::milliseconds duration = 0;
    switch (arguments.index_type){
        case VAMANA:
            /* code */
            break;

        case FILTERED_VAMANA:
            /* code */
            break;

        case STITCHED_VAMANA:
            /* code */
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