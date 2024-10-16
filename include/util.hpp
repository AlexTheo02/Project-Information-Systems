#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <functional>
#include <set>

using namespace std;

// Calculates the euclidian distance between two vectors of the same size
float euclideanDistance(vector<float>, vector<float>);

// Checks for existence of element in the set
template <typename T>
bool setIn(T t, set<T> s){
    return (s.find(t) != s.end());
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
