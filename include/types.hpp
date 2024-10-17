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

// Graph Documentation Here











// end


// Handle for nodes
typedef vector<float>* Node;
// same vector = DUPLICATE??

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
        DirectedGraph() {
            this->n_edges = 0;
            this->n_nodes = 0;
            cout << "Graph created!" << endl;
        }

        void display(){
            // empty
        }

        // Return a set of all Nodes in the graph
        set<Node> getNodes() {
            return this->nodes;
        }

        // Creates a node and adds it in the graph
        void createNode(vector<float> value){
            // Create new node
            Node n = new vector<float>(value);

            // Add it to graph's set of nodes
            this->nodes.insert(n);

            // Increment the number of nodes in graph
            this->n_nodes++;
        }

        // bool remove node (by id by value idk) - search and delete
        // also remove node from any neighbor-lists!

        void addEdge(Node from, Node to){
            this->Nout[from].insert(to);
            this->Nin[to].insert(from);
            this->n_edges++;
        }

        // remove edge
        bool removeEdge(Node from, Node to){
            // Check if key exists before accessing it (and removing it)
            if (mapKeyExists(from, this->Nout) && mapKeyExists(to, this->Nin)) {
                // Key exists, access the value, if successfully removed, return true
                if (this->Nout[from].erase(to) && this->Nin[to].erase(from)){
                    
                    // Check if outgoing neighbors are empty, if so, remove entry from unordered map
                    if (this->Nout[from].empty())
                        this->Nout.erase(from);

                    // Check if incoming neighbors are empty, if so, remove entry from unordered map
                    if (this->Nin[to].empty())
                        this->Nin.erase(to);
                    
                    // Decrement the number of edges in graph
                    this->n_edges--;
                    return true;
                }
            }
            return false;
        }

        // clears all neighbors for a specific node
        bool clearNeighbors(Node node){

            // Check if node exists before trying to access it
            if (setIn(node,this->nodes)){
                cout << "ERROR: Node does not exist" << endl;
                return false;
            }

            // Node has outgoing neighbors
            if (mapKeyExists(node, this->Nout)){
                for (Node n : this->Nout[node]){
                    if (!this->removeEdge(node,n)){
                        cout << "ERROR: Failed to remove edge, something went wrong" << endl;
                        return false;
                    }
                }
                // Check if node has been removed from neighbors map
                if (mapKeyExists(node, this->Nout)){
                    cout << "ERROR: Something went wrong when clearing neighbors" << endl;
                    return false;
                }
            }
            return true;
        }

        // clears all edges in the graph
        bool clearEdges(){
            for (Node n : this->nodes){
                if (!this->clearNeighbors(n)){
                    cout << "ERROR: Failed to clear neighbors for node" << endl;
                    return false;
                }
            }

            if (this->n_edges != 0 || !this->Nout.empty() || !this->Nin.empty()){
                cout << "ERROR: Failed to clear edges in graph" << endl;
                return false;
            }
            
            return true;
        }

        // creates a random R graph with the existing nodes. Return TRUE if successful, FALSE otherwise
        bool Rgraph(int R){
            
            // clear all edges in the graph to create an R random graph anew.
            if (!this->clearEdges())
                return false;

            if (R <= log(this->n_nodes)){
                cout << "WARNING: R <= logn and therefore the graph will not be well connected.\n";
            }
            if (R > this->n_nodes - 1){
                cout << "ERROR: R cannot exceed N-1 (N = the total number of nodes in the Graph)";
                return false;
            }

            int r;
            for (Node n : this->getNodes()){
                
                // Copy of nodes in the graph
                set<Node> remaining = this->getNodes();
                // Remove self from remaining nodes
                if (remaining.erase(n) <= 0){
                    cout << "ERROR: Failed to remove self from remaining nodes" << endl;
                    return false;
                }

                for (int i = 0; i < R; i++){
                    
                    Node nr;

                    nr = sampleFromSet(remaining);
                    
                    this->addEdge(n,nr); // add the node as neighbor
                    remaining.erase(nr);
                }
            }
            return true;
        }

        // Greedy search algorithm
        vector<set<Node>> greedySearch(Node s, vector<float> xq, int k, int L);

        // Robust Prune algorithm
        void robustPrune(Node p, set<Node> V, int a, int R);

        // Vamana Graph creation algorithm
};