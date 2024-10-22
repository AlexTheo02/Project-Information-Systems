#include "util_types.hpp"
using namespace std;

// Calculates the euclidian distance between two Nodes (holding vectors)
float euclideanDistance(Node n1, Node n2){
    int dim1 = (*n1).size();
    int dim2 = (*n2).size();

    if (dim1!=dim2){
        cout<<"Dimension mismatch, dim1 is: "<<dim1 <<" dim2 is: "<<dim2 << endl;
        return -1;
    }

    if ((*n1).empty()){
        cout << "Both vectors are empty" << endl;
        return -1;
    }
    
    float sum = 0;

    for (int i = 0; i < dim1; i++)
        sum += pow(((*n1)[i] - (*n2)[i]), 2);

    return sqrt(sum);
}



// Returns the node from given nodeSet with the minimum distance from a specific vector
Node myArgMin(set<Node> nodeSet, vector<float> vec, function<float(Node, Node)> d){

    if (nodeSet.empty()){
        cout << "ERROR: NodeSet is empty returning NULL" << endl;
        return NULL;
    }

    if (vec.empty()){
        cout << "ERROR: Vector(q) is empty returning NULL" << endl;
        return NULL;
    }

    float minDist = numeric_limits<float>::max(), dist;
    Node minNode = NULL;

    for (Node n : nodeSet){
        
        // Calculate distance
        Node temp = &vec;
        dist = d(n, temp);

        // If the distance is -1, that means that there is a dimension mismatch
        if(dist == -1.0){
            return NULL;
        }

        // New minimum distance found
        if (dist < minDist){
            minNode = n;
            minDist = dist;
        }
    }

    return minNode;
}



// Retains the N closest elements of S to X based on distance d
set<Node> closestN(int N, set<Node> S, vector<float> X, function<float(Node, Node)> d){

    // transform the set to a vector
    vector<Node> Svec(S.begin(), S.end());

    // keep N first
    set<Node> closest_nodes;    

    // check if N is a valid number (size of set > N > 0)
    if (N <= 0 )
        return {};
    
    if(S.size() < N)
        return S;
    
    // check if the set is empty
    if (S.empty())
        return closest_nodes;

    // check if the vector is empty
    if (X.empty())
        return closest_nodes;

    // Check if all nodes in the set have the same dimension as X
    size_t targetDim = X.size();

    for (Node node : S) {
        if (node->size() != targetDim) {

            return {};
        }
    }

    // sort the vector based on the distance from point X
    Node temp = &X;
    sort(Svec.begin(), Svec.end(),
        [temp, d] (Node p1, Node p2) {return (d(temp, p1) < d(temp, p2));});
        // lambda(p1,p2) = determines which of the two points is closest to X

    
    for (int i = 0; i < N && i < Svec.size(); i++){
        closest_nodes.insert(Svec[i]);
    }

    return closest_nodes;
}