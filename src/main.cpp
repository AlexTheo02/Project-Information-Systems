#include "interface.hpp"
using namespace std;

int main (int argc, char* argv[]) {

    args.parseArgs(argc,argv);

    args.printArgs();

    // all data points in our datasets are vector<float> so the graph is initialized the same for all cases (euclidean distance and vector empty)
    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);
    
    // Create the indexed graph if instructed from command line arguments, based on indexing type
    chrono::milliseconds duration; 
    if (args.createIndex)
        duration = createIndex(DG,args);
        
    // Load graph if instructed from command line arguments
    else
        DG.load(args.graph_load_path);

    // Store graph if instructed from command line arguments
    DG.store(args.graph_store_path);

    // Perform queries on the graph and calculate average recall score
    float averageRecall = evaluateIndex(DG,args);
    

    return 0;
}