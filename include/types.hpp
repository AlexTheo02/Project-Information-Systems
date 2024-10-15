#pragma once

#include <iostream>
#include <iterator>

#include <list>
#include <unordered_map>
#include <vector>
#include <set>

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

        // Operator overloading for equality and comparator functions (required for stl set container - std::less = '<')
        bool operator==(const Node& n) const {
            return this->getId() == n.getId();
        }

        bool operator<(const Node& n) const{
            return this->getId() < n.getId();
        }

        // Constructor
        Node(vector<float> v, int id){
            this->value = v;
            this->id = id;
            cout<< "Node created with id: "<< this->id << endl;
        }

        // Value get/set functions
        vector<float> getValue() const{     // const or not?
            return this->value;
        }

        void setValue(vector<float> v){
            this->value = v;
        }

        set<Node> getOutNeighbors() const{        // const or not?
            return this->Nout;
        }

        // inserts 'out' as an out-neighbor of this Node Object
        void insertNeighbor(Node out){
            this->Nout.insert(out);
        }

        // Id get/set funcitons
        int getId() const{      // getters are const as they do not modify the object. 
            return this->id;    // Required for type matching during overloaded comparison (less <) of Node Objects
        }

        // Sets the Node's identifier
        void setId(int id){
            this->id = id;
        }
};

class DirectedGraph{

    private:
        int lastId;         // id of the last added node
        set<Node> nodes;    // a set of all the nodes in the graph

    public:

        // Constructor: Initialize an empty graph
        DirectedGraph() {
            this->lastId = 0;
            cout << "Graph created!" << endl;
        }

        void display(){
            // empty
        }

        // Creates a node in the graph
        void createNode(vector<float> value){
            Node newNode = Node(value, ++this->lastId);
            nodes.insert(newNode);
        }

        void addEdge(Node& from, Node& to){
            from.insertNeighbor(to); 
        }

        // Greedy search algorithm
        vector<set<Node>> greedySearch(Node s, vector<float> xq, int k, int L);

        // Robust Prune algorithm

        // Vamana Graph creation algorithm
};