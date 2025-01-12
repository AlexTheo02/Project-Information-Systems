#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <cstdlib>
#include <string>
#include <cstring>
#include <random>
#include <chrono>
#include <utility>
#include <type_traits>
#include <list>

using namespace std;

// This is the configuration file for this Project.
// Here you will find:
//
//      1. Global Constants
//      2. Global Definitions
//      3. Operator overloads
//      (4. Specialized Hash Functions for specific Types) - was used in a previous implementation, commented out
//
// For your specified datatype T, you should have the ostream << and istream >> operators overloaded. 
// static int greedySearchMode = 0;
// static int greedySearchCount = 0;

// enum for index type
enum IndexType {
    EMPTY,
    VAMANA,
    FILTERED_VAMANA,
    STITCHED_VAMANA
};

enum SortOrder {
    ASCENDING,
    DESCENDING
};

// Struct containing all possible arguments from the CLI.
// call Args::parseArgs with argc and argv to initialize the struct
struct Args{
    IndexType index_type = EMPTY;
    int k = -1;
    int L = -1;
    int R = -1;
    float a = -1;
    int n_threads = -1;             // parallel functions [LIST OF FUNCTIONS THAT CAN BE PARALLELIZED TODO] <------------------------------- TODO 
    float threshold = -1;
    bool debug_mode = false;        // c_log follows this flag (see c_log documentation for more)
    bool stat_mode = false;              // stat follows this flag (see stat documentation for more)
    int Rsmall = -1;
    int n_data = -1;
    int n_queries = -1;
    int n_groundtruths = -1;
    int dim_data = -1;
    int dim_query = -1;
    bool unfiltered = false;
    bool filtered = false;
    bool data_is_unfiltered = false;
    string graph_store_path = "";
    string graph_load_path = "";
    string data_path = "";
    string queries_path = "";
    string groundtruth_path = "";
    bool no_create = false;           // flag whether to create new vamana index using the vamana algorithm
    bool no_query = false;
    bool dummy = false;

    // arguments regarding optimization
    int euclideanType = 1;      // 0 - normal euclidean, 1 - simd euclidean, 2 - parallel euclidean, 3 - custom distance function
    bool randomStart = false;   // false = medoid, true = random sample
    bool usePQueue = false;     // false = Lc is set O(1) insertion, & use closestN O(N), true = Lc is a Pqueue, closest N is optimized but insertion is O(logL)
    bool useRGraph = true;      // true = Use Rgraph in Vamana, false = skip Random Initialization.
    int extraRandomEdges = 0;     // <=0 = don't add extra random edges <after index creation>, >0 = add them (after index creation because index creation assumes unique subgraphs)
    bool accumulateUnfiltered = false;  // uses accumulation and aggregation of |C| filtered queries for the final result (as if unfiltered = all filters)
    string greedySearchIndexStatsPath = "";
    string greedySearchQueryStatsPath = "";

    Args(){};   // default constructor - empty (values are loaded with Args::parseArgs method)

    void parseArgs(int argc, char* argv[]){

        if (argc < 2){ throw invalid_argument("Invalid number of command line arguments\n"); }

        // Iterate through given arguments
        for (int i = 1; i < argc; i++){
            string currentArg = argv[i];

            if (currentArg == "-k")                     { this->k = atoi(argv[++i]); }
            else if (currentArg == "-L")                { this->L = atoi(argv[++i]); }
            else if (currentArg == "-R")                { this->R = atoi(argv[++i]); }
            else if (currentArg == "-a")                { this->a = atof(argv[++i]); }
            else if (currentArg == "-t")                { this->threshold = atof(argv[++i]); }
            else if (currentArg == "--debug")           { this->debug_mode = true; }
            else if (currentArg == "--stat")            { this->stat_mode = true; }
            else if (currentArg == "-Rs")               { this->Rsmall = atoi(argv[++i]); }
            else if (currentArg == "-n_data")           { this->n_data = atoi(argv[++i]); }
            else if (currentArg == "-n_queries")        { this->n_queries = atoi(argv[++i]); }
            else if (currentArg == "-n_groundtruths")   { this->n_groundtruths = atoi(argv[++i]); }
            else if (currentArg == "-dim_data")         { this->dim_data = atoi(argv[++i]); }
            else if (currentArg == "-dim_query")        { this->dim_query = atoi(argv[++i]); }
            else if (currentArg == "--only_unfiltered") { this->unfiltered = true; }
            else if (currentArg == "--only_filtered")   { this->filtered = true; }
            else if (currentArg == "--data_unfiltered") { this->data_is_unfiltered = true; }

            else if (currentArg == "-store")            { this->graph_store_path = argv[++i]; }
            else if (currentArg == "-load")             { this->graph_load_path = argv[++i]; this->no_create = true; }

            else if (currentArg == "-data")             { this->data_path = argv[++i]; } 
            else if (currentArg == "-queries")          { this->queries_path = argv[++i]; }
            else if (currentArg == "-groundtruth")      { this->groundtruth_path = argv[++i]; }

            else if (currentArg == "--vamana")          { this->index_type = VAMANA; }
            else if (currentArg == "--filtered")        { this->index_type = FILTERED_VAMANA; }
            else if (currentArg == "--stitched")        { this->index_type = STITCHED_VAMANA; }

            else if (currentArg == "--no_create")       { this->no_create = true; }
            else if (currentArg == "--no_query")        { this->no_query = true; }
            
            else if (currentArg == "--dummy")           { this->dummy = true; }

            // optimizations
            else if (currentArg == "-n_threads")        { this->n_threads = atoi(argv[++i]); }
            else if (currentArg == "--random_start")    { this->randomStart = true; }
            else if (currentArg == "-distance")         { this->euclideanType = atoi(argv[++i]); }
            else if (currentArg == "--pqueue")          { this->usePQueue = true; }
            else if (currentArg == "--no_rgraph")       { this->useRGraph = false; }
            else if (currentArg == "-extra_edges")      { this->extraRandomEdges = atoi(argv[++i]); }
            else if (currentArg == "--acc_unfiltered")  { this->accumulateUnfiltered = true; }

            // evaluation
            else if (currentArg == "-collect_data_index")   { this->greedySearchIndexStatsPath = argv[++i]; }
            else if (currentArg == "-collect_data_query")   { this->greedySearchQueryStatsPath = argv[++i]; }


            else { throw invalid_argument("Invalid command line arguments"); }
        }

        // set default values on any missing arguments based on IndexType
        if (this->k == -1)          this->k = 100;
        if (this->L == -1)          this->L = 100;
        if (this->R == -1)          this->R = 14;
        if (this->a == -1)          this->a = 1.0f;
        if (this->n_threads == -1)  this->n_threads = 1;
        if (this->threshold == -1)  this->threshold = (this->index_type == VAMANA) ? 0.1 : 0.5f;
        if (this->Rsmall == -1)     this->Rsmall = 14;

        if (this->graph_load_path == "" && this->no_create) {
            throw invalid_argument("Please specify a load path when using --no_create using -load your/path/here");
        }

        if (this->dummy){
            this->n_data = 10000;
            this->n_queries = 10000;
            this->n_groundtruths = 10000;

            this->data_path = "./data/dummy/dummy-data.bin";
            this->queries_path = "./data/dummy/dummy-queries.bin";
            this->groundtruth_path = "./data/dummy/dummy-groundtruth.txt";

            this->dim_data = 102;
            this->dim_query = 104;
        }

        if(!this->unfiltered && !this->filtered){
            this->unfiltered = true;
            this->filtered = true;
        }

        if(this->data_is_unfiltered){
            this->unfiltered = true;
            this->filtered = false;
        }    

        if (this->index_type == VAMANA){

            if (this->n_data == -1)  this->n_data = 10000;                  // 10 000
            if (this->n_queries == -1)  this->n_queries = 100;              // 100
            if (this->n_groundtruths == -1)  this->n_groundtruths = 100;    // 100

            if (this->data_path == "") this->data_path = "data/siftsmall/siftsmall_base.fvecs";
            if (this->queries_path == "") this->queries_path = "data/siftsmall/siftsmall_query.fvecs";
            if (this->groundtruth_path == "") this->groundtruth_path = "data/siftsmall/siftsmall_groundtruth.ivecs";

            if (this->greedySearchIndexStatsPath == "") {
                if (this->usePQueue)
                    this->greedySearchIndexStatsPath =  "./evaluations/PQ_VAMANA_greedySearchIndexStats.csv";
                else
                    this->greedySearchIndexStatsPath =  "./evaluations/SET_VAMANA_greedySearchIndexStats.csv";
            }
            if (this->greedySearchQueryStatsPath == "") {
                if(this->usePQueue)
                    this->greedySearchQueryStatsPath =  "./evaluations/PQ_VAMANA_greedySearchQueryStats.csv";
                else
                    this->greedySearchQueryStatsPath = "./evaluations/SET_VAMANA_greedySearchQueryStats.csv";
            }
        }
        else if (this->index_type == FILTERED_VAMANA || this->index_type == STITCHED_VAMANA){

            if (this->n_data == -1)  this->n_data = 1000000;                // 1 000 000
            if (this->n_queries == -1)  this->n_queries = 10000;            // 10 000
            if (this->n_groundtruths == -1)  this->n_groundtruths = 10000;  // 10 000
            if (this->dim_data == -1) this->dim_data = 102;
            if (this->dim_query == -1) this->dim_query = 104;

            if (this->data_path == "") this->data_path = "data/contest-data-release-1m.bin";
            if (this->queries_path == "") this->queries_path = "data/contest-queries-release-1m.bin";
            if (this->groundtruth_path == "") this->groundtruth_path = "data/contest-groundtruth-custom-1m.txt";
            // Stitched
            if (this->index_type == STITCHED_VAMANA){
                this->R = 14;  // R = Rstitched
                if (this->greedySearchIndexStatsPath == "") {
                if (this->usePQueue)
                    this->greedySearchIndexStatsPath =  "./evaluations/PQ_STITCHED_greedySearchIndexStats.csv";
                else
                    this->greedySearchIndexStatsPath =  "./evaluations/SET_STITCHED_greedySearchIndexStats.csv";
                }
            if (this->greedySearchQueryStatsPath == "") {
                if(this->usePQueue)
                    this->greedySearchQueryStatsPath =  "./evaluations/PQ_STITCHED_greedySearchQueryStats.csv";
                else
                    this->greedySearchQueryStatsPath = "./evaluations/SET_STITCHED_greedySearchQueryStats.csv";
                }
            }
            // Filtered
            else{
                if (this->greedySearchIndexStatsPath == "") {
                if (this->usePQueue)
                    this->greedySearchIndexStatsPath =  "./evaluations/PQ_FILTERED_greedySearchIndexStats.csv";
                else
                    this->greedySearchIndexStatsPath =  "./evaluations/SET_FILTERED_greedySearchIndexStats.csv";
            }
            if (this->greedySearchQueryStatsPath == "") {
                if(this->usePQueue)
                    this->greedySearchQueryStatsPath =  "./evaluations/PQ_FILTERED_greedySearchQueryStats.csv";
                else
                    this->greedySearchQueryStatsPath = "./evaluations/SET_FILTERED_greedySearchQueryStats.csv";
            }
            }
        }
        else throw invalid_argument("You must specify the Index Type. Valid options: [--vamana, --filtered, --stitched]\n");
    }

    // Print argument values for each indexing type
    void printArgs(){
        if(this->debug_mode) cout << "------ Debug mode ------" << endl;
        cout << "Number of threads: " << this->n_threads << endl;
        cout << "Indexing Type: ";
        if(this->index_type == VAMANA) cout << "VAMANA" << endl;
        if(this->index_type == FILTERED_VAMANA) cout << "FILTERED_VAMANA" << endl;
        if(this->index_type == STITCHED_VAMANA) cout << "STITCHED_VAMANA" << endl;
        cout << "k: " << this->k << endl;
        cout << "L: " << this->L << endl;
        cout << "R: " << this->R << endl;
        cout << "a: " << this->a << endl;

        if(this->index_type == FILTERED_VAMANA) cout << "threshold: " << this->threshold << endl;

        if(this->index_type == STITCHED_VAMANA) cout << "Rsmall: " << this->Rsmall << endl;

        if (this->extraRandomEdges > 0) cout << "Number of extra random edges: " << this->extraRandomEdges << endl;
        if (this->unfiltered) cout << "Unfiltered" << endl;
        if (this->filtered) cout << "Filtered" << endl;
        if (this->accumulateUnfiltered) cout << "Accumulate unfiltered" << endl;
        if (this->usePQueue) cout << "Using priority queue" << endl;
        if (!this->useRGraph) cout << "Not using rgraph initialization" << endl;

    }
};

static Args args;
static vector<int> filteredQueryIndices;    // this vector contains the indices of the discarded filtered queries (the filtered queries of categories 2 and 3, or even filtered queries if --filtered was set)
static vector<int> unfilteredQueryIndices;    // this vector contains the indices of the discarded queries (the filtered queries of categories 2 and 3, or even filtered queries if --unfilered was set)

// Custom Cout-like Object that respects the args.debug_mode flag on whether to print or not.
// Cannot be used with endl. Please use << '\n'; instead of << endl;
// Can be chained with simple printable types: c_log << a << b << ... ;
class ConsoleLog{

    public:

    ConsoleLog(){};   // empty constructor

    // overloading the << operator
    template <typename T>
    ConsoleLog& operator<<(const T& t){
        if (args.debug_mode)
            cout << t;
        return *this;
    }
};
// instantiation of static object
static ConsoleLog c_log;    // each source file that includes config.hpp will have its own instance of c_log

// Custom Cout-like Object that respects the args.stat flag on whether to print or not.
// Cannot be used with endl. Please use << '\n'; instead of << endl;
// Can be chained with simple printable types: stat << a << b << ... ;
class StatLog{

    public:

    StatLog(){};   // empty constructor

    // overloading the << operator
    template <typename T>
    StatLog& operator<<(const T& t){
        if (args.stat_mode)
            cout << t;
        return *this;
    }
};
// instantiation of static object
static StatLog s_log;    // each source file that includes config.hpp will have its own instance of stat





// If you plan to use DirectedGraph::load and DirectedGraph::store functions:

// You should implement ostream<< and istream>> for I/O operations for the specific data type
// 
//      Helpful Links (Tutorials):
//      https://stackoverflow.com/questions/476272/how-can-i-properly-overload-the-operator-for-an-ostream
//      https://stackoverflow.com/questions/69803296/overloading-istream-operator
//      https://learn.microsoft.com/en-us/cpp/standard-library/overloading-the-output-operator-for-your-own-classes?view=msvc-170
//
// Such overloads have already been implemented for some specific standard template types that contain any printable type.
// Your overloads should either precede them or be contained within your class.


// Enable Operator Overloading only for specific types, but using a generic template
// Allowed Types are:
// list
// vector
// set
// .        add more as needed. See code below on how to do it:

// https://medium.com/@sidbhasin82/c-templates-what-is-std-enable-if-and-how-to-use-it-fd76d3abbabe

// Primary generic template for unsupported types           All types begin as unsupported
template <typename Container, typename Enable = void>
struct is_supported_container : false_type {};

// Specialization for vector                                vector is enabled
template <typename T>
struct is_supported_container<vector<T>> : true_type {};

// Specialization for list                                  list is enabled
template <typename T>
struct is_supported_container<list<T>> : true_type {};

// Specialization for set                                   set is enabled
template <typename T>
struct is_supported_container<set<T>> : true_type {};

// Specialization for unordered_set                         unordered_set is enabled
template <typename T>
struct is_supported_container<unordered_set<T>> : true_type {};


// template overload for printing any iterable container (of the above specialized), containing any printable type 
template<typename Container>
enable_if_t<is_supported_container<Container>::value, ostream&>
operator<<(ostream& stream, const Container& container) {

    stream << "<";

    bool first = true;  // flag to omit delimiter in first iteration
    for (auto it = container.begin(); it != container.end(); it++){

        if (first){
            stream << *it;  // dereference the iterator and print value in stream
            first = false;  // from now on print delimiters
        }
        else{
            stream << ", " << *it;  // print delimiter and value
        }
    }
    stream << ">";
    return stream;  // return stream to allow chaining
}


// template overload for reading any iterable container, containing any printable type.
template<typename Container>
enable_if_t<is_supported_container<Container>::value, istream&>
operator>>(istream& stream, Container& container) {  // container is updated by reference
    
    container.clear();  // ensure container will be replaced if it was non empty

    stream.ignore(1);   // reads and consumes "<"
    if (stream.peek() == '>'){
        stream.ignore(1);
        return stream;
    }

    typename Container::value_type value;   // value is of type Container::value_type which is the type of the elements in the container
    // maybe auto could work, but we assume this is safer when reading from a stream

    while (stream >> value){    // loop condition: formatted read from stream and attempt to store at Container::value_type type variable
        // stream >> value returns true if it can successfully read a Container::value_type variable from the stream
        // stream >> value returns false if the stream input cannot be converted into a Container::value_type variable format

        // if Container is vector use push_back(), else use insert(). Vector insert requires position.
        if constexpr (is_same<Container, vector<typename Container::value_type>>::value)  //https://en.cppreference.com/w/cpp/types/is_same
            container.push_back(value);
        else
            container.insert(value);   // correct format for value is inserted into the container

        if (stream.peek() == '>'){ // check if we have reached the end of the container (in string representation)
            stream.ignore(1);   // consumes the '>'
            break; 
        }
        // consume delimiter: ", " (= consume 2 characters)
        stream.ignore(2);
    }
    
    return stream;
}

// Operator Overloading for Map<Key, Value>. Works for any printable types Key and Value.

// template overload for printing a unordered_map of Key,Value
template <typename Key, typename Value>
ostream& operator<<(ostream& stream, const unordered_map<Key, Value>& m){

    stream << "{";

    bool first = true;  // flag to omit delimiter in first iteration

    for (const auto& pair : m){ // auto = pair<Key, Value>

        if (first){
            stream << "(" << pair.first << ", " << pair.second << ")";  // dereference the iterator->first for key and seconds for value and print value in stream
            first = false;  // from now on print delimiters
        }
        else{
            stream << ", " << "(" << pair.first << ", " << pair.second << ")";
        }
    }
    stream << "}";
    return stream;
}

// template overload for reading a map of Key, Value
template <typename Key, typename Value>
istream& operator>>(istream& stream, unordered_map<Key, Value>& m){

    m.clear();

    stream.ignore(1);   // reads and consumes "{"

    // if map is empty
    if (stream.peek() == '}'){
        stream.ignore(1);   // consume the '}'
        return stream;
    }
        
    Key key;
    Value value;

    // how the map string representation is formatted (BNF syntax)
    // pair ::= ε
    // pair ::= (T, set<T>)
    // pair_list ::= ε
    // pair_list ::= pair, pair_list
    // map ::= {pair_list}

    // if we reach here, it is guaranteed that the map is not empty and at least one pair exists in the map's pair_list
    
    stream.ignore(1);   // consume the first parenthesis of the pair

    while (stream >> key){  // loop condition: formatted read from stream and attempt to store T type variable (key)
        // stream >> value returns true if it can successfully read a Container::value_type variable from the stream
        // stream >> value returns false if the stream input cannot be converted into a Container::value_type variable format
        
        stream.ignore(2);   // consume in-pair delimiter ", "

        stream >> value;    // read the value

        m[key] = value;     // create the pair and put it in the map

        stream.ignore(1);   // consume pair closing parenthesis ')'
        
        if (stream.peek() == '}'){  // check if we have reached the end of the map (in string representation)
            stream.ignore(1);       // consume the '}'
            return stream;          
        }

        // consume delimiter ", "
        stream.ignore(2);
        // consume "(" for the next pair
        stream.ignore(1);
    }

    return stream;
}