#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>
#include <set>
#include <cstdlib>
#include <random>

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

// Returns the medoid of the set s according to distance d
template<typename T>
T medoid(const set<T>& s, function<float(T, T)> d){
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
vector<T> permutation(const set<T>& s){

    // https://stackoverflow.com/questions/6926433/how-to-shuffle-a-stdvector

    // transforming the set into a vector
    vector<T> vec(s.begin(), s.end());

    // shuffling the vector
    auto rd = random_device {};
    auto rng = default_random_engine { rd() };
    shuffle(vec.begin(), vec.end(), rng);

    return vec;
}