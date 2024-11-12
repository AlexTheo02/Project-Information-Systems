#include "interface.hpp"
using namespace std;

vector<vector<Id>> generateGroundtruth(vector<vector<float>>& data, vector<vector<float>>& queries){

    // Create map category -> set of pair<nodeId, value>
    unordered_map<int, unordered_map<Id,vector<float>>> categories;
    for (int i=0; i<data.size(); i++){
        vector<float> vec = data[i];
        int category = vec[0];
        vector<float> value(vec.begin() + 2, vec.end());
        pair<Id, vector<float>> nodePair(i,value);
        categories[category].insert(nodePair);
    }

    // Initialize neighbors vector
    vector<vector<Id>> queryNeighbors;

    // For each query
    for(int i = 0; i < queries.size(); i++){
        cout << "Generating groundtruth for query: " << i << endl;

        vector<float> query = queries[i];
        vector<float> queryValue(query.begin() + 4, query.end());
        int category = query[1];

        float distance;
        unordered_map <Id, float> distances; 

        // Query is filtered
        if (category != -1){
            // Iterate over same-category nodes and calculate the distance for each of them
            for (pair<Id, vector<float>> vecPair : categories[category]){
                distances[vecPair.first] = euclideanDistance(queryValue,vecPair.second);
            }
        }
        // Query is unfiltered 
        else{
            // Iterate over all nodes and calculate the distance for each of them
            for (int i=0; i<data.size(); i++){
                vector<float> vec = data[i];
                vector<float> value(vec.begin() + 2, vec.end());
                distances[i] = euclideanDistance(queryValue, value);
            }
        }

        // Convert distances map to a vector of pairs
        vector<pair<Id, float>> distancesVec(distances.begin(), distances.end());

        // Sort distancesVec by the second element (distance) in ascending order
        sort(distancesVec.begin(), distancesVec.end(),
             [](const pair<Id, float>& a, const pair<Id, float>& b) {
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

// Παραδοτέο 1
void unfilteredVamana(){
    // Instantiate a DirectedGraph object DG
    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);

    // Read base vectors file
    vector<vector<float>> vectors = read_vecs<float>(args.data_path, args.n_data);

    // Create map key: vector, value: id (for groundtruth)
    map<vector<float>, int> v2id;
    for (int i=0; i<vectors.size(); i++){
        v2id[vectors[i]] = i;
    }

    // Read query vectors file
    vector<vector<float>> queries = read_vecs<float>(args.queries_path, args.n_queries);

    // Read groundtruth file
    vector<vector<int>> groundtruth = read_vecs<int>(args.groundtruth_path, args.n_groundtruths);

    // Populate the Graph
    for (auto& v : vectors){
        DG.createNode(v);
    }

    // MOVE TO PRINT FUNCTION

    // cout << "Running unfiltered main program with:" << endl;
    // cout << "L: " << L << endl;
    // cout << "R: " << R << endl;
    // cout << "a: " << a << endl;
    // cout << "k: " << k << endl;
    // cout << "Number of threads: " << args.n_threads << endl;
    // if (args.debug_mode) { cout << "Debug mode" << endl; }

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // profileUnfilteredVamana(L,R,a,k,DG,vectors,queries,groundtruth); // creating the vamana index and querying the groundtruths

    DG.store(args.graph_store_path); // store the vamana Index
}

// Παραδοτέο 2
void filteredVamana(){
    // Read data
    vector<vector<float>> data;
    ReadBin(args.data_path, args.dim_data, data);

    // Read queries
    vector<vector<float>> queries;
    ReadBin(args.queries_path, args.dim_query, queries);

    // Generate groundtruth
    fstream file;
    
    // Generate and store groundtruth
    // file.open(F_GROUNDTRUTH_PATH, ios::out | ios::trunc);  // mode flags: https://cplusplus.com/reference/fstream/fstream/open/
    // vector<vector<Id>> groundtruth = generateGroundtruth(data, queries);
    // file << groundtruth;

    // Read groundtruth
    file.open(args.groundtruth_path, ios::in);
    vector<vector<Id>> groundtruth;
    file >> groundtruth;

    file.close();

    // Create and initialize graph
    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>, data, true);

    // cout << "Running filtered main program with:" << endl;
    // cout << "L: " << L << endl;
    // cout << "R: " << R << endl;
    // cout << "a: " << a << endl;
    // cout << "k: " << k << endl;
    // cout << "t: " << t << endl;
    // cout << "Number of threads: " << args.n_threads << endl;
    // if (args.debug_mode) { cout << "Debug mode" << endl; }

    // profileFilteredVamana(L,R,a,k,t,DG,data,queries,groundtruth,F_GRAPH_STORE_PATH);

}

int main (int argc, char* argv[]) {

    args.parseArgs(argc,argv);

    args.printArgs();

    // all data points in our datasets are vector<float> so the graph is initialized the same for all cases (euclidean distance and vector empty)
    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);

    // Load graph if instructed from command line arguments
    DG.load(args.graph_load_path);
    
    // Reindex the graph if instructed from command line arguments, based on indexing type
    if (args.reindex){
        reindex(DG,args);
    }

    // Store graph if instructed from command line arguments
    DG.store(args.graph_store_path);

    // Perform queries on the graph and calculate average recall score
    float averageRecall = evaluateIndex(DG,args);
    

    return 0;
}