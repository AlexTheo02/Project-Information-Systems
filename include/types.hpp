#pragma once
#include "util.hpp"

#include <iostream>
#include <iterator>

#include <list>
#include <unordered_map>
#include <vector>
#include <set>

using namespace std;

class Node{
/* Each node will contain the vector (value) and a unique identifier*/

    private:
        int id;
        vector<float> value;
        set<Node> Nout;

    public:

        // Constructor
        Node(vector<float> v, int id){
            this->value = v;
            this->id = id;
            cout<< "Node created with id: "<< this->id << endl;
        }

        // Value get/set functions
        vector<float> getValue(){
            return this->value;
        }

        void setValue(vector<float> v){
            this->value = v;
        }

        set<Node> getOutNeighbors(){
            return this->Nout;
        }

        void insertNeighbor(Node out){
            this->Nout.insert(out);
        }

        // Id get/set funcitons
        int getId(){
            return this->id;
        }

        void setId(int id){
            this->id = id;
        }

        // Operator overloading
        bool operator==(Node n){
            return this->getId() == n.getId();
        }

        bool operator!=(Node n){
            return this->getId() != n.getId();
        }
};

// class G = (V,E) : V = Nodes, E = Edges, edges are directed => tuple(=struct) (v1,v2)

class DirectedGraph{

    private:
        int lastId;
        set<Node> nodes;

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
        vector<set<Node>> greedySearch(Node s, vector<float> xq, int k, int L){
            // Create empty sets
            set<Node> Lc,V;

            // Initialize Lc with s
            Lc.insert(s);
            
            set<Node> diff;
            while(!empty(diff = setSubtraction(Lc,V))){
                Node pmin = myArgMin(diff, xq, euclideanDistance);

                // Error checking 
                if (pmin.getId() < 0) { break; }

                Lc = setUnion(Lc, pmin.getOutNeighbors());
                V.insert(pmin);
            }

            if (Lc.size() > L){
                Lc = closestN(L, Lc, xq, euclideanDistance);    // function: find N closest points from a specific Xq from given set and return them
            }

            vector<set<Node>> ret;
            
            ret.insert(ret.begin(), closestN(k, Lc, xq, euclideanDistance));
            ret.insert(ret.end(), V);

            return ret;
        }
};