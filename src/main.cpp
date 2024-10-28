#include <iostream>

#include "util.hpp"
#include "types.hpp"


using namespace std;

template <typename T>
void printVector(vector<T> v){
    cout << "<";
    for (int i=0; i<v.size(); i++){
        cout << v[i];
        if (i < v.size() -1){
            cout << ", ";
        }
    }
    cout << ">" << endl;
}

// make generic for any vector<T> container (set<vector<T>> etc)
template <typename T>
void printVectors(vector<vector<T>> vs){
    vector<T> current;
    for (int i=0; i<vs.size(); i++){
        current = vs[i];
        printVector(current);
    }
}

int main () {

    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>);

    // Read base vectors file
    vector<vector<float>> vectors = read_vecs<float>("data/siftsmall/siftsmall_base.fvecs", 10000);

    // Print base vectors
    printVectors(vectors);

    // Read groundtruth file
    vector<vector<int>> groundtruth = read_vecs<int>("data/siftsmall/siftsmall_groundtruth.ivecs", 100);

    // Print groundtruth vectors
    printVectors(groundtruth);

    return 0;
}