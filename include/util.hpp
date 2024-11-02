#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>
#include <map>
#include <set>
#include <cstdlib>
#include <string>
#include <cstring>
#include <random>
#include <chrono>
#include <utility>
#include <type_traits>
#include <list>

using namespace std;

// global constant on whether to omit output on tests. Set to non-zero to omit outputs, 0 to allow them.
#define SHOULD_OMIT 1

// Checks with the SHOULD_OMIT flag on whether to omit or allow output (sets state).
// https://stackoverflow.com/questions/30184998/how-to-disable-cout-output-in-the-runtime
#define OMIT_OUTPUT if (SHOULD_OMIT) cout.setstate(ios_base::failbit)

// calculates the euclidean distance between two containers of the same dimension that its elements can be accessed with the [ ] operator.
template <typename T>
float euclideanDistance(const T& t1, const T& t2){
    int dim1 = t1.size();
    int dim2 = t2.size();

    if (dim1!=dim2){ throw invalid_argument("Dimension Mismatch between Arguments"); }

    if (t1.empty()){ throw invalid_argument("Argument Containers are empty"); }
    
    float sum = 0.0f;

    for (int i = 0; i < dim1; i++){
        float diff = t1[i] - t2[i];
        sum += diff * diff;         // better performance than pow(diff,2)
    }

    return sum; 
    // return sqrt(sum);    // because we only care about comparisons, sqrt is not needed [x1 < x2 <=> sqrt(x1) < sqrt(x2), ∀ x1,x2 > 0]
}

// Wrapper function that checks for existence of element in the set
template <typename T>
bool setIn(const T& t, const set<T>& s){
    return (s.find(t) != s.end());
}

// Wrapper function that checks for existence as key in an unordered map
template <typename T1, typename T2>
bool mapKeyExists(const T1& key, const map<T1, T2>& map){
    return (map.find(key) != map.end());
}

// Subtract set2 from set1. Returns a new set.
template <typename T>
set<T> setSubtraction(const set<T>& set1, const set<T>& set2){
    set<T> result = set1;

    for (auto& elem : set2){
        result.erase(elem);
    }
    return result;
}

// Joins set1 with set1. Returns a new set.
template <typename T>
set<T> setUnion(const set<T>& set1, const set<T>& set2){
    // https://stackoverflow.com/questions/283977/c-stl-set-difference - adapted for setUnion

    set<T> result;
    set_union(set1.begin(), set1.end(), set2.begin(), set2.end(), inserter(result, result.end()));
    return result;
}

// Returns an element from the set, chosen uniformly at random
template <typename T>
T sampleFromSet(const set<T>& s){

    // https://stackoverflow.com/questions/3052788/how-to-select-a-random-element-in-stdset

    if (s.empty()){ throw invalid_argument("Set is empty.\n"); }

    if (s.size() == 1){ return *(s.begin()); }  // directly return the singular element from the set if |s| is 1.

    auto it = s.begin();
    advance(it, rand() % s.size()); // iterator moves to a random position between 0 and set_size

    return *it;                     // dereferencing the iterator to return the pointed element
}

// returns a vector of a random permutation of the elements beloning in the set s
template<typename T>
vector<T> permutation(const set<T>& s) {
    // Transforming the set into a vector
    vector<T> vec(s.begin(), s.end());

    // Shuffling the vector
    random_device rd;
    default_random_engine rng(rd());  // Seed the engine with rd()

    shuffle(vec.begin(), vec.end(), rng);

    return vec;
}

// Returns the node from given nodeSet with the minimum distance from a specific point in the nodespace (node is allowed to not exist in the graph)
template<typename T>
T myArgMin(const set<T>& nodeSet, T t, function<float(const T&, const T&)> d, function<bool(const T&)> isEmpty){

    if (nodeSet.empty()) { throw invalid_argument("Set is Empty.\n"); }

    if (isEmpty(t)) { throw invalid_argument("Query container is empty.\n"); }

    if (nodeSet.size() == 1)
        return *nodeSet.begin();

    float minDist = numeric_limits<float>::max(), dist;
    T minNode;

    for (const T& n : nodeSet){
        dist = d(n, t);
       
        if (dist <= minDist){    // New minimum distance found
            minNode = n;
            minDist = dist;
        }
    }
    return minNode;
}

// Retains the N closest elements of S to X based on distance d
template<typename T>
set<T> closestN(int N, const set<T>& S, T X, function<float(const T&, const T&)> d, function<bool(const T&)> isEmpty){

    // check if the set is empty
    if (S.empty()){
        cout << "WARNING: Set is empty. There exist no neighbors inside the given set.\n";
        return S;
    }

    // check if the vector is empty
    if (isEmpty(X)) { throw invalid_argument("Query X is empty.\n"); }

    // check if N is a valid number (size of set > N > 0)
    if (N < 0){ throw invalid_argument("N must be greater than 0.\n"); } 

    // if N is equal to 0 return the empty set
    if (N == 0){
        cout << "WARNING: N is 0. Returning the empty set.\n";
        set<T> nullset;
        return nullset;
    }
    
    // if N is greater than the set size, return the whole set
    if(S.size() < N)
        return S;

    // transform the set to a vector for partitioning around a pivot
    vector<T> Svec(S.begin(), S.end());

    // partition the vector based on the distance from point X up around the N-th element
    nth_element(Svec.begin(), Svec.begin() + N, Svec.end(),
                [X, d] (T p1, T p2) {return (d(X, p1) < d(X, p2));});
                // lambda(p1,p2) = determines which of the two points is closest to X given distance metric d.

    // the vector after the use of nth_element has the following properties:
    // the N-th element is in the position that it would've been if the vector was sorted in its entirety
    // elements before the N-th element are < than the N-th element, and elements after the N-th element are > than the N-th element. (< and > defined by compare function)
    // the elements in the left and right subvectors are not themselves sorted, but for this task we don't need them sorted. 
    // https://en.cppreference.com/w/cpp/algorithm/nth_element

    // keep N first
    set<T> closest_nodes(Svec.begin(), Svec.begin() + N);

    return closest_nodes;
}

// Returns a vector of vectors from specified .<f|i|b>vecs file
template <typename T>
vector<vector<T>> read_vecs(string file_path, int n_vec){

    // Open file in binary mode
    ifstream file(file_path, ios::binary);

    // To be returned
    vector<vector<T>> vectors;

    if (!file.is_open()) {
        cerr << "Error opening file: " << file_path << endl;
        return vectors; // empty
    }

    int dim,cnt=0, vec_size;
    char *dim_buffer, *vec_buffer;
    // Read file
    for (int i = 0; i<n_vec && !file.eof(); i++) {

        dim_buffer = (char*) malloc(sizeof(T));

        // Read dimension (4 - int)
        file.read(dim_buffer, 4);
        
        memcpy(&dim, dim_buffer, 4);

        if (dim > 0){
            cnt++;
            vec_size = dim * sizeof(T);
            // Allocate memory for vec_buffer
            vec_buffer = (char*) malloc(vec_size);

            // Create vector with correct dimension
            vector<T> vec(dim);

            // Read vector and assign values to it (d*sizeof(T) - 4 | 4 | 1 for T: float | int | char)
            file.read(vec_buffer, vec_size);

            memcpy(vec.data(), vec_buffer, vec_size);

            // Add vector to vectors
            vectors.push_back(vec);

        }
        if (dim_buffer != NULL)
            free(dim_buffer);
        if (vec_buffer != NULL)
            free(vec_buffer);
        dim = 0;
    }

    cout << "Total vectors read: " << cnt << endl;

    // Close the file
    file.close();

    cout << file_path << " read successfully, returning vectors." << endl;
    return vectors;
}

// prints a vector
template <typename T>
void printVector(const vector<T>& v){
    cout << v;  // overload exists for vector
}

// Prints all vectors stored in any iterable container
template <typename T>
void printVectors(const T& vs){
    for (auto& current : vs){ printVector(current); } // auto type is vector<ContentType>, int,float,...
}

// Returns the index of the requested value inside the given vector
template <typename T>
int getIndex(const T& value, const vector<T>& v){
    auto it = find(v.begin(), v.end(), value);
    return distance(v.begin(), it);
}


// Evaluation:

// Returns the k-recall value between 2 Containers that support the .size(), .begin(), .end() methods
template <typename Container>
float k_recall(const Container& c1, const Container& c2){

    if (c1.empty()) { return 0.0f; }

    int cnt = 0;
    for (auto& elem : c1){
        if (find(c2.begin(), c2.end(), elem) != c2.end()) { cnt++; }
    }
    // Typecast return
    return (float) cnt / c1.size();
}

// Measures the time it takes for the given function to run. Absorbs any function returns.
// See use example in implementation.
template <typename Func, typename ...Args>
double measureTime(const string name, Func func, Args&&... args){
    OMIT_OUTPUT;

    // https://stackoverflow.com/questions/65900218/template-that-measures-elapsed-time-of-any-function
    // https://stackoverflow.com/questions/22387586/measuring-execution-time-of-a-function-in-c

    const auto start = chrono::high_resolution_clock::now();

    cout << "Calculating time for "<< name <<" . . ." << endl;

    forward<Func>(func)(forward<Args>(args)...);

    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double, milli> ms_double = end - start;

    cout << "Elapsed Time for "<< name <<": " << ms_double.count() << "ms | " << ms_double.count()/1000 << "s" << endl;

    return ms_double.count();

    // Use example: need to bind method to instance and add placeholders for arguments
    // auto boundRgraph = bind(&DirectedGraph<vector<float>>::Rgraph, &DG, placeholders::_1);
    // measureTime("Medoid", boundRgraph, 13);
}


// Requirements and requirement-placeholders

// always false function for isEmpty default argument
template<typename T>
bool alwaysValid(const T& t) { return false; }

// ValidCheck function specific for vector container of any type.
template<typename T>
bool vectorEmpty(const vector<T>& v){ return v.empty(); }



// NEW FILE FROM HERE DOWN



// User Requirement: implement the std::hash<T> for the specific T to be used.
// In this scenario, Content Type of graph T is a vector<T2>, where T2 can be anything already implemented (or expanded) in the std::hash
// T2 is the inner type (that inside of the vector: int, float, ..., any other type that std::hash<T> has been expanded for)
// namespace std {
//     // https://en.cppreference.com/w/cpp/container/map - unordered map hash defaults to std::hash<TYPE> => specialize for given type TYPE.
//     // https://stackoverflow.com/questions/10405030/c-unordered-map-fail-when-used-with-a-vector-as-key - Hash Function for vectors.
//     template <typename T2>
//         class hash<vector<T2>>{
//         public:
//             size_t operator()(const vector<T2>& t) const{

//                 size_t ret = t.size();
//                 for (const auto& v : t) {
//                     ret ^=  hash<T2>()(v) + 0x9e3779b9 + (ret << 6) + (ret >> 2);
//                 }
//                 return ret;
//             }
//     };
// };
// from previous implementation. Commented out instead of deleted because we might need it again in future updates.



















// Operator Overloading for Map (adjacency). Works for any printable type T.

// template overload for printing a map of T,set<T>
template <typename T>
ostream& operator<<(ostream& stream, const map<T, set<T>>& m){

    stream << "{";

    bool first = true;  // flag to omit delimiter in first iteration

    for (const auto& pair : m){

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

// template overload for reading a map of T,set<T>
template <typename T>
istream& operator>>(istream& stream, map<T, set<T>>& m){

    m.clear();

    stream.ignore(1);   // reads and consumes "{"

    // if map is empty
    if (stream.peek() == '}'){
        stream.ignore(1);   // consume the '}'
        return stream;
    }
        
    T key;
    set<T> value;

    // pair ::= ε
    // pair ::= (T, set<T>)
    // pair_list ::= ε
    // pair_list ::= pair, pair_list
    // map ::= {pair_list}

    // if we reach here, it is guaranteed that the map is not empty and a pair exists
    
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



// User Requirement: implement << and >> for I/O operations for the specific data type
// https://stackoverflow.com/questions/476272/how-can-i-properly-overload-the-operator-for-an-ostream
// https://stackoverflow.com/questions/69803296/overloading-istream-operator



// Enable Operator Overloading only for specific types, but using a generic template
// Allowed Types are:
// list
// vector
// set
// ADD MORE IF NEEDED

// https://medium.com/@sidbhasin82/c-templates-what-is-std-enable-if-and-how-to-use-it-fd76d3abbabe

// Primary template for unsupported types
template <typename Container, typename Enable = void>
struct is_supported_container : false_type {};

// Specialization for vector
template <typename T>
struct is_supported_container<vector<T>> : true_type {};

// Specialization for list
template <typename T>
struct is_supported_container<list<T>> : true_type {};

// Specialization for set
template <typename T>
struct is_supported_container<set<T>> : true_type {};


// template overload for printing any iterable container, containing any printable type 
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