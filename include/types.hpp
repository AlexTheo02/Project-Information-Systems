#pragma once

#include <iostream>
#include <iterator>
#include <cstdlib>  // for rand()
#include <ctime>    // for rand seed (not yet used DO IT)
#include <list>
#include <unordered_map>
#include <vector>
#include <set>
#include <cmath>

#include "util.hpp"

using namespace std;

// Handle for nodes
typedef vector<float> Node;
// same vector = DUPLICATE??

// Graph Documentation Here
// Attributes required for Constructor are marked with *
// + Number of Edges
// + Number of Nodes
// + Set containing all nodes in the graph
// + Dictionary{node : OutNeighbors}
// + Dictionary{node : InNeighbors}
// + *Distance Function: Node x Node -> float
template <typename T>
class DirectedGraph{

    private:
        int n_edges;        // number of edges present in the graph
        int n_nodes;
        set<T> nodes;    // a set of all the nodes in the graph
        unordered_map<T, set<T>> Nout;// key: node, value: set of outgoing neighbors 
        unordered_map<T, set<T>> Nin; // key: node, value: set of incoming neighbors
        function<float(T, T)> d;      // Graph's distance function

    public:

        // Constructor: Initialize an empty graph
        DirectedGraph(function<float(T, T)> distance_function) {
            this->n_edges = 0;
            this->n_nodes = 0;
            this->d = distance_function;
            cout << "Graph created!" << endl;
        }

        void display(){
            // empty
        }

        // Return a set of all Nodes in the graph
        set<Node> getNodes() {
            return this->nodes;
        }

        // Return the number of edges in the graph
        int get_n_edges(){
            return this->n_edges;
        }

        // Return the number of nodes in the graph
        int get_n_nodes(){
            return this->n_nodes;
        }

        // Return Nout map
        unordered_map<T, set<T>> get_Nout(){
            return this->Nout;
        }

        // Return Nin map
        unordered_map<T, set<T>> get_Nin(){
            return this->Nin;
        }

        // Creates a node, adds it in the graph and returns it
        typename set<T>::iterator createNode(T value);

        // bool remove node (by id by value idk) - search and delete    //TODO!
        // also remove node from any neighbor-lists!

        // Adds an directed edge (from->to). Updates outNeighbors(from) and inNeighbors(to)
        bool addEdge(T from, T to);

        // remove edge
        bool removeEdge(T from, T to);

        // clears all neighbors for a specific node
        bool clearNeighbors(T node);

        // clears all edges in the graph
        bool clearEdges();

        // creates a random R graph with the existing nodes. Return TRUE if successful, FALSE otherwise
        bool Rgraph(int R);

        // Greedy search algorithm
        vector<set<T>> greedySearch(T s, T xq, int k, int L);

        // Robust Prune algorithm
        bool robustPrune(T p, set<T> V, float a, int R);

        // Vamana Graph creation algorithm
        bool vamanaAlgorithm(int L, int R);
};