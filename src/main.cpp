#include <iostream>

#include "util.hpp"
#include "types.hpp"


using namespace std;

int main () {

    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);

    // Read base vectors file
    vector<vector<float>> vectors = read_vecs<float>("data/siftsmall/siftsmall_base.fvecs", 10000);

    // Create unordered_map key: vector, value: id
    unordered_map<vector<float>, int> v2id;
    for (int i=0; i<vectors.size(); i++){
        v2id[vectors[i]] = i;
    }

    // Read query vectors file
    vector<vector<float>> queries = read_vecs<float>("data/siftsmall/siftsmall_query.fvecs", 100);

    // Read groundtruth file
    vector<vector<int>> groundtruth = read_vecs<int>("data/siftsmall/siftsmall_groundtruth.ivecs", 100);

    // Create graph nodes for all vectors
    for (auto& v : vectors){
        DG.createNode(v);
    }

    int R = 4, L=50, k=50;
    float a = 1.0f;

    // Vamana indexing
    DG.vamanaAlgorithm(L, R, a);

    cout << "Vamana Index Created!" << endl;

    // Greedy search
    vector<set<vector<float>>> GS_return =  DG.greedySearch(vectors[0], queries[0], k, L);

    cout << "Greedy search done!" << endl;

    set<vector<float>> neighbors = GS_return[0];

    // for each neighbor
    float score = 0;
    for (vector<float> n : neighbors){
        int id = v2id[n];
        int pos = getIndex(id, groundtruth[0]);
        score += pos;
    }

    cout << "FINISHED!\nSCORE IS: " << score / 5050 << endl;

    return 0;
}