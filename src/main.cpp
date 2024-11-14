#include "interface.hpp"
using namespace std;

int main (int argc, char* argv[]) {

    args.parseArgs(argc,argv);

    args.printArgs();

    // all data points in our datasets are vector<float> so the graph is initialized the same for all cases (euclidean distance and vector empty)
    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);
    
    // Create the indexed graph if instructed from command line arguments, based on indexing type
    chrono::milliseconds duration; 
    if (args.createIndex){
        duration = createIndex(DG,args);

        // calculate the medoid to store (used as starting node for unfiltered queries)
        if (args.index_type == FILTERED_VAMANA || args.index_type == STITCHED_VAMANA){
            vector<Node<vector<float>>> medoids_vec;
            for (pair<int, Id> fmedoid : DG.findMedoids(args.threshold))    // already calculated in createIndex
                medoids_vec.push_back(DG.getNodes()[fmedoid.second]);
            DG.medoid(medoids_vec, true);
        }
    }
        
    // Load graph if instructed from command line arguments
    else
        DG.load(args.graph_load_path);

    cout << "Index is ready\n";
    // Store graph if instructed from command line arguments

    DG.store(args.graph_store_path);

    // Perform queries on the graph and calculate average recall score
    float averageRecall = evaluateIndex(DG,args);

    // print recall and duration
    cout << "Evaluation Finished." << endl;
    cout << "Time to create the index: "; printFormatMiliseconds(duration);
    cout << "Average recall score: " << averageRecall << endl;

    return 0;
}