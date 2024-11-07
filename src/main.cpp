#include "interface.hpp"
using namespace std;

static int L;
static int R;
static float a;
static int k;
static float t;

void parseArgs(int argc, char*argv[]){

    // Check number of arguments
    if (argc > 14){ throw invalid_argument("Invalid number of command line arguments\n"); }

    // Load default values
    L = DEFAULT_L;
    R = DEFAULT_R;
    a = DEFAULT_a;
    k = DEFAULT_k;
    N_THREADS = DEFAULT_N_THREADS;
    t = DEFAULT_t;
    SHOULD_OMIT = DEFAULT_SHOULD_OMIT;

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
        else { throw invalid_argument("Invalid command line arguments"); }
    }
}


int main (int argc, char* argv[]) {

    // Parse command line arguments
    parseArgs(argc, argv);

    // vector<vector<float>> data;

    // ReadBin("data/contest-queries-release-1m.bin", 104, data);

    // c_log << data << "\n";

    

    // Instantiate a DirectedGraph object DG
    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);

    // Read base vectors file
    vector<vector<float>> vectors = read_vecs<float>("data/siftsmall/siftsmall_base.fvecs", 10000);

    // Create map key: vector, value: id (for groundtruth)
    map<vector<float>, int> v2id;
    for (int i=0; i<vectors.size(); i++){
        v2id[vectors[i]] = i;
    }

    // Read query vectors file
    vector<vector<float>> queries = read_vecs<float>("data/siftsmall/siftsmall_query.fvecs", 100);

    // Read groundtruth file
    vector<vector<int>> groundtruth = read_vecs<int>("data/siftsmall/siftsmall_groundtruth.ivecs", 100);

    // Populate the Graph
    for (auto& v : vectors){
        DG.createNode(v);
    }

    cout << "Running main program with:" << endl;
    cout << "L: " << L << endl;
    cout << "R: " << R << endl;
    cout << "a: " << a << endl;
    cout << "k: " << k << endl;
    cout << "Number of threads: " << N_THREADS << endl;
    if (!SHOULD_OMIT) { cout << "Debug mode" << endl; }

    profileVamana(L,R,a,k,DG,vectors,queries,groundtruth,v2id); // creating the vamana index and querying the groundtruths

    DG.store("graph_instance.txt"); // store the vamana Index

    return 0;
}