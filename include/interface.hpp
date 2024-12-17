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


// specific read_query function for .vecs format
template <typename T>
vector<Query<T>> read_queries_vecs(void){

    vector<T> queries_raw = read_vecs<float>(args.queries_path, args.n_queries);
    vector<Query<T>> queries;

    for (int i = 0; i < queries_raw.size(); i++){
        Query<T> q(i, -1, false, queries_raw[i], vectorEmpty<float>);
        queries.push_back(q);
    }
    return queries;
};

// specific read_query function for .bin format with specifications as described in the SIGMOD 2024 contest - [insert link]
template <typename T>
vector<Query<T>> read_queries_bin_contest(void){
    vector<T> queries_raw;
    ReadBin(args.queries_path, args.dim_query, ref(queries_raw));

    vector<Query<T>> queries;

    for (int i = 0; i < queries_raw.size(); i++){
        T query_value(queries_raw[i].begin() + 4, queries_raw[i].end());

        if (args.crop_filters){
            // create unfiltered query from filtered query data

            if (queries_raw[i][0] == 0){   // get only the unfiltered queries. Discard filtered queries.
                Query<T> q(i, -1, false, query_value, vectorEmpty<float>);
                queries.push_back(q);
            }
            // keep the indices of the discarded queries to not consider them during groundtruth evaluation
            else{
                args.__discardedQueryIndices.push_back(i);  // discarding queries of type 1 (because of --crop_filters), 2 or 3 (timestamps)
            }
        }
        else if (args.crop_unfiltered){
            // create unfiltered query from filtered query data

            if (queries_raw[i][0] == 1){   // get only the unfiltered queries. Discard filtered queries.
                Query<T> q(i, 1, true, query_value, vectorEmpty<float>);
                queries.push_back(q);
            }
            // keep the indices of the discarded queries to not consider them during groundtruth evaluation
            else{
                args.__discardedQueryIndices.push_back(i);  // discarding queries of type 1 (because of --crop_filters), 2 or 3 (timestamps)
            }
        }
        else{

            if (queries_raw[i][0] == 0 || queries_raw[i][0] == 1){
                Query<T> q(i, queries_raw[i][1], queries_raw[i][0], query_value, vectorEmpty<float>);
                queries.push_back(q);
            }
            else{
                args.__discardedQueryIndices.push_back(i);  // discarding queries of type 2 or 3 (timestamps)
            }
        }
    }
    args.n_queries = queries.size();        // size update to account for discarded queries
    args.n_groundtruths = queries.size();   // same as above

    return queries;
};

// This function reads the args.queries_path file and returns a vector of well constructed queries depending on the file format.
template <typename T>
vector<Query<T>> read_queries(void){
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
                if (args.crop_filters){
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
            DG.stitchedVamanaAlgorithm(args.L, args.R, args.Lsmall, args.Rsmall, args.a);
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
void DirectedGraph<T>::_thread_findQueryNeighbors_fn(vector<Query<T>>& queries, mutex& mx_query_index, int& query_index, vector<pair<unordered_set<Id>, pair<chrono::microseconds, bool>>>& returnVec){
    mx_query_index.lock();
    while(query_index < queries.size()){
        int my_q_index = query_index++;     // store current and increment
        mx_query_index.unlock();

        chrono::high_resolution_clock::time_point start_time = chrono::high_resolution_clock::now();
        returnVec[my_q_index].first = findNeighbors(queries[my_q_index]);
        chrono::high_resolution_clock::time_point end_time = chrono::high_resolution_clock::now();
        returnVec[my_q_index].second.first = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
        returnVec[my_q_index].second.second = queries[my_q_index].filtered;

        mx_query_index.lock();
    }
    mx_query_index.unlock();
}

// Returns the neighbors of all queries found in the given queries_path file.
// If the file is .vecs format read_arg corresponds to the number of queries and, if the file is in .bin format, it corresponds to the dimension of the query vector
template <typename T>
vector<pair<unordered_set<Id>, pair<chrono::microseconds, bool>>> DirectedGraph<T>::findQueriesNeighbors(function<vector<Query<T>>(void)> readQueries){

    c_log << "In Queries Neighbors" << '\n';
    // Read queries
    vector<Query<T>> queries = readQueries();

    vector<pair<unordered_set<Id>, pair<chrono::microseconds, bool>>> returnVec(args.n_queries);
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
pair<float, chrono::microseconds> evaluateIndex(DirectedGraph<T>& DG, function<vector<Query<T>>(void)> readQueries){

    chrono::microseconds duration = (chrono::microseconds) 0;
    chrono::high_resolution_clock::time_point startTime, endTime;

    greedySearchCount = 0;
    greedySearchMode = 1;

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

    // Start the timer
    startTime = chrono::high_resolution_clock::now();
    vector<pair<unordered_set<Id>, pair<chrono::microseconds, bool>>> queriesNeighbors = DG.findQueriesNeighbors(readQueries);
    // End the timer
    endTime = chrono::high_resolution_clock::now();

    // Calculate duration
    duration = chrono::duration_cast<chrono::microseconds>(endTime - startTime);

    float total_recall = 0.f, query_recall;

    // discard appropriate query answers from groundtruths
    int deleted = 0;
    for (int indexToRemove : args.__discardedQueryIndices){
        groundtruth.erase(groundtruth.begin() + indexToRemove - deleted++); // assumming __discardedQueryIndices are sorted in ascending order, and contain valid and corresponding indices with queries.
    }
    s_log << "--QUERIES--\n";
    s_log<<"index,recall_score,duration,filtered\n";
    for(int i = 0; i < args.n_queries; i++){
        // Calculate the current recall and add it to the sum of all recall scores
        query_recall = k_recall(queriesNeighbors[i].first, groundtruth[i]);
        c_log << "Recall score for query: " << i+1 << "/" << args.n_queries << ":\t\t" << query_recall << "\n";
        // query index, query recall, time to execute, filtered
        s_log << i << "," << query_recall << "," << FormatMicroseconds(queriesNeighbors[i].second.first) << "," << queriesNeighbors[i].second.second << '\n';
        total_recall += query_recall;
    }
    s_log << "--QUERIES_END--\n";

    pair<float, chrono::microseconds> ret;
    ret.first = total_recall / args.n_queries;
    ret.second = duration;

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