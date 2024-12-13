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

    // what is a good value of T for filteredMedoids?
    vector<Node<T>> perm = permutation(this->nodes);
    

    for (Node<T>& si : perm){
        // Id starting_node_i = this->startingNode(); // st[si.category];   // because each node belongs to at most one category.
        // unordered_map<int, Id> st = this->findMedoids(t);  // paper says starting points should be the medoids found in [algorithm 2]
        

        // create query with si value to pass to filteredGreedySearch
        Query<T> q(si.id, si.category, true, si.value, this->isEmpty);

        unordered_set<Id> Vi = this->filteredGreedySearch(this->startingNode(q.category), q, 0, L).second;

        filteredRobustPrune(si.id, Vi, a, R);

        if (mapKeyExists(si.id, this->Nout)){
        
            for (const Id j : this->Nout[si.id]){  // for every neighbor j of si

                this->addEdge(j, si.id);   // does it in either case (simpler code, robust prune clears all neighbors after copying to candidate set V anyway)
                if (this->Nout[j].size() > R)
                    filteredRobustPrune(j, this->Nout[j], a, R);
            }
        }
    }
    return true;
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
    
    int arg_placeholder_random1_or_medoid0 = 0;

    if (category == nullopt){  // category doesn't matter. Medoid or Sample from all nodes in graph
        if (args.randomStart) return sampleFromContainer(this->nodes).id;
        else return this->medoid();
    }
    else{   // category matters. Specific Category Medoid or Sample from specific category

        if (!mapKeyExists(category.value(), this->categories)) { throw invalid_argument("Category not found. No nodes of that category exist in the graph."); }

        if (args.randomStart) return sampleFromContainer(this->categories[category.value()]);
        else return this->findMedoids(args.threshold).at(category.value());
    }
}