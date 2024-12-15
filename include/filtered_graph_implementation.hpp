#pragma once

#include "types.hpp"

// This file implements member functions of the DirectedGraph class template declared in the types.hpp header file.


// medoids
template <typename T>
const unordered_map<int, Id> DirectedGraph<T>::findMedoids(float threshold){
    
    c_log << "Filtered Medoids\n";

    // argument checks
    if (threshold <= 0 || threshold > 1){ throw invalid_argument("threshold must be in (0,1]\n"); }
    if (!this->filteredMedoids.empty()) { return this->filteredMedoids; }
    
    return _filtered_medoid(threshold);
}

template <typename T>
const unordered_map<int, Id> DirectedGraph<T>::_filtered_medoid(float threshold){

    // Initialize T_counter (counts how many times a specific node has been selected as a medoid)
    vector<int> T_counter(this->n_nodes, 0);

    // For each category
    for (pair<int, unordered_set<Id>> cpair : this->categories){
        
        vector<Node<T>> Rf;
        unordered_set<Id> remaining(cpair.second.begin(), cpair.second.end()); // remaining is a copy that holds all the remaining values to be sampled from

        while (!remaining.empty() && Rf.size() < (ceil(threshold * cpair.second.size()))){
            int sample = sampleFromContainer(remaining);
            Rf.push_back(this->nodes[sample]);
            remaining.erase(sample);
        }

        this->filteredMedoids[cpair.first] = this->medoid(Rf);
    }
    c_log << "MEDOIDS FOUND" << "\n";
    
    return this->filteredMedoids;
}

// Returns a filtered set
template <typename T>
unordered_set<Id> DirectedGraph<T>::filterSet(unordered_set<Id> S, int filter){
    if (filter == -1){
        return S;
    }
    if(S.empty()) return S;
    unordered_set<Id> filtered;

    for (Id s : S){
        if (this->nodes[s].category == filter){
            filtered.insert(s);
        }
    }
    
    return filtered;
}

template <typename T>
const pair<unordered_set<Id>, unordered_set<Id>> DirectedGraph<T>::filteredGreedySearch(Id s, Query<T> q, int k, int L){

    c_log << "Filtered Greedy Search\n";

    // Argument checks

    // No filters are present, perform unfiltered greedy search
    if (q.empty()) { return this->greedySearch(this->startingNode(), q.value, k, L); }

    if (s == -1){ throw invalid_argument("No start node was provided.\n"); }

    if (k < 0){ throw invalid_argument("K must be greater than or equal to 0.\n"); }

    if (L < k){ throw invalid_argument("L must be greater or equal to K.\n"); }

    C = 0;
    I = 0;

    // Create empty sets
    unordered_set<Id> Lc, V, diff;
    Node<T> sNode;

    // Get node using id from s
    sNode = this->nodes[s];
    
    // Category match
    if (sNode.category == q.category) {
        I++;
        Lc.insert(s);
    }

    while (!(diff = setSubtraction(Lc,V)).empty()){
        Id pmin = _myArgMin(diff, q.value);     // pmin is the node with the minimum distance from query xq

        V.insert(pmin);

        unordered_set<Id> filteredNoutPmin;
        if (mapKeyExists(pmin, this->Nout))
            filteredNoutPmin = this->filterSet(setSubtraction(this->Nout[pmin], V), q.category);

        I++;
        Lc.insert(filteredNoutPmin.begin(), filteredNoutPmin.end());

        if (Lc.size() > L){
            C++;
            Lc = _closestN(L, Lc, q.value); // function: find N closest points from a specific xq from given set and return them
        }
    }
    
    string file_path = (greedySearchMode) ? "./evaluations/greedySearchQueryStats.txt"
                                          : "./evaluations/greedySearchIndexStats.txt";

    ofstream outFile;
    greedySearchCount ? outFile.open(file_path, ios::app) : outFile.open(file_path, ios::trunc); // Open for writing
    if (!outFile.is_open()) {
        cerr << "Error: could not open " << file_path << " for writing." << endl;
        // return EXIT_FAILURE;
    }
    if (greedySearchCount == 0){
        outFile << "greedySearchCount" << "," << "C" << "," << "I" << endl;
    }
    outFile << greedySearchCount << "," << C << "," << I << endl;

    outFile.close();

    greedySearchCount ++;
    
    pair<unordered_set<Id>, unordered_set<Id>> ret;
    
    ret.first = _closestN(k, Lc, q.value);
    ret.second = V;

    return ret;
}

template <typename T>
void DirectedGraph<T>::filteredRobustPrune(Id p, unordered_set<Id> V, float a, int R){

    c_log << "Filtered Robust Prune\n";

    // argument checks
    if (a < 1) { throw invalid_argument("Parameter a must be >= 1.\n"); }

    if (R <= 0) {throw invalid_argument("Parameter R must be > 0.\n"); }

    if (p < 0 || p >= this->n_nodes){ throw invalid_argument("Invalid Index was provided.\n"); }

    if (this->nodes[p].empty()) { throw invalid_argument("No node was provided.\n"); }

    if (mapKeyExists(p, this->Nout))
        V.insert(this->Nout[p].begin(), this->Nout[p].end());
    
    V.erase(p);
    this->clearNeighbors(p);

    while (!V.empty()){
        Id pmin = this->_myArgMin(V, this->nodes[p].value);
        this->addEdge(p, pmin);

        if (this->Nout[p].size() == R)  break;

        // pmin = p*, pv = p', p = p (as seen in paper)
        // *it = pv
        
        // safe iteration on V for deletions
        for (auto it = V.begin(); it != V.end(); /*no increment here*/){ // safe set iteration with mutable set during the iteration
            if (this->nodes[*it].category == this->nodes[p].category && this->nodes[p].category != this->nodes[pmin].category) {it++; continue;}
            if (a * this->d(this->nodes[pmin].value, this->nodes[*it].value) <= this->d(this->nodes[p].value, this->nodes[*it].value)){
                it = V.erase(it); // it now points to the next element in the set
            }
            else { it++; }
        }
    }
}

template <typename T>
bool DirectedGraph<T>::filteredVamanaAlgorithm(int L, int R, float a, float t){

    if (R <= 0){ throw invalid_argument("R must be a positive, non-zero integer.\n"); }

    if (L < 1) { throw invalid_argument("Parameter L must be >= 1.\n"); }

    if (a < 1) { throw invalid_argument("Parameter a must be >= 1.\n"); }

    if (t <= 0 || t > 1) { throw invalid_argument("Parameter t must be between (0,1]"); }

    if (this->nodes.size() == 1) return true;   // no edges possible

    c_log << "Filtered Vamana\n";

    // initialize G as an empty graph => clear all edges
    if(this->clearEdges() == false)
        return false;

    if (args.n_threads == 1){

        vector<Id> nodes_ids(this->n_nodes);
        iota(nodes_ids.begin(), nodes_ids.end(), 0);

        vector<Id> perm_id = permutation(nodes_ids);

        return this->_serial_filteredVamana(L, R, a, t, ref(perm_id));
    }
    else{
        
        // Heuristic for better thread job scheduling: sort based on diminishing workload (Longest Processing Time First) (REFERENCE HERE) TODO
        // Copy elements into a vector of pairs to sort
        vector<pair<int, vector<Id>>> sorted_categories;

        // Convert the categories into a vector containing a pair of an int(category id) and a vector containing all the ids of that category's nodes
        for (pair<int, unordered_set<Id>> cpair : this->categories) {
            sorted_categories.emplace_back(cpair.first, vector<Id>(cpair.second.begin(), cpair.second.end()));
        }

        // Sort the vector based on the size of the vector in the second element
        sort(sorted_categories.begin(), sorted_categories.end(),
                [](const pair<int, vector<Id>>& cpair1, const pair<int, vector<Id>>& cpair2) {
                    return cpair1.second.size() > cpair2.second.size();
                });
        
        
        return this->_parallel_filteredVamana(L, R, a, t, sorted_categories);
    }
    
}

template <typename T>
bool DirectedGraph<T>::_serial_filteredVamana(int L, int  R, float a, float t, vector<Id>& perm){

    for (Id& si_id : perm){

        Node<T> si = this->nodes[si_id];
        // Id starting_node_i = this->startingNode(); // st[si.category];   // because each node belongs to at most one category.
        // unordered_map<int, Id> st = this->findMedoids(t);  // paper says starting points should be the medoids found in [algorithm 2]
        

        // create query with si value to pass to filteredGreedySearch
        Query<T> q(si.id, si.category, true, si.value, this->isEmpty);

        unordered_set<Id> Vi = this->filteredGreedySearch(this->startingNode(q.category), q, 0, L).second;

        filteredRobustPrune(si.id, Vi, a, R);

        if (mapKeyExists(si.id, this->Nout)){
            
            unordered_map<Id, unordered_set<Id>> NoutCopy(this->Nout.begin(), this->Nout.end());
            for (const Id j : NoutCopy[si.id]){  // for every neighbor j of si

                this->addEdge(j, si.id);   // does it in either case (simpler code, robust prune clears all neighbors after copying to candidate set V anyway)
                int noutSize = this->Nout[j].size();
                if (noutSize > R)
                    filteredRobustPrune(j, this->Nout[j], a, R);
            }
        }
    }
    return true;
}

template <typename T>
bool DirectedGraph<T>::_parallel_filteredVamana(int L, int  R, float a, float t, vector<pair<int, vector<Id>>>& sorted_categories){

    int current_index = 0;
    mutex mx_index;

    vector<thread> threads;

    vector<char> rvs;   // return values of threads - actually bool type

    this->findMedoids(t);   // pre-computing medoids for sync issues
    // vector<Id> perm = {1865, 2404, 4674, 6392, 2256, 2386, 9409, 9007, 3177, 4220, 421, 3495, 2859, 6290, 7476, 9249, 8443, 8121, 7075, 4531, 3574, 328, 9476, 458, 4402, 3356, 5865, 3566, 8634, 4844, 8714, 4208, 2503, 3690, 8858, 4291, 1834, 1674, 2328, 2390, 4241, 5187, 4255, 7013, 5201, 6543, 420, 9322, 3220, 6729, 4403, 8662, 4439, 8117, 8540, 8096, 5347, 8291, 6462, 663, 8204, 4462, 791, 7925, 8302, 8586, 1719, 2936, 6455, 87, 9710, 9178, 3441, 7168, 4426, 1228, 67, 6852, 1541, 9110, 4526, 974, 153, 6807, 7780, 5020, 4642, 1900, 9451, 6582, 1604, 2172, 2666, 7016, 8128, 2238, 3472, 7058, 244, 2900, 6386, 942, 2408, 7701, 2122, 2120, 4259, 6322, 3350, 3754, 8496, 3634, 3335, 1034, 3541, 4692, 8697, 3217, 6518, 997, 2712, 2647, 8963, 9006, 5989, 4000, 5960, 6656, 8312, 1821, 8748, 7233, 7776, 259, 711, 9018, 9779, 5308, 375, 1517, 6803, 5892, 5762, 7545, 7903, 6364, 5859, 3640, 1850, 1933, 8920, 6835, 7746, 5623, 4753, 7332, 2740, 846, 3617, 2444, 5232, 7466, 4754, 3929, 293, 8048, 9868, 463, 7994, 2699, 9594, 441, 671, 4965, 1968, 2219, 2831, 6245, 5386, 1580, 3342, 9010, 5526, 5321, 8140, 198, 3500, 5429, 6469, 8501, 8657, 9407, 9687, 1465, 550};
    // vector<Id> perm = permutation(sorted_categories[7].second);
    // this->_serial_filteredVamana(L,R,a,t,perm);

    // return true;

    this->Nout.reserve(this->n_nodes);

    for (int i = 0; i < args.n_threads; i++){
        rvs.push_back(true);

        threads.push_back(thread(
            &DirectedGraph::_thread_filteredVamana_fn,
            this,
            ref(L),
            ref(R),
            ref(a),
            ref(t),
            ref(current_index),
            ref(mx_index),
            ref(rvs[i]),
            ref(sorted_categories)));
    }

    for (thread& th : threads)
        th.join();


    for (bool rv : rvs){
        if (rv == false){
            c_log << "Something went wrong in the Filtered Vamana Index Creation.\n";
            return false;
        }
    }

    c_log << "Filtered Vamana Index Created.\n";
    return true;

}

template <typename T>
void DirectedGraph<T>::_thread_filteredVamana_fn(int& L, int& R, float& a, float& t, int& current_index, mutex& mx_index, char& rv, vector<pair<int, vector<Id>>>& sorted_categories){


    
    mx_index.lock();
    while(current_index < sorted_categories.size()){
        int my_index = current_index++;
        mx_index.unlock();

        // cout << my_index << endl;
        vector<Id> perm = permutation(sorted_categories[my_index].second);
        // vector<Id> perm = {1865, 2404, 4674, 6392, 2256, 2386, 9409, 9007, 3177, 4220, 421, 3495, 2859, 6290, 7476, 9249, 8443, 8121, 7075, 4531, 3574, 328, 9476, 458, 4402, 3356, 5865, 3566, 8634, 4844, 8714, 4208, 2503, 3690, 8858, 4291, 1834, 1674, 2328, 2390, 4241, 5187, 4255, 7013, 5201, 6543, 420, 9322, 3220, 6729, 4403, 8662, 4439, 8117, 8540, 8096, 5347, 8291, 6462, 663, 8204, 4462, 791, 7925, 8302, 8586, 1719, 2936, 6455, 87, 9710, 9178, 3441, 7168, 4426, 1228, 67, 6852, 1541, 9110, 4526, 974, 153, 6807, 7780, 5020, 4642, 1900, 9451, 6582, 1604, 2172, 2666, 7016, 8128, 2238, 3472, 7058, 244, 2900, 6386, 942, 2408, 7701, 2122, 2120, 4259, 6322, 3350, 3754, 8496, 3634, 3335, 1034, 3541, 4692, 8697, 3217, 6518, 997, 2712, 2647, 8963, 9006, 5989, 4000, 5960, 6656, 8312, 1821, 8748, 7233, 7776, 259, 711, 9018, 9779, 5308, 375, 1517, 6803, 5892, 5762, 7545, 7903, 6364, 5859, 3640, 1850, 1933, 8920, 6835, 7746, 5623, 4753, 7332, 2740, 846, 3617, 2444, 5232, 7466, 4754, 3929, 293, 8048, 9868, 463, 7994, 2699, 9594, 441, 671, 4965, 1968, 2219, 2831, 6245, 5386, 1580, 3342, 9010, 5526, 5321, 8140, 198, 3500, 5429, 6469, 8501, 8657, 9407, 9687, 1465, 550};

        // vector<Id> perm = {2390, 7476, 7925, 4692, 671, 4462, 328, 8302, 259, 4241, 8291, 6518, 1821, 974, 5187, 4844, 9687, 3754, 441, 1228, 3500, 8496, 7701, 5989, 1604, 6835, 2647, 2740, 5623, 3177, 7058, 6656, 3342, 550, 1541, 8540, 9476, 1719, 1850, 2936, 198, 9110, 8204, 8501, 4753, 4291, 3495, 2408, 4403, 1517, 3220, 4526, 4426, 87, 2404, 1900, 9710, 3217, 5429, 791, 153, 8096, 5762, 6852, 8586, 244, 7466, 5865, 2238, 375, 4259, 9010, 5960, 3541, 997, 6322, 6455, 2900, 2444, 5386, 2256, 6729, 6807, 1034, 7233, 5347, 1834, 293, 2699, 9007, 9178, 663, 4965, 7780, 2122, 3350, 942, 9018, 6245, 3356, 5201, 6543, 420, 2172, 67, 1968, 463, 1933, 8714, 3441, 4674, 6582, 711, 421, 9779, 1465, 9409, 1674, 2503, 8963, 8858, 2859, 3617, 7746, 6462, 3634, 7994, 7075, 6386, 7545, 9249, 9006, 3574, 2120, 8662, 9451, 2712, 8748, 7776, 5321, 7016, 8697, 4208, 6392, 4255, 7013, 7168, 5526, 6290, 8117, 7332, 4402, 6803, 4220, 7903, 5232, 8657, 2328, 3472, 9868, 3690, 2666, 4531, 1580, 8634, 2219, 846, 1865, 3335, 2386, 5892, 5020, 9407, 8128, 9594, 4754, 5859, 3566, 8312, 9322, 8121, 8048, 6469, 2831, 8443, 5308, 3929, 8140, 4439, 3640, 4642, 4000, 6364, 458, 8920};


        mx_index.lock();
        cout << "----------------------------------------------\n";
        cout << my_index << endl;
        cout << perm << endl;
        mx_index.unlock();
        this->_serial_filteredVamana(L,R,a,t,perm);

        cout << my_index << " Ended\n";

        // cout << "Filtered Vamana Ended with my index: "<< my_index << endl;
        

        mx_index.lock();
    }
    mx_index.unlock();

}

template <typename T>
bool DirectedGraph<T>::stitchedVamanaAlgorithm(int Lstitched, int Rstitched, int Lsmall, int Rsmall, float a){

    c_log << "Stitched Vamana\n";

    if (Lstitched < 1) { throw invalid_argument("Parameter L must be >= 1.\n"); }
    
    if (Rstitched <= 0){ throw invalid_argument("Rstitched must be a positive, non-zero integer.\n"); }

    if (Lsmall < 1) { throw invalid_argument("Parameter Lsmall must be >= 1.\n"); }

    if (Rsmall <= 0){ throw invalid_argument("Rsmall must be a positive, non-zero integer.\n"); }

    if (a < 1) { throw invalid_argument("Parameter a must be >= 1.\n"); }

    if (args.n_threads <= 0) throw invalid_argument("args.n_threads constant is invalid. Value must be args.n_threads >= 1.\n");

    return (args.n_threads == 1)
        ? this->_serial_stitchedVamana(Lstitched, Rstitched, Lsmall, Rsmall, a)
        : this->_parallel_stitchedVamana(Lstitched, Rstitched, Lsmall, Rsmall, a); 
}

template <typename T>
bool DirectedGraph<T>::_serial_stitchedVamana(int Lstitched, int Rstitched, int Lsmall, int Rsmall, float a){

    // initialize G as an empty graph => clear all edges
    if(this->clearEdges() == false)
        return false;

    DirectedGraph<T> DGf(this->d, this->isEmpty);
    for (pair<int, unordered_set<Id>> cpair : this->categories){
        
        vector<Id> original_id;                         // a vector that maps DGf node ids to the original this ids

        for (Id node : cpair.second){
            DGf.createNode(this->nodes[node].value);    // create nodes as unfiltered data (specific category)
            original_id.push_back(node);
        }

        c_log << "Creating Index for Category: " << cpair.first << '\n';

        // Creating Index for nodes of specific category as unfiltered data
        int Rsmall_f = min(Rsmall, (int)cpair.second.size() - 1);    // handle case when Rsmall > |Pf| - 1 for certain filters f
        if (!DGf.vamanaAlgorithm(Lsmall, Rsmall_f, a)){
            cout << "Something went wrong in vamana algorithm.\n";
            return false;
        }
        c_log << "Creating Index for Category: " << cpair.first << " created.\n";
        
        
        c_log << "Pruning.\n";
        for (Node<T>& node : DGf.nodes){
            DGf.robustPrune(node.id, DGf.Nout[node.id], a, Rstitched);
        }
        
        c_log << "Pruning complete.Stitching with main graph.\n";

        // union of edges: this->Nout ∪= DGf.Nout
        for (pair<Id, unordered_set<Id>> edge : DGf.get_Nout()){
            Id from = edge.first;
            for (Id to : edge.second)
                this->addEdge(original_id[from], original_id[to]);
        }
        this->filteredMedoids[cpair.first] = original_id[DGf.medoid()];
        c_log << "Stitching successful.\n";

        // cleanup for next category
        original_id.clear();
        DGf.init();
    }
    c_log << "Stitched Vamana Index Created.\n";


    return true;
}


template <typename T>
void DirectedGraph<T>::_thread_stitchedVamana_fn(int& Lstitched, int& Rstitched, int& Lsmall, int& Rsmall, float& a, int& category_index, mutex& mx_category_index, mutex& mx_merge, vector<int>& category_names, char& rv){
    

    mx_category_index.lock();
    while(category_index < this->categories.size()){
        int my_category_index = category_index++;
        mx_category_index.unlock();

        int my_category = category_names[my_category_index];

        vector<Id> original_id;                         // a vector that maps DGf node ids to the original this ids

        DirectedGraph<T> DGf(this->d, this->isEmpty);

        for (Id node : this->categories[my_category]){
            DGf.createNode(this->nodes[node].value);    // create nodes as unfiltered data (specific category)
            original_id.push_back(node);
        }

        c_log << "Creating Index for Category: " << my_category << '\n';

        // Creating Index for nodes of specific category as unfiltered data
        int Rsmall_f = min(Rsmall, (int)this->categories[my_category].size() - 1);    // handle case when Rsmall > |Pf| - 1 for certain filters f
        if (!DGf.vamanaAlgorithm(Lsmall, Rsmall_f, a)){
            cout << "Something went wrong in vamana algorithm.\n";
            rv = false;
            return;
        }
        c_log << "Creating Index for Category: " << my_category << " created.\n";

        c_log << "Pruning.\n";
        for (Node<T>& node : DGf.nodes){
            DGf.robustPrune(node.id, DGf.Nout[node.id], a, Rstitched);
        }

        c_log << "Pruning complete.Stitching with main graph.\n";
        
        mx_merge.lock();
        // union of edges: this->Nout ∪= DGf.Nout
        for (pair<Id, unordered_set<Id>> edge : DGf.get_Nout()){
            Id from = edge.first;
            for (Id to : edge.second)
                this->addEdge(original_id[from], original_id[to]);
        }
        this->filteredMedoids[my_category] = original_id[DGf.medoid()];
        mx_merge.unlock();
        c_log << "Stitching successful.\n";

        // cleanup for next category
        original_id.clear();
        DGf.init();

        mx_category_index.lock();
    }
    mx_category_index.unlock();
}

template <typename T>
bool DirectedGraph<T>::_parallel_stitchedVamana(int Lstitched, int Rstitched, int Lsmall, int Rsmall, float a){

    // initialize G as an empty graph => clear all edges
    if(this->clearEdges() == false)
        return false;

    vector<thread> threads;
    vector<char> rvs;   // return values of threads - actually bool type

    // vector containing all unique categories
    vector<int> category_names;

    for (pair<int, unordered_set<Id>> cpair : this->categories)
        category_names.push_back(cpair.first);

    int category_index = 0;

    mutex mx_category_index, mx_merge;

    for (int i = 0; i < args.n_threads; i++){
        rvs.push_back(true);
        threads.push_back(thread(
            &DirectedGraph::_thread_stitchedVamana_fn,
            this,
            ref(Lstitched),
            ref(Rstitched),
            ref(Lsmall),
            ref(Rsmall),
            ref(a),
            ref(category_index),
            ref(mx_category_index),
            ref(mx_merge),
            ref(category_names),
            ref(rvs[i])));
    }

    for (thread& th : threads)
        th.join();

    for (bool rv : rvs){
        if (rv == false){
            c_log << "Something went wrong in the Stitched Vamana Index Creation.\n";
            return false;
        }
    }

    c_log << "Stitched Vamana Index Created.\n";
    return true;

}

template <typename T>
const Id DirectedGraph<T>::startingNode(optional<int> category){

    if (category == nullopt){  // category doesn't matter. Medoid or Sample from all nodes in graph
        if (args.randomStart) return sampleFromContainer(this->nodes).id;
        else return this->medoid();
    }
    else{   // category matters. Specific Category Medoid or Sample from specific category

        if (!mapKeyExists(category.value(), this->categories)) { throw invalid_argument("Category not found. No nodes of that category exist in the graph."); }

        if (args.randomStart) return sampleFromContainer(this->categories[category.value()]);
        else { unordered_map<int, Id> medoids = this->findMedoids(args.threshold);
                return medoids[category.value()]; }
    }
}