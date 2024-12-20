#include "interface.hpp"
using namespace std;

DirectedGraph<vector<float>>* DGptr = nullptr;

int main(int argc, char* argv[]) {

    args.parseArgs(argc,argv);

    args.printArgs();

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
    if (!args.no_create){
        chrono::microseconds duration;
        duration = createIndex(DG);
        cout << "Time to create the index: " << FormatMicroseconds(duration) << endl;
        s_log << "Number of edges: " << DG.get_n_edges() << "\n";
    }
        
    // Load graph if instructed from command line arguments
    else
        DG.load(args.graph_load_path);

    c_log << "Index is ready\n";
    // Store graph if instructed from command line arguments

    DG.store(args.graph_store_path);

    // If instructed to not perform queries, stop execution here
    if (args.no_query) { return 0; }


    // Perform queries on the graph and calculate average recall score
    time_t time_now = time(NULL); // get current time
    struct tm * timeinfo;
    timeinfo = localtime(&time_now);

    c_log << "Starting index evaluation on "<< asctime(timeinfo); // https://cplusplus.com/reference/ctime/localtime/, https://cplusplus.com/reference/ctime/time/
    pair<pair<float, chrono::microseconds>, pair<float, chrono::microseconds>> results = evaluateIndex<vector<float>>(ref(DG), (args.index_type == VAMANA && !endsWith(args.queries_path, ".bin")) ? read_queries_vecs<vector<float>> : read_queries_bin_contest<vector<float>>);

    // print recall and duration
    c_log << "Evaluation Finished.\n";

    if(args.unfiltered){
        cout << "Time to query the index (unfiltered): " << FormatMicroseconds(results.first.second) << endl;
        cout << "Average recall score for unfiltered queries: " << results.first.first << endl;
    }
    
    if(args.filtered){
        cout << "Time to query the index (filtered): " << FormatMicroseconds(results.second.second) << endl;
        cout << "Average recall score for filtered queries: " << results.second.first << endl;
    }
    
    return 0;
}