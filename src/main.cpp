#include <iostream>

#include "util.hpp"
#include "types.hpp"
#include "config.hpp"

// Timing functions 
// https://www.geeksforgeeks.org/measure-execution-time-function-cpp/
#include <chrono>

#include <iomanip>

void printFormatMiliseconds(chrono::milliseconds duration){
    int hours = duration.count() / 3600000;
    int minutes = (duration.count() % 3600000) / 60000;
    int seconds = (duration.count() % 60000) / 1000;

    cout 
    << setw(2) << setfill('0') << hours << ":"
    << setw(2) << setfill('0') << minutes << ":"
    << setw(2) << setfill('0') << seconds << endl;
}

void profileVamana(
    int L, int R, float a, int k,
    DirectedGraph<vector<float>>& DG,
    const vector<vector<float>>& vectors,
    const vector<vector<float>>& queries,
    const vector<vector<int>>& groundtruth,
    map<vector<float>, int> v2id
    ){
    
    auto startTime = chrono::high_resolution_clock::now();
    // Vamana indexing
    DG.vamanaAlgorithm(L, R, a);
    auto endTime = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);

    printf("Time to execute vamanaAlgorithm(L=%d, R=%d, a=%f, k=%d): ", L,R,a,k);
    printFormatMiliseconds(duration);

    cout << "Vamana Index Created!" << endl;

    float recall = 0, total_recall = 0;
    // Greedy search for all queries
    for (int i=0; i<queries.size(); i++){

        vector<set<vector<float>>> GS_return =  DG.greedySearch(vectors[0], queries[i], k, L);

        set<vector<float>> neighbors = GS_return[0];

        vector<int> neighborIds;

        for (vector<float> v : neighbors){
            neighborIds.push_back(v2id[v]);
        }

        recall = k_recall(neighborIds, groundtruth[i]);

        total_recall += recall;

    }
    cout << "FINISHED!\nAverage recall score is: " << total_recall / queries.size() << endl;
}


using namespace std;

int main () {

    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);

    // Read base vectors file
    vector<vector<float>> vectors = read_vecs<float>("data/siftsmall/siftsmall_base.fvecs", 10000);

    // Create map key: vector, value: id
    map<vector<float>, int> v2id;
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

    // Lamda function to call profileVamana
    auto callProfileVamana = [&DG,vectors,queries,groundtruth,v2id](int L, int R, float a, int k){
        profileVamana(L,R,a,k,DG,vectors,queries,groundtruth,v2id);
    };

    callProfileVamana(100, 14, 1, 100); // creating the vamana index and querying the groundtruths
    DG.store("graph_instance.txt");


    // DG.Rgraph(5);

    // DG.store("graph_instance.txt");

    // DirectedGraph<vector<float>> DG2(euclideanDistance<vector<float>>, vectorEmpty<float>);

    // DG2.load("graph_instance.txt");


    // cout << DG.get_n_edges() << endl;
    // cout << DG2.get_n_edges() << endl;

    // cout << DG.get_n_nodes() << endl;
    // cout << DG2.get_n_nodes() << endl;

    // vector<vector<float>> n1 = DG.getNodes();
    // vector<vector<float>> n2 = DG2.getNodes();

    // cout << (n1 == n2) << endl;

    // map<vector<float>, set<vector<float>>> m1 = DG.get_Nout();
    // map<vector<float>, set<vector<float>>> m2 = DG2.get_Nout();

    // cout << (m1 == m2) << endl;

    return 0;
}