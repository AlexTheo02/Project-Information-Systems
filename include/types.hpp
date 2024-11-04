#pragma once

#include <iostream>
#include <iterator>
#include <cstdlib>
#include <map>
#include <vector>
#include <set>
#include <cmath>
#include <functional>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <thread>
#include <list>

#include "util.hpp"

using namespace std;

// This file includes forward declarations for type definitions.
// Their implementations are on a separate .hpp file
// They are all linked together in the interface.hpp file

// Directed Graph Class Template:
// This implementation of a Directed Graph Class makes use of dictionaries/maps for adjacency lists.
// To instantiate such a Directed Graph Object, you will need to specify the Content Type T, as well as provide:
// 1. Distance Function: T x T -> float <=> float distance_function(T,T)
// 2. (Optional) Content type T Valid Check Function: T -> bool <=> bool isEmpty(T) (Default is an AlwaysValid function that returns false for any input)
//
// IMPORTANT: If you are planning to use DirectedGraph::store and DirectedGraph::load methods to save an instance of your DirectedGraph class,
// you must have the operators "<<" and ">>" overloaded FOR I/O OPERATIONS for your specific content type T.
// See more on: https://stackoverflow.com/questions/476272/how-can-i-properly-overload-the-operator-for-an-ostream
//              https://stackoverflow.com/questions/69803296/overloading-istream-operator
// Such overloads already exist in the config.hpp file. More instructions for the implementation can be found there. 
template <typename T>
class DirectedGraph{

    private:
        int n_edges;                            // number of edges present in the graph
        int n_nodes;                            // number of nodes present in the graph
        set<T> nodes;                           // a set containing all the nodes in the graph
        vector<T> _nodes;                       // vector representation of nodes used temporarily for medoid (if parallel approach is chosen)
        T _medoid;                              // used to avoid recalculation of medoid if we want to access it more than once
        map<T, set<T>> Nout;                    // key: node, value: set of outgoing neighbors 
        function<float(const T&, const T&)> d;  // Graph's distance function
        function<bool(const T&)> isEmpty;       // typename T valid check

        // implements medoid function using serial programming.
        const T _serial_medoid(void);

        // Implements medoid function using parallel programming with threads. Concurrency is set by the global constant N_THREADS.
        const T _parallel_medoid(void);

        // Thread function for parallel medoid. Work inside the range defined by [start_index, end_index). Update minima by reference for the merging of the results.
        void _thread_medoid_fn(int start_index, int end_index, T& local_minimum, float& local_dmin);
    
    public:

        // Constructor: Initialize an empty graph
        DirectedGraph(function<float(const T&, const T&)> distance_function, function<bool(const T&)> is_Empty = alwaysValid<T>, vector<T> values = {}) {
            this->n_edges = 0;
            this->n_nodes = 0;
            this->d = distance_function;
            this->isEmpty = is_Empty;
            this->_medoid.clear();
            this->_nodes.clear();
            c_log << "Graph created!" << '\n';

            for (const T& value : values){
                this->createNode(value);
            }
        }

        // Return a set of all Nodes in the graph
        const set<T>& getNodes() const { return this->nodes; }

        // Return the number of edges in the graph
        const int& get_n_edges() const { return this->n_edges; }

        // Return the number of nodes in the graph
        const int& get_n_nodes() const { return this->n_nodes; }

        // Return Nout map
        const map<T, set<T>>& get_Nout() const { return this->Nout; }

        // Creates a node, adds it in the graph and returns it
        typename set<T>::iterator createNode(const T& value);

        // Adds an directed edge (from->to). Updates outNeighbors(from) and inNeighbors(to)
        bool addEdge(const T& from, const T& to);

        // Remove edge
        bool removeEdge(const T& from, const T& to);

        // Clears all neighbors for a specific node
        bool clearNeighbors(const T& node);

        // Clears all edges in the graph
        bool clearEdges(void);

        // Calculates the medoid of the nodes in the graph based on the given distance function
        const T medoid(void);

        // creates a random R graph with the existing nodes. Return TRUE if successful, FALSE otherwise
        bool Rgraph(int R);

        // Greedily searches the graph for the k nearest neighbors of query xq (in an area of size L), starting the search from the node s.
        // Returns a set with the k closest neighbors (returned_vector[0]) and a set of all visited nodes (returned_vector[1]).
        const vector<set<T>> greedySearch(const T& s, T xq, int k, int L);

        // Prunes out-neighbors of node p up until a minimum threshold R of out-neighbors for node p, based on distance criteria with parameter a.
        void robustPrune(const T& p, set<T> V, float a, int R);

        // Transforms the graph into a Directed Graph such that it makes the finding of nearest neighbors easier.
        // Parameters:
        // + R the out-degree of each node in the graph (R >= 1)
        // + L the area parameter for searching (L >= k >= 1, where k is the desired number of neighbors)
        // + a the parameter for robust pruning (a >=1)
        bool vamanaAlgorithm(int L, int R, float a);


        // Stores the current state of a graph into the specified file.
        // IMPORTANT: makes use of overloaded << operator to store the graph into a file.
        void store(const string& filename) const;

        // Loads a graph state from the specified file. A Graph instance must already be instantiated with the appropriate distance and isEmpty functions.
        // IMPORTANT: makes use of overloaded >> operator to load the graph from a file
        void load(const string& filename);
};