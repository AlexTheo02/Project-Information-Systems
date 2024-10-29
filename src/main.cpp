#include <iostream>

#include "util.hpp"
#include "types.hpp"

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
    unordered_map<vector<float>, int> v2id
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

        vector<unordered_set<vector<float>>> GS_return =  DG.greedySearch(vectors[0], queries[i], k, L);

        unordered_set<vector<float>> neighbors = GS_return[0];

        vector<int> neighborIds;

        for (vector<float> v : neighbors){
            neighborIds.push_back(v2id[v]);
        }

        recall = k_recall(neighborIds, groundtruth[i]);

        total_recall += recall;

    }
    cout << "FINISHED!\n Average recall score is: " << total_recall / queries.size() << endl;
}


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

    // Lamda function to call profileVamana
    auto callProfileVamana = [&DG,vectors,queries,groundtruth,v2id](int L, int R, float a, int k){
        profileVamana(L,R,a,k,DG,vectors,queries,groundtruth,v2id);
    };

    callProfileVamana(100, 5, 1, 100);
    callProfileVamana(100, 10, 1, 100);
    callProfileVamana(100, 30, 1, 100);

    return 0;
}