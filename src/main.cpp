#include "interface.hpp"
using namespace std;

DirectedGraph<vector<float>>* DGptr = nullptr;

int main(int argc, char* argv[]) {

    args.parseArgs(argc,argv);

    args.printArgs();

    int placeholder_arg = 0;


    // choose distance function depending on the argument
    if (args.euclideanType == 0){
        DGptr = new DirectedGraph<vector<float>>(euclideanDistance<vector<float>>, vectorEmpty<float>);
    }
    else if (args.euclideanType == 1){
        DGptr = new DirectedGraph<vector<float>>(simd_euclideanDistance, vectorEmpty<float>);
    }
    else if (args.euclideanType == 2){
        DGptr = new DirectedGraph<vector<float>>(parallel_euclideanDistance<vector<float>>, vectorEmpty<float>);
    }
    else { throw invalid_argument("distance must be in {0,1,2}. | 0 - euclidean, 1 - simd_euclidean, 2 - parallel euclidean"); }
    
    // dereferencing the pointer (functions were built expecting a reference and not a pointer. Changing them would be too much unnecessary work)
    DirectedGraph<vector<float>> DG = *DGptr;

    
    // Create the indexed graph if instructed from command line arguments, based on indexing type
    chrono::milliseconds duration; 
    if (args.createIndex){
        duration = createIndex(DG);
        cout << "Time to create the index: "; printFormatMiliseconds(duration);

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
    time_t time_now = time(NULL); // get current time
    struct tm * timeinfo;
    timeinfo = localtime(&time_now);

    cout << "Starting index evaluation on "<< asctime(timeinfo); // https://cplusplus.com/reference/ctime/localtime/, https://cplusplus.com/reference/ctime/time/
    pair<float, chrono::milliseconds> results = evaluateIndex<vector<float>>(ref(DG), (args.index_type == VAMANA) ? read_queries_vecs<vector<float>> : read_queries_bin_contest<vector<float>>);

    // print recall and duration
    cout << "Evaluation Finished." << endl;
    cout << "Time to query the index: "; printFormatMiliseconds(results.second);
    cout << "Average recall score: " << results.first << endl;

    return 0;
}