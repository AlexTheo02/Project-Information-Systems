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


// specific read_query function for .vecs format (only for unfiltered queries. Return format is used to match read_queries_bin_contest function)
template <typename T>
pair<vector<Query<T>>, vector<Query<T>>> read_queries_vecs(void){

    vector<T> queries_raw = read_vecs<float>(args.queries_path, args.n_queries);
    vector<Query<T>> unfiltered_queries;

    for (int i = 0; i < queries_raw.size(); i++){
        Query<T> q(i, -1, false, queries_raw[i], vectorEmpty<float>);
        unfiltered_queries.push_back(q);
        unfilteredQueryIndices.push_back(i);
    }

    pair<vector<Query<T>>, vector<Query<T>>> queries;
    queries.first = unfiltered_queries;

    return queries;
};

// specific read_query function for .bin format with specifications as described in the SIGMOD 2024 contest
template <typename T>
pair<vector<Query<T>>, vector<Query<T>>> read_queries_bin_contest(void){
    vector<T> queries_raw;
    ReadBin(args.queries_path, args.dim_query, ref(queries_raw));

    vector<Query<T>> unfiltered_queries;
    vector<Query<T>> filtered_queries;

    for (int i = 0; i < queries_raw.size(); i++){
        T query_value(queries_raw[i].begin() + 4, queries_raw[i].end());

        if(args.unfiltered){
            if (queries_raw[i][0] == 0){   // get only the unfiltered queries.
                Query<T> q(i, -1, false, query_value, vectorEmpty<float>);
                unfiltered_queries.push_back(q);
                unfilteredQueryIndices.push_back(i);
            }
        }
        if(args.filtered){
            if (queries_raw[i][0] == 1){   // get only the filtered queries.
                Query<T> q(i, queries_raw[i][1], true, query_value, vectorEmpty<float>);
                filtered_queries.push_back(q);
                filteredQueryIndices.push_back(i);
            }
        }
    }
    args.n_queries = unfiltered_queries.size() + filtered_queries.size();        // size update to account for discarded queries
    args.n_groundtruths = unfiltered_queries.size() + filtered_queries.size();   // same as above

    pair<vector<Query<T>>, vector<Query<T>>> queries;
    queries.first = unfiltered_queries;
    queries.second = filtered_queries;

    return queries;
};

// This function reads the args.queries_path file and returns a vector of well constructed queries depending on the file format.
template <typename T>
pair<vector<Query<T>>, vector<Query<T>>> read_queries(void){
    // Implement this function for your own data type.
    // This function is to be passed in the index evaluation function.
};

bool endsWith(const string& str, const string& suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// Creates the index on the graph based on the indexing type and return the duration in microseconds
template <typename T>
chrono::microseconds createIndex(DirectedGraph<T>& DG){
    chrono::microseconds duration = (chrono::microseconds) 0;
    chrono::high_resolution_clock::time_point startTime, endTime;

    vector<vector<float>> data;
    ifstream data_file(args.data_path);

    switch (args.index_type){
        case VAMANA:
            // Read base vectors file
            if(endsWith(args.data_path, ".bin")){
                ReadBin(args.data_path, args.dim_data, data);
            }
            else{
                data = read_vecs<float>(args.data_path, args.n_data);
            }

            // Populate the Graph
            for (auto& v : data){
                if (args.unfiltered && !args.data_is_unfiltered){
                    v = T(v.begin() + 2, v.end());
                }
                DG.createNode(v);
            }

            // Start the timer and create the index using vamanaAlgorithm
            startTime = chrono::high_resolution_clock::now();
            DG.vamanaAlgorithm(args.L, args.R, args.a);
            endTime = chrono::high_resolution_clock::now();

            // Calculate duration
            duration = chrono::duration_cast<chrono::microseconds>(endTime - startTime);
            
            break;

        case FILTERED_VAMANA:

            // Read data
            if(endsWith(args.data_path, ".bin")){
                ReadBin(args.data_path, args.dim_data, data);
            }else{
                if(!data_file.is_open()){
                    cout << "Could not open file\n";
                    exit(EXIT_FAILURE);
                }

                data_file >> data;
                data_file.close();
            }

            // Populate the Graph
            for (const T& value : data){
                int category = value[0];
                // Ignore the first 2 dimensions when finding the value
                T newValue(value.begin() + 2, value.end());
                DG.createNode(newValue, category);
            }

            // Start the timer and create the index using vamanaAlgorithm
            startTime = chrono::high_resolution_clock::now();
            DG.filteredVamanaAlgorithm(args.L, args.R, args.a, args.threshold);
            endTime = chrono::high_resolution_clock::now();

            // Calculate duration
            duration = chrono::duration_cast<chrono::microseconds>(endTime - startTime);

            break;

        case STITCHED_VAMANA:

            // Read data
            if(endsWith(args.data_path, ".bin")){
                ReadBin(args.data_path, args.dim_data, data);
            }else{
                if(!data_file.is_open()){
                    cout << "Could not open file\n";
                    exit(EXIT_FAILURE);
                }

                data_file >> data;
                data_file.close();
            }

            // Populate the Graph
            for (const T& value : data){
                int category = value[0];
                // Ignore the first 2 dimensions when finding the value
                T newValue(value.begin() + 2, value.end());
                DG.createNode(newValue, category);
            }
            
            // Start the timer and create the index using vamanaAlgorithm
            startTime = chrono::high_resolution_clock::now();
            DG.stitchedVamanaAlgorithm(args.L, args.R, args.Rsmall, args.a);
            endTime = chrono::high_resolution_clock::now();

            // Calculate duration
            duration = chrono::duration_cast<chrono::microseconds>(endTime - startTime);

            break;
        
        default:
            break;
    }
    return duration;
}

// Based on the qiven vector, the function returns the query's neighbors
template <typename T>
unordered_set<Id> DirectedGraph<T>::findNeighbors(Query<T> q){
    // Set for storing the query's neighbors
    unordered_set<Id> queryNeighbors;
    
    // Check index_type
    if(args.index_type == VAMANA){
        queryNeighbors = this->greedySearch(this->startingNode(), q.value, args.k, args.L).first;
    }
    else if(args.index_type == FILTERED_VAMANA || args.index_type == STITCHED_VAMANA){                      // in both cases we call filteredGreedySearch
        if (q.category != -1){  // filtered query case
            queryNeighbors = this->filteredGreedySearch(this->startingNode(q.category), q, args.k, args.L).first;
        }
        else{   // unfiltered query: get K neighbors of each category and get the closest K of the union of the neighbors of each category

            if (!args.accumulateUnfiltered){
                queryNeighbors = this->filteredGreedySearch(this->startingNode(), q, args.k, args.L).first; // CONSIDER OPTIMIZATIONS FOR UNFILTERED QUERIES STARTING NODE - TODO
            }
            else {  // costs |C| * findNeighbors + O(|C|*100) (nth element)
                for (pair<int, unordered_set<Id>> cpair : this->categories){                    // unfiltered query => find K neighbors in all categories
                    unordered_set<Id> queryNeighbors_f;                                         // and take the best K of all n_categories*K neighbor candidates
                    q.category = cpair.first;                                                   // update the category of the query
                    queryNeighbors_f = findNeighbors(q);                             // find the neighbors of that category
                    queryNeighbors.insert(queryNeighbors_f.begin(), queryNeighbors_f.end());    // insert them in the neighbor-candidate set
                }
                queryNeighbors = this->_closestN(args.k, queryNeighbors, q.value);         // keep the closest K neighbors of all neighbor candidates
            }
        }
    }
    
    // Return the set containing the query's neighbors
    return queryNeighbors;
}

// Thread function for parallel querying.
template <typename T>
void DirectedGraph<T>::_thread_findQueryNeighbors_fn(vector<Query<T>>& queries, mutex& mx_query_index, int& query_index, vector<unordered_set<Id>>& returnVec){
    mx_query_index.lock();
    while(query_index < queries.size()){
        int my_q_index = query_index++;     // store current and increment
        mx_query_index.unlock();

        returnVec[my_q_index] = findNeighbors(queries[my_q_index]);

        mx_query_index.lock();
    }
    mx_query_index.unlock();
}

// Returns the neighbors of all queries found in the given queries_path file.
// If the file is .vecs format read_arg corresponds to the number of queries and, if the file is in .bin format, it corresponds to the dimension of the query vector
template <typename T>
vector<unordered_set<Id>> DirectedGraph<T>::findQueriesNeighbors(vector<Query<T>> queries){

    c_log << "In Queries Neighbors" << '\n';
    

    vector<unordered_set<Id>> returnVec(args.n_queries);
    vector<thread> threads;
    mutex mx_query_index;
    int query_index = 0;

    if (args.index_type == FILTERED_VAMANA || STITCHED_VAMANA && !args.randomStart){
        this->findMedoids(args.threshold);    // compute the medoids to ensure the dictionary is complete and won't be resized/rehashed, invalidating any references of other threads.
    }

    // load and launch threads.
    for (int i = 0; i < args.n_threads; i++){
        threads.push_back(thread(&DirectedGraph::_thread_findQueryNeighbors_fn, this, ref(queries), ref(mx_query_index), ref(query_index), ref(returnVec)));
    }
    
    // collect the threads
    for (thread& th : threads)
        th.join();
   
    // Return the vector
    return returnVec;

}

// Evaluate given index based on its types and return a pair containing average recall score and duration
template <typename T>
pair<pair<float, chrono::microseconds>, pair<float, chrono::microseconds>> evaluateIndex(DirectedGraph<T>& DG, function<pair<vector<Query<T>>,vector<Query<T>>>(void)> readQueries){

    chrono::microseconds duration = (chrono::microseconds) 0;
    chrono::high_resolution_clock::time_point startTime, endTime;

    // greedySearchCount = 0;
    // greedySearchMode = 1;

    vector<vector<Id>> groundtruth;
    // If the groundtruth path is given, then read the specified file
    if(args.groundtruth_path != ""){

        if(args.index_type == VAMANA && !endsWith(args.groundtruth_path, ".txt"))
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

    // Read queries
    pair<vector<Query<T>>, vector<Query<T>>> queries = readQueries();

    // Start the timer
    startTime = chrono::high_resolution_clock::now();
    vector<unordered_set<Id>> queriesNeighbors = DG.findQueriesNeighbors(queries.first);
    // End the timer
    endTime = chrono::high_resolution_clock::now();

    // Calculate duration
    duration = chrono::duration_cast<chrono::microseconds>(endTime - startTime);

    // Return var that contains the a pair of recall scores and duration of each query category(unfiltered and filtered)
    pair<pair<float, chrono::microseconds>, pair<float, chrono::microseconds>> ret;

    float total_recall = 0.f, query_recall;

    s_log << "--UNF_QUERIES--\n";
    // for(int u_query : unfilteredQueryIndices){
    for(int i = 0; i < unfilteredQueryIndices.size(); i++){
        int u_query = unfilteredQueryIndices[i];
        c_log << u_query << '\n';
        // Calculate the current recall and add it to the sum of all recall scores
        query_recall = k_recall(queriesNeighbors[i], groundtruth[u_query]);
        c_log << "Recall score for unfiltered query: " << u_query << "/" << unfilteredQueryIndices.size() << ":\t\t" << query_recall << "\n";
        // query index, query recall, time to execute, filtered
        s_log << query_recall << '\n';
        total_recall += query_recall;
    }
    s_log << "--UNF_QUERIES_END--\n";

    ret.first.first = total_recall / unfilteredQueryIndices.size();
    ret.first.second = duration;

    // Start the timer
    startTime = chrono::high_resolution_clock::now();
    queriesNeighbors = DG.findQueriesNeighbors(queries.second);
    // End the timer
    endTime = chrono::high_resolution_clock::now();

    // Calculate duration
    duration = chrono::duration_cast<chrono::microseconds>(endTime - startTime);

    total_recall = 0.f;
    
    s_log << "--F_QUERIES--\n";
    for(int i = 0; i < filteredQueryIndices.size(); i++){
        int f_query = filteredQueryIndices[i];
        // Calculate the current recall and add it to the sum of all recall scores
        query_recall = k_recall(queriesNeighbors[i], groundtruth[f_query]);
        c_log << "Recall score for filtered query: " << f_query << "/" << filteredQueryIndices.size() << ":\t\t" << query_recall << "\n";
        // query index, query recall, time to execute, filtered
        s_log << query_recall << '\n';
        total_recall += query_recall;
    }
    s_log << "--F_QUERIES_END--\n";
    

    ret.second.first = total_recall / filteredQueryIndices.size();
    ret.second.second = duration;

    return ret;
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
        c_log << "Generating groundtruth for query: " << i << '\n';

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