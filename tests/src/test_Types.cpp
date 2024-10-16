#include <iostream>
#include "acutest.h"
#include "types.hpp"
#include "util_types.hpp"


void test_node(){

    vector<float> v;

    for (int i = 0; i < 128; i++)
        v.push_back(i);

    Node n1(v,1),n2(v,2);

    // test getters for constructor fields
    TEST_CHECK(n1.getId() == 1);
    TEST_CHECK(n1.getValue() == v);

    // test setters 
    n1.setId(2);
    TEST_CHECK(n1.getId() == 2);
    n1.setId(1);

    vector<float> v1;
    v1.push_back(1); v1.push_back(2); v1.push_back(3);
    n1.setValue(v1);
    TEST_CHECK(n1.getValue() == v1);

    // test operator overloading for class Node
    TEST_CHECK(n1 < n2);
    TEST_CHECK(n2 > n1);
    TEST_CHECK(n1 != n2);
    n2.setId(1);
    TEST_CHECK(n1 == n2);
    n2.setId(2);

    // test adding neighbors and neighbor getter
    TEST_CHECK(n1.insertNeighbor(n2));
    TEST_CHECK(n1.insertNeighbor(n2));  // also check for uniqueness in the get set
    set<Node> out = n1.getOutNeighbors();
    set<Node> known_out; known_out.insert(n2);
    TEST_CHECK(out.size() == 1);
    TEST_CHECK(out == known_out);

    // insert n2, change n2, is neighbor set changed?
    n2.setId(3);
    n1.insertNeighbor(n2);
    known_out.insert(n2);
    out = n1.getOutNeighbors();     // refresh neighbors
    TEST_CHECK(out.size() == 2);
    TEST_CHECK(out == known_out);


    // inserting invalid node as neighbor
    n2.setId(-1);
    n1.insertNeighbor(n2);
    out = n1.getOutNeighbors();     // refresh neighbors
    TEST_CHECK(out.size() == 2);
    TEST_CHECK(out == known_out);

    // ADD TESTS AFTER ADDING FUNCTIONALITY FOR:
    // REMOVE NEIGHBOR (search and delete)
    // CLEAR NEIGHBORS

}

void test_graph(){

    DirectedGraph g;

    vector<float> v1,v2,v3,v4;
    for (int i = 0; i < 128; i++){
        v1.push_back(i);
        v2.push_back(i);    // same value is still differentiable, equality is based on id (???)
        v3.push_back(3*i);
        v4.push_back(4*i);
    }

    g.createNode(v1);
    g.createNode(v2);
    g.createNode(v3);
    g.createNode(v4);

    set<Node> nodes = g.getNodes();
    TEST_CHECK(nodes.size() == 4);

    // add neighbors

    // clear edges

    // R graph creation (ASSERT NO MORE THAN R OUTGOING EDGES FOR EACH NODE) - NOT YET IMPLEMENTED
}


TEST_LIST = {
    { "test_node", test_node },
    { "test_graph", test_graph},
    { NULL, NULL }     // zeroed record marking the end of the list
};