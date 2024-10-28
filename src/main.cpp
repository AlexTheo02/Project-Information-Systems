#include <iostream>

#include "util.hpp"
#include "types.hpp"


using namespace std;

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