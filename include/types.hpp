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


class Node{
/* Each node will contain the vector (value) and a unique identifier*/

    private:
        int id;                 // node identifier (used for order among nodes - sorted containers)
        vector<float> value;    // the feature vector of an entry
        set<Node*> Nout;         // Self-referencing container for out-neighbors of each node
        set<Node*> Nin;         // MAYBE TO BE USED

    public:

        // Constructor
        Node(){
            this->id = -1;
        }

        // Constructor
        Node(vector<float> v, int id){
            this->value = v;
            this->id = id;
            cout<< "Node created with id: "<< this->id << endl;
        }

        // Operator overloading for equality and comparator functions (required for stl set container)
        bool operator==(const Node& n) const { return this->getId() == n.getId(); }
        bool operator!=(const Node& n) const { return this->getId() != n.getId(); }
        bool operator<(const Node& n)  const { return this->getId() < n.getId();  }
        bool operator>(const Node& n)  const { return this->getId() > n.getId();  }

        // Value get/set functions
        vector<float> getValue() const{
            return this->value;
        }

        void setValue(vector<float> v){
            this->value = v;
        }

        set<Node*> getOutNeighbors() const{
            return this->Nout;
        }

        // inserts 'out' as an out-neighbor of this Node Object
        bool insertNeighbor(Node* out){
            if (out->getId() < 0) {
                cout << "ERROR: Invalid Node. Node ID is " << out->getId() <<".\n";
                return false;
            }
            // add a warning: if out in thisNout warning: already exists
            if (setIn(out, this->Nout)){
                cout << "WARNING: Node with ID " << out->getId() << "already exists as a neighbor to node " << this->getId() << "."; 
            }
            this->Nout.insert(out);

            out->Nin.insert(this);       // UNIT TEST FOR THIS


            return true;
        }

        bool removeNeighbor(Node* target){
            // find neighbor in out neighbors and delete it
            // return true on success
            return false;
        }

        int clearNeighbors(){

            int removed = 0;
            for (Node* n : this->Nout){
                this->removeNeighbor(n);
                removed++;
            }
            return removed;
        }

        // Id get/set funcitons
        int getId() const{      // getters are const as they do not modify the object. 
            return this->id;    // Required for type matching during overloaded comparison (less <) of Node Objects
        }

        // Set the Node's identifier
        void setId(int id){
            if (id < 0){
                cout << "WARNING: You are setting the Node's ID to " << id << ". Node will become invalid.\n";
            }
            this->id = id;
        }
};

class DirectedGraph{

    private:
        int lastId;         // id of the last added node
        int n_edges;        // number of edges present in the graph
        int n_nodes;
        set<Node*> nodes;    // a set of all the nodes in the graph

    public:

        // Constructor: Initialize an empty graph
        DirectedGraph() {
            this->lastId = 0;
            this->n_edges = 0;
            this->n_nodes = 0;
            cout << "Graph created!" << endl;
        }

        void display(){
            // empty
        }

        // getters
        int getLastId() const{
            return this->lastId;
        }

        set<Node*> getNodes() const{
            return this->nodes;
        }

        // Creates a node and adds it in the graph
        void createNode(vector<float> value){
            Node* newNode = new Node(value, ++this->lastId);
            nodes.insert(newNode);
            this->n_nodes++;
        }

        // bool remove node (by id by value idk) - search and delete
        // also remove node from any neighbor-lists!

        bool addEdge(Node* from, Node* to){
            if (!from->insertNeighbor(to))
                return false; 
            this->n_edges++;
            return true;
        }

        // remove edge
        bool removeEdge(Node& from, Node& to){
            // check if exists. Return True if exists and successfully removed
            // false otherwise
            return false;
        }

        // clears all edges in the graph
        bool clearEdges(){
            for (Node* n : this->nodes){
                this->n_edges -= n->clearNeighbors();
            }

            if (this->n_edges == 0)
                return true;
            
            cout << "ERROR: Edges not cleared successfully. Something went wrong.\n";
            return false;
        }

        // creates a random R graph with the existing nodes. Return TRUE if successful, FALSE otherwise
        bool Rgraph(int R){
            
            this->clearEdges();     // clear all edges in the graph to create an R random graph anew.

            if (R <= log(this->n_nodes)){
                cout << "WARNING: R <= logn and therefore the graph will not be well connected.\n";
            }
            if (R > this->n_nodes - 1){
                cout << "ERROR: R cannot exceed N-1 (N = the total number of nodes in the Graph)";
                return false;
            }

            int r;
            for (Node* n : this->getNodes()){
                
                set<Node*> remaining = this->getNodes(); // MUST BE COPY!

                for (int i = 0; i < min(R, this->n_nodes -1); i++){ // max_i = min(R, N-1) where N is the number of nodes - redundant check if check for false above ? (failsafe)
                    
                    Node* nr;
                    
                    if (!remaining.empty()){
                        do{ // CAREFUL: POSSIBLE INFINITE LOOP WHEN SET SIZE IS 1! (when sampling in a while loop)
                            nr = sampleFromSet(remaining);
                        }while(nr == n || setIn(nr, n->getOutNeighbors()));              // ensure the neighbor is not itself and not already in the set
                    }
                    
                    n->insertNeighbor(nr);                                               // add the node as neighbor
                    remaining.erase(nr);
                }
            }
            return true;
            // UPDATE to work with references instead of copies? MAYBE required change on n.neighbors to handle references of existing nodes instead of copies?
            // can set hold references or should the elements be copy-able?
        }

        // Greedy search algorithm
        vector<set<Node*>> greedySearch(Node* s, vector<float> xq, int k, int L);

        // Robust Prune algorithm

        // Vamana Graph creation algorithm
};