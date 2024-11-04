#include "interface.hpp"
using namespace std;



int main () {

    // Instantiate a DirectedGraph object DG
    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);

    // Read base vectors file
    vector<vector<float>> vectors = read_vecs<float>("data/siftsmall/siftsmall_base.fvecs", 10000);

    // Create map key: vector, value: id (for groundtruth)
    map<vector<float>, int> v2id;
    for (int i=0; i<vectors.size(); i++){
        v2id[vectors[i]] = i;
    }

    // Read query vectors file
    vector<vector<float>> queries = read_vecs<float>("data/siftsmall/siftsmall_query.fvecs", 100);

    // Read groundtruth file
    vector<vector<int>> groundtruth = read_vecs<int>("data/siftsmall/siftsmall_groundtruth.ivecs", 100);

    // Populate the Graph
    for (auto& v : vectors){
        DG.createNode(v);
    }

    int L = 100;
    int R = 14;
    float a = 1.0f;
    float k = 100;

    profileVamana(L,R,a,k,DG,vectors,queries,groundtruth,v2id); // creating the vamana index and querying the groundtruths

    DG.store("graph_instance.txt"); // store the vamana Index

    return 0;
}