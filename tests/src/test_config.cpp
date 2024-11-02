#include <iostream>
#include "acutest.h"
#include "types.hpp"
#include "config.hpp"


void test_Store_and_Load(){
    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);

    // Add nodes to graph (10 nodes)
    vector<float> v1 = vector<float>{1.2f, 2.4f, 3.64f, 4.234f, 5.8f, 6.0f};
    vector<float> v2 = vector<float>{2.1f, 4.2f, 6.63f, 3.232f, 8.5f, 0.6f};
    vector<float> v3 = vector<float>{0.1f, 0.2f, 0.63f, 0.232f, 0.5f, 0.6f};
    vector<float> v4 = vector<float>{1.0f, 3.5f, 7.2f, 2.3f, 5.7f, 6.8f};
    vector<float> v5 = vector<float>{2.2f, 4.4f, 1.6f, 8.1f, 3.3f, 7.7f};
    vector<float> v6 = vector<float>{3.3f, 1.1f, 5.5f, 7.8f, 9.0f, 2.2f};
    vector<float> v7 = vector<float>{4.4f, 6.6f, 2.2f, 1.9f, 8.8f, 3.3f};
    vector<float> v8 = vector<float>{5.5f, 3.3f, 9.9f, 0.0f, 6.7f, 4.4f};
    vector<float> v9 = vector<float>{6.6f, 7.7f, 4.4f, 5.5f, 2.1f, 8.8f};
    vector<float> v10 = vector<float>{7.7f, 8.8f, 3.3f, 9.9f, 0.5f, 1.2f};

    // Create nodes in the graph
    vector<float> n1 = *DG.createNode(v1);
    vector<float> n2 = *DG.createNode(v2);
    vector<float> n3 = *DG.createNode(v3);
    vector<float> n4 = *DG.createNode(v4);
    vector<float> n5 = *DG.createNode(v5);
    vector<float> n6 = *DG.createNode(v6);
    vector<float> n7 = *DG.createNode(v7);
    vector<float> n8 = *DG.createNode(v8);
    vector<float> n9 = *DG.createNode(v9);
    vector<float> n10 = *DG.createNode(v10);



    DG.Rgraph(5);

    DG.store("graph_instance.txt");

    DirectedGraph<vector<float>> DG2(euclideanDistance<vector<float>>, vectorEmpty<float>);

    DG2.load("graph_instance.txt");

    TEST_CHECK(DG.get_n_edges() == DG2.get_n_edges());

    TEST_CHECK(DG.get_n_nodes() == DG2.get_n_nodes());

    set<vector<float>> nodes1 = DG.getNodes();
    set<vector<float>> nodes2 = DG2.getNodes();

    TEST_CHECK(nodes1 == nodes2);

    map<vector<float>, set<vector<float>>> m1 = DG.get_Nout();
    map<vector<float>, set<vector<float>>> m2 = DG2.get_Nout();

    TEST_CHECK(m1 == m2);

}

TEST_LIST = {
    { "test_Store_and_Load", test_Store_and_Load},
    { NULL, NULL }     // zeroed record marking the end of the list
};