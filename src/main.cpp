#include "interface.hpp"
using namespace std;

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
    
    // Create the indexed graph if instructed from command line arguments, based on indexing type
    if (args.createIndex)
        createIndex(DG,args);
        
    // Load graph if instructed from command line arguments
    else
        DG.load(args.graph_load_path);

    // Store graph if instructed from command line arguments
    DG.store(args.graph_store_path);

    // Perform queries on the graph and calculate average recall score
    float averageRecall = evaluateIndex(DG,args);
    

    return 0;
}