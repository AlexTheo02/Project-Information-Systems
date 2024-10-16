#pragma once

#include <iostream>
#include <iterator>

#include <list>
#include <unordered_map>
#include <vector>
#include <set>

#include "util.hpp"

using namespace std;

// Graph Documentation Here











// end


class Node{
/* Each node will contain the vector (value) and a unique identifier*/

    private:
        int id;                 // node identifier (used for order among nodes - sorted containers)
        vector<float> value;    // the feature vector of an entry
        set<Node> Nout;         // Self-referencing container for out-neighbors of each node

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

        set<Node> getOutNeighbors() const{
            return this->Nout;
        }

        // inserts 'out' as an out-neighbor of this Node Object
        bool insertNeighbor(Node out){
            if (out.getId() < 0) {
                cout << "ERROR: Invalid Node. Node ID is " << out.getId() <<".\n";
                return false;
            }
            // add a warning: if out in thisNout warning: already exists
            if (setIn(out, this->Nout)){
                cout << "WARNING: Node with ID " << out.getId() << "already exists as a neighbor to node " << this->getId() << "."; 
            }
            this->Nout.insert(out);
            return true;
        }

        bool removeNeighbor(Node target){
            // find neighbor in out neighbors and delete it
            // return true on success
            return false;
        }

        bool clearNeighbors(){
            // remove all out neighbors (= clear outgoing edges) 
            // return true on success
            return false;
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
        set<Node> nodes;    // a set of all the nodes in the graph

    public:

        // Constructor: Initialize an empty graph
        DirectedGraph() {
            this->lastId = 0;
            this->n_edges = 0;
            cout << "Graph created!" << endl;
        }

        void display(){
            // empty
        }

        // getters
        int getLastId() const{
            return this->lastId;
        }

        set<Node> getNodes() const{
            return this->nodes;
        }

        // Creates a node and adds it in the graph
        void createNode(vector<float> value){
            Node newNode = Node(value, ++this->lastId);
            nodes.insert(newNode);
        }

        bool addEdge(Node& from, Node& to){
            if (!from.insertNeighbor(to))
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
            // for node in this->nodes
            // if (node.clearNeighbors());
            this->n_edges = 0;
            return false;
        }

        // creates a random R graph with the existing nodes, granted there exist no edges in the graph. Return TRUE if successful, FALSE otherwise
        bool Rgraph(int R){
            // if any edges exist in the graph
            // if all(node.Nout.empty() for node in this->nodes)
            if (false){
                cout << "ERROR: Cannot create R random graph because edges already exist in the graph.\n";
                return false;
            }
            
        }

        // Greedy search algorithm
        vector<set<Node>> greedySearch(Node s, vector<float> xq, int k, int L);

        // Robust Prune algorithm

        // Vamana Graph creation algorithm
};