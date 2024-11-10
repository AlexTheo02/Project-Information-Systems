#include "interface.hpp"
using namespace std;

static int L;
static int R;
static float a;
static int k;
static float t;

#define DATA_PATH "data/siftsmall/siftsmall_base.fvecs"
#define QUERIES_PATH "data/siftsmall/siftsmall_query.fvecs"
#define GROUNDTRUTH_PATH "data/siftsmall/siftsmall_groundtruth.ivecs"
#define N_DATA 10000
#define N_QUERIES 100
#define GRAPH_STORE_PATH "graph_instance.txt"

#define F_DATA_PATH "data/contest-data-release-1m.bin"
#define F_QUERIES_PATH "data/contest-queries-release-1m.bin"
#define F_DATA_DIM 102
#define F_QUERY_DIM 104
#define F_GRAPH_STORE_PATH "filtered_graph_instance.txt"
#define F_GROUNDTRUTH_PATH "data/contest-groundtruth-custom-1m.txt"

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
    vector<vector<float>> vectors = read_vecs<float>(DATA_PATH, N_DATA);

    // Create map key: vector, value: id (for groundtruth)
    map<vector<float>, int> v2id;
    for (int i=0; i<vectors.size(); i++){
        v2id[vectors[i]] = i;
    }

    // Read query vectors file
    vector<vector<float>> queries = read_vecs<float>(QUERIES_PATH, N_QUERIES);

    // Read groundtruth file
    vector<vector<int>> groundtruth = read_vecs<int>(GROUNDTRUTH_PATH, N_QUERIES);

    // Populate the Graph
    for (auto& v : vectors){
        DG.createNode(v);
    }

    cout << "Running unfiltered main program with:" << endl;
    cout << "L: " << L << endl;
    cout << "R: " << R << endl;
    cout << "a: " << a << endl;
    cout << "k: " << k << endl;
    cout << "Number of threads: " << N_THREADS << endl;
    if (!SHOULD_OMIT) { cout << "Debug mode" << endl; }

    profileUnfilteredVamana(L,R,a,k,DG,vectors,queries,groundtruth); // creating the vamana index and querying the groundtruths

    DG.store(GRAPH_STORE_PATH); // store the vamana Index
}

// Παραδοτέο 2
void filteredVamana(){
    // Read data
    vector<vector<float>> data;
    ReadBin(F_DATA_PATH, F_DATA_DIM, data);

    // Read queries
    vector<vector<float>> queries;
    ReadBin(F_QUERIES_PATH, F_QUERY_DIM, queries);

    // Generate groundtruth
    fstream file;
    
    // Generate and store groundtruth
    // file.open(F_GROUNDTRUTH_PATH, ios::out | ios::trunc);  // mode flags: https://cplusplus.com/reference/fstream/fstream/open/
    // vector<vector<Id>> groundtruth = generateGroundtruth(data, queries);
    // file << groundtruth;

    // Read groundtruth
    file.open(F_GROUNDTRUTH_PATH, ios::in);
    vector<vector<Id>> groundtruth;
    file >> groundtruth;

    file.close();

    // Create and initialize graph
    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>, data, true);

    cout << "Running filtered main program with:" << endl;
    cout << "L: " << L << endl;
    cout << "R: " << R << endl;
    cout << "a: " << a << endl;
    cout << "k: " << k << endl;
    cout << "t: " << t << endl;
    cout << "Number of threads: " << N_THREADS << endl;
    if (!SHOULD_OMIT) { cout << "Debug mode" << endl; }

    profileFilteredVamana(L,R,a,k,t,DG,data,queries,groundtruth,F_GRAPH_STORE_PATH);

}
void parseArgs(int argc, char*argv[]){

    // Check number of arguments
    if (argc > 15){ throw invalid_argument("Invalid number of command line arguments\n"); }

    // Load default values
    L = DEFAULT_L;
    R = DEFAULT_R;
    a = DEFAULT_a;
    k = DEFAULT_k;
    N_THREADS = DEFAULT_N_THREADS;
    t = DEFAULT_t;
    SHOULD_OMIT = DEFAULT_SHOULD_OMIT;
    FILTERED = DEFAULT_FILTERED;

    // Iterate through given arguments
    string currentArg,nextArg;

    for (int i=1; i<argc; i++){
        currentArg = argv[i];

        if (currentArg == "-L"){ L = atoi(argv[++i]); }
        else if (currentArg == "-R"){ R = atoi(argv[++i]); }
        else if (currentArg == "-a"){ a = atof(argv[++i]); }
        else if (currentArg == "-k"){ k = atoi(argv[++i]); }
        else if (currentArg == "-P"){ N_THREADS = atoi(argv[++i]); }
        else if (currentArg == "-t"){ t = atof(argv[++i]); }
        else if (currentArg == "--debug"){ SHOULD_OMIT = false; }
        else if (currentArg == "--filtered"){ FILTERED = true; }
        else { throw invalid_argument("Invalid command line arguments"); }
    }
}


int main (int argc, char* argv[]) {
    parseArgs(argc, argv);

    if (FILTERED){
        filteredVamana();
    }
    else{
        unfilteredVamana();
    }

    return 0;
}