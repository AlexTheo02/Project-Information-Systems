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
typedef vector<float>* Node;
// same vector = DUPLICATE??

// Graph Documentation Here
// Attributes required for Constructor are marked with *
// + Number of Edges
// + Number of Nodes
// + Set containing all nodes in the graph
// + Dictionary{node : OutNeighbors}
// + Dictionary{node : InNeighbors}
// + *Distance Function: Node x Node -> float
class DirectedGraph{

    private:
        int n_edges;        // number of edges present in the graph
        int n_nodes;
        set<Node> nodes;    // a set of all the nodes in the graph
        unordered_map<Node, set<Node>> Nout;// key: node, value: set of outgoing neighbors 
        unordered_map<Node, set<Node>> Nin; // key: node, value: set of incoming neighbors
        function<float(Node, Node)> d;      // Graph's distance function

    public:

        // Constructor: Initialize an empty graph
        DirectedGraph(function<float(Node, Node)> distance_function) {
            this->n_edges = 0;
            this->n_nodes = 0;
            cout << "Graph created!" << endl;
            this->d = distance_function;
        }

        void display(){
            // empty
        }

        // Return a set of all Nodes in the graph
        set<Node> getNodes() {
            return this->nodes;
        }

        // Creates a node and adds it in the graph
        void createNode(vector<float> value);

        // bool remove node (by id by value idk) - search and delete    //TODO!
        // also remove node from any neighbor-lists!

        // Adds an directed edge (from->to). Updates outNeighbors(from) and inNeighbors(to)
        void addEdge(Node from, Node to);

        // remove edge
        bool removeEdge(Node from, Node to);

        // clears all neighbors for a specific node
        bool clearNeighbors(Node node);

        // clears all edges in the graph
        bool clearEdges();

        // creates a random R graph with the existing nodes. Return TRUE if successful, FALSE otherwise
        bool Rgraph(int R);

        // Greedy search algorithm
        vector<set<Node>> greedySearch(Node s, vector<float> xq, int k, int L);

        // Robust Prune algorithm
        void robustPrune(Node p, set<Node> V, int a, int R);

        // Vamana Graph creation algorithm
};