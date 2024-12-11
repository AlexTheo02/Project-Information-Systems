#pragma once

#include <iostream>
#include <iomanip>
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
#include <unordered_set>
#include <unordered_map>
#include <stdexcept>
#include <thread>
#include <immintrin.h>  // compiler intrinsics for SIMD optimization
#include "assert.h"

#include "config.hpp"

using namespace std;

// This file contains several independent utility function templates.
// Other .hpp files use these functions internally, but they can be used independently.


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

// euclidean distance leveraging SIMD parallelism using 256bit registers, specifically for float vectors of dimension 100 or 128 (exactly).
float simd_euclideanDistance(const vector<float>& t1, const vector<float>& t2){

    int dim = t1.size();

    if (dim != t2.size()){ throw invalid_argument("Dimension Mismatch between Arguments"); }
    if (dim != 100 && dim != 128){ throw invalid_argument("Argument Containers are not of 100 or 128 dimensions"); }

    __m256 sum = _mm256_setzero_ps();   // initialize a 256-bit register to zero value

    if (dim == 128){
        for (int i = 0; i < 128; i += 8){  // Change to backward loop
            // std::vector stores data sequentially, but it is not guaranteed that the memory is aligned in 32-bit segments. Therefore we can leverage pointer memory access, but in an unaligned fashion (loadu)
            __m256 vec1 = _mm256_loadu_ps(&t1[i]);   // load (unaligned) packed single precision (= float) value into a 256-bit register
            __m256 vec2 = _mm256_loadu_ps(&t2[i]);   // load (unaligned) packed single precision (= float) value into a 256-bit register
            __m256 diff = _mm256_sub_ps(vec1, vec2); // subtract values using 256-bit register operations
            sum = _mm256_fmadd_ps(diff, diff, sum);  // fused multiply-add operation: multiply diff*diff and add with sum. returns sum = diff*diff + sum
        }
        float results[8];
        _mm256_storeu_ps(results, sum); // store the results of the 256-register to 8 float variables.

        return results[0] + results[1] + results[2] + results[3] + results[4] + results[5] + results[6] + results[7];   // sum up the values and return
    }

    // Case dim == 100

    for (int i = 0; i < 96; i += 8){ 
        // std::vector stores data sequentially, but it is not guaranteed that the memory is aligned in 32-bit segments. Therefore we can leverage pointer memory access, but in an unaligned fashion (loadu)
        __m256 vec1 = _mm256_loadu_ps(&t1[i]);   // load (unaligned) packed single precision (= float) value into a 256-bit register
        __m256 vec2 = _mm256_loadu_ps(&t2[i]);   // load (unaligned) packed single precision (= float) value into a 256-bit register
        __m256 diff = _mm256_sub_ps(vec1, vec2); // subtract values using 256-bit register operations
        sum = _mm256_fmadd_ps(diff, diff, sum);  // fused multiply-add operation: multiply diff*diff and add with sum. returns diff*diff + sum
    }
    float results[8], result;
    _mm256_storeu_ps(results, sum); // store the results of the 256-register to 8 float variables.

    result = results[0] + results[1] + results[2] + results[3] + results[4] + results[5] + results[6] + results[7];   // sum up the values and return

    // handle remaining 4 values using an 128-bit register
    __m128 rem1 = _mm_loadu_ps(&t1[96]);
    __m128 rem2 = _mm_loadu_ps(&t2[96]);
    __m128 diff_rem = _mm_sub_ps(rem1, rem2);
    __m128 sq_diff_rem = _mm_mul_ps(diff_rem, diff_rem);
    float result_rem[4];
    _mm_store_ps(result_rem, sq_diff_rem);
    
    return result + result_rem[0] + result_rem[1] + result_rem[2] + result_rem[3];
}

// Wrapper function that checks for existence of element in the set
template <typename T>
bool setIn(const T& t, const unordered_set<T>& s){
    return (s.find(t) != s.end());
}

// Wrapper function that checks for existence as key in an unordered map
template <typename T1, typename T2>
bool mapKeyExists(const T1& key, const unordered_map<T1, T2>& map){
    return (map.find(key) != map.end());
}

// Subtract set2 from set1. Returns a new set.
template <typename T>
unordered_set<T> setSubtraction(const unordered_set<T>& set1, const unordered_set<T>& set2){
    unordered_set<T> result = set1;

    for (auto& elem : set2){
        result.erase(elem);
    }
    return result;
}

// Joins set1 with set2. Returns a new set.
template <typename T>
set<T> setUnion(const set<T>& set1, const set<T>& set2){
    // https://stackoverflow.com/questions/283977/c-stl-set-difference - adapted for setUnion

    set<T> result;
    set_union(set1.begin(), set1.end(), set2.begin(), set2.end(), inserter(result, result.end()));
    return result;
}

// Returns an element from the set, chosen uniformly at random
template <typename Container>
typename Container::value_type sampleFromContainer(const Container& s){

    // https://stackoverflow.com/questions/3052788/how-to-select-a-random-element-in-stdset

    if (s.empty()){ throw invalid_argument("Container is empty.\n"); }

    if (s.size() == 1){ return *(s.begin()); }  // directly return the singular element from the container if |s| is 1.

    if constexpr (is_same< vector<typename Container::value_type>, Container >::value)
        return s[rand() % s.size()];            // leverage vector's instant access for optimization if Container is Vector

    auto it = s.begin();
    advance(it, rand() % s.size()); // iterator moves to a random position between 0 and container_size

    return *it;                     // dereferencing the iterator to return the pointed element
}

// returns a vector of a random permutation of the elements beloning in the set s
template<typename T>
vector<T> permutation(const vector<T>& s) {
    // Copy of the vector to shuffle
    vector<T> vec(s.begin(), s.end());

    // Shuffling the vector
    random_device rd;
    default_random_engine rng(rd());  // Seed the engine with rd()

    shuffle(vec.begin(), vec.end(), rng);

    return vec;
}

// Returns a vector of vectors from specified .<f|i|b>vecs file
template <typename T>
vector<vector<T>> read_vecs(string file_path, int n_vec){

    // Open file in binary mode
    ifstream file(file_path, ios::binary);

    // To be returned
    vector<vector<T>> vectors;

    if (!file.is_open()) {
        cerr << "Error opening file: " << file_path << '\n';
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

    c_log << "Total vectors read: " << cnt << '\n';

    // Close the file
    file.close();

    c_log << file_path << " read successfully, returning vectors." << '\n';
    return vectors;
}

/// @brief Save knng in binary format (uint32_t) with name "output.bin"
/// @param knn a (N * 100) shape 2-D vector
/// @param path target save path, the output knng should be named as
/// "output.bin" for evaluation
void SaveKNN(const std::vector<std::vector<uint32_t>> &knns,
              const std::string &path = "output.bin") {
  std::ofstream ofs(path, std::ios::out | std::ios::binary);
  const int K = 100;
  const uint32_t N = knns.size();
  assert(knns.front().size() == K);
  for (unsigned i = 0; i < N; ++i) {
    auto const &knn = knns[i];
    ofs.write(reinterpret_cast<char const *>(&knn[0]), K * sizeof(uint32_t));
  }
  ofs.close();
}



/// @brief Reading binary data vectors. Raw data store as a (N x dim)
/// @param file_path file path of binary data
/// @param data returned 2D data vectors
void ReadBin(const std::string &file_path,
             const int num_dimensions,
             std::vector<std::vector<float>> &data) {
  std::cout << "Reading Data: " << file_path << std::endl;
  std::ifstream ifs;
  ifs.open(file_path, std::ios::binary);
  assert(ifs.is_open());
  uint32_t N;  // num of points
  ifs.read((char *)&N, sizeof(uint32_t));
  data.resize(N);
  std::cout << "# of points: " << N << std::endl;
  std::vector<float> buff(num_dimensions);
  int counter = 0;
  while (ifs.read((char *)buff.data(), num_dimensions * sizeof(float))) {
    std::vector<float> row(num_dimensions);
    for (int d = 0; d < num_dimensions; d++) {
      row[d] = static_cast<float>(buff[d]);
    }
    data[counter++] = std::move(row);
  }
  ifs.close();
  std::cout << "Finish Reading Data" << endl;
}

// prints a vector
template <typename T>
void printVector(const vector<T>& v){
    c_log << v;  // overload exists for vector
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
template <typename Container1, typename Container2>
float k_recall(const Container1& c1, const Container2& c2){

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

    // https://stackoverflow.com/questions/65900218/template-that-measures-elapsed-time-of-any-function
    // https://stackoverflow.com/questions/22387586/measuring-execution-time-of-a-function-in-c

    const auto start = chrono::high_resolution_clock::now();

    cout << "Calculating time for "<< name <<" . . ." << endl;

    forward<Func>(func)(forward<Args>(args)...);

    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double, milli> ms_double = end - start;

    cout << "Elapsed Time for "<< name <<": " << ms_double.count() << "ms | " << ms_double.count()/1000 << 's' << endl;

    return ms_double.count();

    // Use example: need to bind method to instance and add placeholders for arguments
    // auto boundRgraph = bind(&DirectedGraph<vector<float>>::Rgraph, &DG, placeholders::_1);
    // measureTime("Medoid", boundRgraph, 14);
}

// Timing functions 
// https://www.geeksforgeeks.org/measure-execution-time-function-cpp/
void printFormatMiliseconds(chrono::milliseconds duration){
    int hours = duration.count() / 3600000;
    int minutes = (duration.count() % 3600000) / 60000;
    int seconds = (duration.count() % 60000) / 1000;

    cout 
    << setw(2) << setfill('0') << hours << ":"
    << setw(2) << setfill('0') << minutes << ":"
    << setw(2) << setfill('0') << seconds << endl;
}

// Requirements and requirement-placeholders

// always false function for DirectedGraph::isEmpty default argument
template<typename T>
bool alwaysValid(const T& t) { return false; }

template<typename T>
bool alwaysEmpty(const T& t) { return true; }

// ValidCheck function specific for vector container of any type.
template<typename T>
bool vectorEmpty(const vector<T>& v){ return v.empty(); }




