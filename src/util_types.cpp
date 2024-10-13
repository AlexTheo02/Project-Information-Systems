#include "util_types.hpp"
using namespace std;

// Subtract set2 from set1
// https://stackoverflow.com/questions/283977/c-stl-set-difference
set<Node> setSubtraction(set<Node> set1, set<Node> set2){
    set<Node> result;
    set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(), inserter(result, result.end()));
    return result;
}

// Joins set1 with set1
// source corresponding to setSubtraction
set<Node> setUnion(set<Node> set1, set<Node> set2){
    set<Node> result;
    set_union(set1.begin(), set1.end(), set2.begin(), set2.end(), inserter(result, result.end()));
    return result;
}

// Returns the node witht the minimum distance from a specific node
Node myArgMin(set<Node> nodeSet, vector<float> vec, function<float(vector<float>,vector<float>)> d){

    float minDist = numeric_limits<float>::max(), dist;
    vector<float> emptyValue;
    Node minNode = Node(emptyValue, -1);

    for (Node n : nodeSet){

        // Skip any duplicates
        if (n.getValue() == vec){
            continue;
        }

        // Calculate distance
        dist = d(n.getValue(), vec);

        // New minimum distance found
        if (dist < minDist){
            minNode = n;
            minDist = dist;
        }
    }

    if (minNode.getValue() == vec)
        cout<<"ERROR: min Node NOT FOUND - RETURN SELF NODE\n";

    return minNode;
}



// Retains the N closest elements of S to X based on distance d
set<Node> closestN(int N, set<Node> S, vector<float> X, function<float(vector<float>, vector<float>)> d){

    // transform the set to a vector
    vector<Node> Svec(S.begin(), S.end());

    // sort the vector based on the distance from point X
    sort(Svec.begin(), Svec.end(),
        [X, d] (Node p1, Node p2) {return (d(X, p1.getValue()) < d(X, p2.getValue()));});

    // keep N first
    set<Node> closest_nodes;    // unit test if N > size set return whole set ktl
    for (int i = 0; i < N && i < Svec.size(); i++){
        closest_nodes.insert(Svec[i]);
    }

    return closest_nodes;
}