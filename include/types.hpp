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
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <optional>

#include "util.hpp"

using namespace std;

// This file includes forward declarations for type definitions.
// Their implementations are on a separate .hpp file
// They are all linked together in the interface.hpp file


template <typename T>
class DirectedGraph;    // forward declaration for nodes and queries

struct Id {
    int value;
    Id() : value(-1) {}
    Id(int newValue) : value(newValue) {}

    operator int() const { return value; } // Used to convert id to int in array indexing

    Id& operator=(int newValue);                // assignment
    bool operator==(const Id& other) const;     // equality with Id
    bool operator==(const int other) const;     // equality with int
    bool operator<(const Id& other) const;      // less with Id
    bool operator<(const int other) const;      // less with in
    bool operator>=(const int other) const;     // geq with int
};

// Specialize std::hash for the Id struct (required because Id is used in unordered_containers(set, map) that are implemented with hash tables)
namespace std { // Hash based on the integer value
    template <> struct hash<Id> { size_t operator()(const Id& id) const noexcept { return hash<int>()(id.value);} };
}                                                                       /* actual hash implementation = hash with id.value (hash<int>)*/


// Node class
template <typename T>
class Node{
    
    public:
        Id id;
        int category;
        T value;
        function<bool(const T&)> isEmpty;   // pointer to the isEmpty method

        // Constructor
        Node(Id id = -1, int category = -1, T value = {}, function<bool(const T&)> isEmpty = alwaysEmpty<T>){
            this->id = id;
            this->category = category;
            this->value = value;
            this->isEmpty = isEmpty;
        }

        bool operator<(const Node& n) const;
        bool operator==(const Node& n) const;

        bool empty();
};

// Query class
template <typename T>
class Query{

    public:
        Id id;
        int category;
        bool filtered;   // 0(false) = ANN, 1(true) = ANN where Node.category == Query.category
        T value;
        function<bool(const T&)> isEmpty;   // pointer to the isEmpty method

        // Constructor
        Query(Id id = -1, int category = -1, bool fil = false, T value = {}, function<bool(const T&)> isEmpty = alwaysEmpty<T>){
            this->id = id;
            this->filtered = fil;
            this->category = category;
            this->value = value;
            this->isEmpty = isEmpty;
        }

        bool operator<(const Query& q);
        bool operator==(const Query& q);

        bool empty();
};

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
        int n_edges;                                        // number of edges present in the graph
        int n_nodes;                                        // number of nodes present in the graph
        vector<Node<T>> nodes;                              // vector containing all the nodes in the graph
        Id _medoid;                                         // medoid node's id. Used to avoid recalculation of medoid if we want to access it more than once
        unordered_map<int, Id> filteredMedoids;             // map containing each category key and its corresponding medoid node
        unordered_map<int, unordered_set<Id>> categories;   // a map containing all unique categories in the data and their corresponding nodes that belong to each
        unordered_map<Id, unordered_set<Id>> Nout;          // key: node, value: set of outgoing neighbors 
        function<float(const T&, const T&)> d;              // Graph's distance function
        function<bool(const T&)> isEmpty;                   // typename T valid check

        // implements medoid function using serial programming.
        const Id _serial_medoid(vector<Node<T>>& nodes);

        // Implements medoid function using parallel programming with threads. Concurrency is set by the argument args.n_threads.
        const Id _parallel_medoid(vector<Node<T>>& nodes);

        // Thread function for parallel medoid. Work inside the range defined by [start_index, end_index). Update minima by reference for the merging of the results.
        void _thread_medoid_fn(vector<Node<T>>& nodes, int start_index, int end_index, Id& local_minimum, float& local_dmin);

        // Thread function for parallel querying.
        void _thread_findQueryNeighbors_fn(vector<T>& queries, mutex& mx_query_index, int& query_index, vector<unordered_set<Id>>& returnVec);
    
    public:

        // Constructor: Initialize an empty graph
        DirectedGraph(function<float(const T&, const T&)> distance_function, function<bool(const T&)> is_Empty, vector<T> values = {}, bool isFiltered = false) {
            this->n_edges = 0;
            this->n_nodes = 0;
            this->d = distance_function;
            this->isEmpty = is_Empty;
            c_log << "Graph created!" << '\n';

            if (isFiltered){
                for (const T& value : values){
                    int category = value[0];
                    // Ignore the first 2 dimensions
                    T newValue(value.begin() + 2, value.end());
                    this->createNode(newValue, category);
                }
            }
            else{
                for (const T& value : values){
                    this->createNode(value);
                }
            }
        }

        // Return a set of all Nodes in the graph
        vector<Node<T>>& getNodes() { return this->nodes; }

        // Return the number of edges in the graph
        const int& get_n_edges() const { return this->n_edges; }

        // Return the number of nodes in the graph
        const int& get_n_nodes() const { return this->n_nodes; }

        // Return Nout map
        const unordered_map<Id, unordered_set<Id>>& get_Nout() const { return this->Nout; }

        // Creates a node, adds it in the graph and returns it
        Id createNode(const T& value, int category = -1);

        // Adds a directed edge (from->to). Updates outNeighbors(from) and inNeighbors(to)
        bool addEdge(const Id from, const Id to);

        // Remove edge
        bool removeEdge(const Id from, const Id to);

        // Clears all neighbors for a specific node
        bool clearNeighbors(const Id id);

        // Clears all edges in the graph
        bool clearEdges(void);

        // Calculates the medoid of the nodes in the graph based on the given distance function
        const Id medoid(optional<vector<Node<T>>> nodes_arg = nullopt, optional<bool> update_stored = nullopt);

        // calculates the Filtered Medoids
        const unordered_map<int, Id> findMedoids(float threshold);

        // implements filtered medoid function using serial programming.
        const unordered_map<int, Id> _filtered_medoid(float threshold);

        // returns the Id of the node in nodeSet which is closest to the point t, using the distance function provided
        Id _myArgMin(const unordered_set<Id>& nodeSet, T t);

        // returns a set with the Ids of the N nodes in set S which are closest to point X
        unordered_set<Id> _closestN(int N, const unordered_set<Id>& S, T X);

        // Returns a filtered set
        unordered_set<Id> filterSet(unordered_set<Id> S, int filter);

        // creates a random R graph with the existing nodes. Return TRUE if successful, FALSE otherwise
        bool Rgraph(int R);

        // Greedily searches the graph for the k nearest neighbors of query xq (in an area of size L), starting the search from the node s.
        // Returns a set with the k closest neighbors (returned.first) and a set of all visited nodes (returned.second).
        const pair<unordered_set<Id>, unordered_set<Id>> greedySearch(Id s, T xq, int k, int L);

        // Returns a set with the k closest neighbors (returned.first) and a set of all visited nodes (returned.second).
        const pair<unordered_set<Id>, unordered_set<Id>> filteredGreedySearch(unordered_set<Id>& S, Query<T> q, int k, int L);

        // Prunes out-neighbors of node p up until a minimum threshold R of out-neighbors for node p, based on distance criteria with parameter a.
        void robustPrune(Id p, unordered_set<Id> V, float a, int R);

        // Prunes same-category out-neighbors of node p up until a minimum threshold R of out-neighbors for node p, based on distance criteria with parameter a.
        void filteredRobustPrune(Id p, unordered_set<Id> V, float a, int R);

        // Transforms the graph into a Directed Graph such that it makes the finding of nearest neighbors easier.
        // Parameters:
        // + R the out-degree of each node in the graph (R >= 1)
        // + L the area parameter for searching (L >= k >= 1, where k is the desired number of neighbors)
        // + a the parameter for robust pruning (a >=1)
        bool vamanaAlgorithm(int L, int R, float a);

        // performs the filtered vamana algorithm to create the filtered index
        // float t threshold is a value in (0,1] that represents a fraction of the data in a specific category to be accounted for when searching for the medoid of that specific category
        bool filteredVamanaAlgorithm(int L, int R, float a, float t); // + t = threshold.

        // Performs the stitched vamana algorithm to create the filtered index
        bool stitchedVamanaAlgorithm(int Lstitched, int Rstitched, int Lsmall, int Rsmall, float a);


        // Stores the current state of a graph into the specified file.
        // IMPORTANT: makes use of overloaded << operator to store the graph into a file.
        void store(const string& filename) const;

        // Loads a graph state from the specified file. A Graph instance must already be instantiated with the appropriate distance and isEmpty functions.
        // IMPORTANT: makes use of overloaded >> operator to load the graph from a file
        void load(const string& filename);

        // Initializes the Graph as if it has been instantiated just now (resets everything to default except constructor arguments)
        void init();


        // Based on the qiven vector, the function returns the query's neighbors
        unordered_set<Id> findNeighbors(Query<T> q);

        // Returns the neighbors of all queries found in the given queries_path file.
        // If the file is .vecs format read_arg corresponds to the number of queries and, if the file is in .bin format, it corresponds to the dimension of the query vector
        vector<unordered_set<Id>> findQueriesNeighbors(int read_arg = -1);

};