#include "util_types.hpp"
using namespace std;

// Returns the node with the minimum distance from a specific node
Node myArgMin(set<Node> nodeSet, vector<float> vec, function<float(vector<float>,vector<float>)> d){

    if (nodeSet.empty()){
        cout << "ERROR: NodeSet is empty returning NULL" << endl;
        return NULL;
    }

    float minDist = numeric_limits<float>::max(), dist;
    Node minNode = NULL;

    for (Node n : nodeSet){

        // Calculate distance
        dist = d(*n, vec);

        // New minimum distance found
        if (dist < minDist){
            minNode = n;
            minDist = dist;
        }
    }

    return minNode;
}



// Retains the N closest elements of S to X based on distance d
set<Node> closestN(int N, set<Node> S, vector<float> X, function<float(vector<float>, vector<float>)> d){

    // transform the set to a vector
    vector<Node> Svec(S.begin(), S.end());

    // sort the vector based on the distance from point X
    sort(Svec.begin(), Svec.end(),
        [X, d] (Node p1, Node p2) {return (d(X, *p1) < d(X, *p2));});
        // lambda(p1,p2) = determines which of the two points is closest to X

    // keep N first
    set<Node> closest_nodes;    // unit test if N > size set return whole set ktl
    for (int i = 0; i < N && i < Svec.size(); i++){
        closest_nodes.insert(Svec[i]);
    }

    return closest_nodes;
}