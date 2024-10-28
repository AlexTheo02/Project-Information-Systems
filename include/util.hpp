#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <set>
#include <cstdlib>
#include <string>
#include <cstring>
#include <random>

using namespace std;

// global constant on whether to omit output on tests. Set to non-zero to omit outputs, 0 to allow them.
#define SHOULD_OMIT 1

// Checks with the SHOULD_OMIT flag on whether to omit or allow output in the specific scope.
// https://stackoverflow.com/questions/30184998/how-to-disable-cout-output-in-the-runtime
#define OMIT_OUTPUT if (SHOULD_OMIT) cout.setstate(ios_base::failbit)


// calculates the euclidean distance between two containers of the same dimension that its elements can be accessed with the [ ] operator.
template <typename T>
float euclideanDistance(const T& t1, const T& t2){
    int dim1 = t1.size();
    int dim2 = t2.size();

    if (dim1!=dim2){
        throw invalid_argument("Dimension Mismatch between Arguments");
    }

    if (t1.empty()){
        throw invalid_argument("Argument Containers are empty");
    }
    
    float sum = 0.0f;

    for (int i = 0; i < dim1; i++)
        sum += pow((t1[i] - t2[i]), 2);

    return sqrt(sum);
}

// Wrapper function that checks for existence of element in the set
template <typename T>
bool setIn(const T& t, const set<T>& s){
    return (s.find(t) != s.end());
}

// Wrapper function that checks for existence as key in an unordered map
template <typename T1, typename T2>
bool mapKeyExists(const T1& key, const unordered_map<T1, T2>& map){
    return (map.find(key) != map.end());
}

// Subtract set2 from set1
// https://stackoverflow.com/questions/283977/c-stl-set-difference
template <typename T>
set<T> setSubtraction(const set<T>& set1, const set<T>& set2){
    set<T> result;
    set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(), inserter(result, result.end()));
    return result;
}

// Joins set1 with set1
// source corresponding to setSubtraction
template <typename T>
set<T> setUnion(const set<T>& set1, const set<T>& set2){
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

// Returns the medoid of the set s according to metric distance d
template<typename T>
T medoid(const set<T>& s, function<float(T, T)> d){

    // empty set case
    if (s.empty()){
        throw invalid_argument("Set is empty.\n");
    }

    // if |s| = 1 or 2, return the first element of the set (metric distance is symmetric)
    if (s.size() <= 2){
        return *(s.begin());
    }


    T med;
    float dmin = numeric_limits<float>::max();

    for (const T& t : s){
        float dsum = 0;

        for (const T& t_other : s)
            dsum += d(t, t_other);  // we don't need to check if the other element is the same, because one's distance to itself is zero

        // updating best medoid if current total distance is smaller than the minimum total distance yet
        if (dsum < dmin){
            dmin = dsum;
            med = t;
        }
    }
    return med;
}


// returns a vector of a random permutation of the elements beloning in the set s
template<typename T>
const vector<T> permutation(const set<T>& s){

    // https://stackoverflow.com/questions/6926433/how-to-shuffle-a-stdvector

    // transforming the set into a vector
    vector<T> vec(s.begin(), s.end());

    // shuffling the vector
    auto rd = random_device {};
    auto rng = default_random_engine { rd() };
    shuffle(vec.begin(), vec.end(), rng);

    return vec;
}


// Returns the node from given nodeSet with the minimum distance from a specific vector
template<typename T>
T myArgMin(set<T> nodeSet, T vec, function<float(T, T)> d){

    if (nodeSet.empty()) { throw invalid_argument("Set is Empty.\n"); }

    if (vec.empty()) { throw invalid_argument("Query container is empty.\n"); }

    float minDist = numeric_limits<float>::max(), dist;
    T minNode;

    for (T n : nodeSet){
        dist = d(n, vec);
       
        if (dist <= minDist){    // New minimum distance found
            minNode = n;
            minDist = dist;
        }
    }
    return minNode;
}


// Retains the N closest elements of S to X based on distance d
template<typename T>
set<T> closestN(int N, const set<T>& S, T X, function<float(T, T)> d){

    // check if the set is empty
    if (S.empty()){
        cout << "WARNING: Set is empty. There exist no neighbors inside the given set.\n";
        return S;
    }

    // check if the vector is empty
    if (X.empty())
        throw invalid_argument("Query X is empty.\n");

    // check if N is a valid number (size of set > N > 0)
    if (N < 0){
        throw invalid_argument("N must be greater than 0.\n");
    } 

    // if N is equal to 0 return the empty set
    if (N == 0){
        cout << "WARNING: N is 0. Returning the empty set.\n";
        return {};
    }
    
    // if N is greater than the set size, return the whole set
    if(S.size() < N)
        return S;

    // transform the set to a vector
    vector<T> Svec(S.begin(), S.end());

    // keep N first
    set<T> closest_nodes;

    // sort the vector based on the distance from point X
    sort(Svec.begin(), Svec.end(),
        [X, d] (T p1, T p2) {return (d(X, p1) < d(X, p2));});
        // lambda(p1,p2) = determines which of the two points is closest to X given distance metric d.

    
    for (int i = 0; i < N && i < Svec.size(); i++){
        closest_nodes.insert(Svec[i]);
    }

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

// User Requirement: implement the std::hash<T> for the specific T to be used.
// In this scenario, T(graph) is a vector<T2>, where T2 can be anything already implemented in the std::hash
// T2 is the inner type (that inside of the vector)

namespace std {
    // https://en.cppreference.com/w/cpp/container/unordered_map - unordered map hash defaults to std::hash => specialize for given type.
    // https://stackoverflow.com/questions/10405030/c-unordered-map-fail-when-used-with-a-vector-as-key - Hash Function for vectors.
    template <typename T2>
        class hash<vector<T2>>{
        public:
            size_t operator()(const vector<T2>& t) const{

                size_t ret = t.size();
                for (const auto& v : t) {
                    ret ^=  hash<T2>()(v) + 0x9e3779b9 + (ret << 6) + (ret >> 2);
                }
                return ret;
            }
    };
};