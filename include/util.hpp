#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>
#include <set>
#include <cstdlib>

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