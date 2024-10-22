#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>
#include <set>
#include <cstdlib>
#include <string>
#include <cstring>

using namespace std;

// Checks for existence of element in the set
template <typename T>
bool setIn(T t, set<T> s){
    return (s.find(t) != s.end());
}

// Checks for existence as key in an unordered map
template <typename T1, typename T2>
bool mapKeyExists(T1 key, unordered_map<T1, T2> map){
    return (map.find(key) != map.end());
}

// Subtract set2 from set1
// https://stackoverflow.com/questions/283977/c-stl-set-difference
template <typename T>
set<T> setSubtraction(set<T> set1, set<T> set2){
    set<T> result;
    set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(), inserter(result, result.end()));
    return result;
}

// Joins set1 with set1
// source corresponding to setSubtraction
template <typename T>
set<T> setUnion(set<T> set1, set<T> set2){
    set<T> result;
    set_union(set1.begin(), set1.end(), set2.begin(), set2.end(), inserter(result, result.end()));
    return result;
}

// Returns an element from the set, chosen uniformly at random
template <typename T>
T sampleFromSet(set<T> s){

    // https://stackoverflow.com/questions/3052788/how-to-select-a-random-element-in-stdset

    if (s.empty()){
        cout << "ERROR: Cannot sample from an empty set.\n";
        // return what on error? UNIT TESTS
    }

    auto it = s.begin();
    advance(it, rand() % s.size()); // iterator moves to a random position between 0 and set_size

    return *it;                     // dereferencing the iterator will return the pointed element
}

// Returns a vector of vectors from specified .fvecs file
template <typename T>
vector<vector<T>> read_vecs(string file_path, int n_vec, T){

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