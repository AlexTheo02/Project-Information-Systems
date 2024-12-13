#include "interface.hpp"

template <typename T>
void benchmark_euclidean(function<float(T,T)> d ,vector<T> vectors){

    for (int i = 0; i < vectors.size(); i++){
        for (int j = 0; j < vectors.size(); j++){
            d(vectors[i], vectors[j]);
        }
    }
}


// Evaluation Metrics to Consider:
// Index Creation Time
// Total Query Time
// Minimum, Average, Maximum Query Time
// Minimum, Average, Maximum Recall Score (for both filtered and unfiltered queries)
// Space Complexity (ανάλυση μέσω κώδικα;; tools?)\

// 1 000 000

// result = readvecs()

// sampled_100k = sample_from(result, 100k)

// store to file -> na diavazontai apo readvecs()

// generateGroundtruth(sampled_100k) -> minigrounds

// dokimes sta minigrounds


// ./evaluate --medoid

// Control Setting: 
// medoid is parallel
// option for medoids/(starting nodes for each category) to be given from file (precomputed)
// querying is parallel
// index creation (stitched Vamana) is serial (is parallel AN TO KANOUME PROTA)

// 1. medoids -> random points : metrics: index creation time, recall_score
// 2. parallel index creation (stitched Vamana - study paper if applicable for FilteredVamana) - MUST -------------- TODO FILTERED VAMANA
// 3. parallel querying (exoume idi)
// 4. parallel euclidean - MUST (simd)
// 5. hold a map of size T (window) LRU/MRU replacement policy - benefit or just extra space? NOT.
// 6. ClosestK na dokimasoume to Lc pou tis pernietai san orisma na einai priority Queue (logn insertion time, O(K) return - linear)
//     posa insertions VS return/partial_sort with nth_element - linear anti nlogn
//     an pano apo K insertions nomizo useless optimization

// 7. Is RGraph useless? If not, RGraph -> parallel with threads - map[out] = {outneighbors} -> no mutex required - 1 thread for each edge (exclusivity) | add_edge increments the edgecount => solution partial counts and replace final edgecount with the sum of the partials N*R ? (close)
// 8. Rgraph for filtered?  random edges between independent subgraphs
// OR post-processing (after index creation), add additional random edges.



int main(int argc, char* argv[]){

    // timing

    // dummy vectors
    vector<vector<float>> vectors128 = read_vecs<float>("data/siftsmall/siftsmall_base.fvecs", 10000);

    // double parallel_time = measureTime("Parallel Euclidean", benchmark_euclidean<float>, parallel_euclideanDistance<float>, vectors);
    // double parallel_time = measureTime("Parallel Euclidean", benchmark_euclidean<vector<float>>, parallel_euclideanDistance<vector<float>>, vectors);
    double serial_time = measureTime("Serial Euclidean", benchmark_euclidean<vector<float>>, euclideanDistance<vector<float>>, vectors128);
    double simd_time = measureTime("SIMD Euclidean128", benchmark_euclidean<vector<float>>, simd_euclideanDistance, vectors128);

    return 0;
}