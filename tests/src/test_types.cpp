#include <iostream>
#include "acutest.h"
#include "types.hpp"
#include "util_types.hpp"

void test_graphCreation(){

    // Create a directed graph with euclidean distance as the distance function
    DirectedGraph DG = DirectedGraph(euclideanDistance);

    // Address is not null
    TEST_ASSERT(&DG != NULL);
    TEST_MSG("Address is NULL");

    // Nodes is an empty set
    TEST_ASSERT(DG.getNodes().empty());
    TEST_MSG("Nodes (set) is non-empty on creation");

    // Number of nodes is zero
    TEST_ASSERT(DG.get_n_nodes() == 0);
    TEST_MSG("Number of nodes is non-zero on creation");

    // Number of edges is zero
    TEST_ASSERT(DG.get_n_edges() == 0);
    TEST_MSG("Number of edges is non-zero on creation");

}

void test_createNode(){

    // Create graph (should work based on previous tests)
    DirectedGraph DG = DirectedGraph(euclideanDistance);

    // Value for node
    vector<float> value = vector<float>{1.2f,2.4f,3.64f,4.234f,5.8f,6.0f};
    
    // Create a node inside the graph
    Node n = DG.createNode(value);

    // Check that node creation was successful
    TEST_ASSERT(DG.get_n_nodes() == 1);
    TEST_MSG("Node was not added successfully");
    
    // Ensure that nodeSet is non-empty
    TEST_ASSERT(!DG.getNodes().empty());
    TEST_MSG("NodeSet is empty after node creation");

    // Ensure that nodeSet includes the node
    set<Node>::iterator it = DG.getNodes().begin();
    Node nset = *(it);
    TEST_ASSERT(nset == n);
    TEST_MSG("Inserted node = Node");

    TEST_ASSERT(*nset == *n);
    TEST_MSG("Values do not match");
    
}

void test_Edges(){

    // Create graph (should work based on previous tests)
    DirectedGraph DG = DirectedGraph(euclideanDistance);

    // Add nodes to graph (should work based on previous tests)
    vector<float> v1 = vector<float>{1.2f,2.4f,3.64f,4.234f,5.8f,6.0f};
    vector<float> v2 = vector<float>{2.1f,4.2f,6.63f,3.232f,8.5f,0.6f};
    vector<float> v3 = vector<float>{0.1f,0.2f,0.63f,0.232f,0.5f,0.6f};

    Node n1 = DG.createNode(v1);
    Node n2 = DG.createNode(v2);
    Node n3 = DG.createNode(v3);
    
    // Add edge
    TEST_ASSERT(DG.addEdge(n1, n2));

    // Verify that edge exists
    TEST_ASSERT(mapKeyExists(n1, DG.get_Nout()));
    TEST_MSG("Add edge, nout key does not exist");
    
    TEST_ASSERT(mapKeyExists(n2, DG.get_Nin()));
    TEST_MSG("Add edge, nin key does not exist");

    set<Node> n1out = DG.get_Nout()[n1];
    TEST_ASSERT(*n1out.begin() == n2);
    TEST_MSG("Add edge verification nout");
    
    set<Node> n2in = DG.get_Nin()[n2];
    TEST_ASSERT(*n2in.begin() == n1);
    TEST_MSG("Add edge verification nin");

    TEST_ASSERT(DG.get_n_edges() == 1);
    TEST_MSG("Failed to increment n_edges");

    DG.addEdge(n1,n3);

    TEST_ASSERT(DG.get_n_edges() == 2);
    TEST_MSG("Failed to increment n_edges 2");

    // Remove edge
    DG.removeEdge(n1,n2);

    // Remove non-existing edge, on self loop
    TEST_ASSERT(DG.removeEdge(n1,n1) == false);

    // Verify that edge has been removed
    TEST_ASSERT(mapKeyExists(n1, DG.get_Nout()));
    TEST_MSG("Remove edge, nout key removed");

    TEST_ASSERT(!mapKeyExists(n2, DG.get_Nin()));
    TEST_MSG("Remove edge, failed to remove nin");

    TEST_ASSERT(DG.get_n_edges() == 1);
    TEST_MSG("Failed to ddecrement n_edges");

    DG.removeEdge(n1,n3);
    TEST_ASSERT(!mapKeyExists(n1, DG.get_Nout()));
    TEST_MSG("Remove edge, failed to remove nout key");

    // CHECK SELF-LOOP (edge a -> a)
    TEST_ASSERT(DG.addEdge(n1,n1) == false);
    TEST_MSG("Add edge on same node should decline adding the edge");
    
}

void test_clear(){
    // Create graph (should work based on previous tests)
    DirectedGraph DG = DirectedGraph(euclideanDistance);

    // Add nodes to graph (should work based on previous tests)
    vector<float> v1 = vector<float>{1.2f,2.4f,3.64f,4.234f,5.8f,6.0f};
    vector<float> v2 = vector<float>{2.1f,4.2f,6.63f,3.232f,8.5f,0.6f};
    vector<float> v3 = vector<float>{0.1f,0.2f,0.63f,0.232f,0.5f,0.6f};
    vector<float> v4 = vector<float>{4.1f,4.2f,4.63f,4.232f,4.5f,4.6f};

    Node n1 = DG.createNode(v1);
    Node n2 = DG.createNode(v2);
    Node n3 = DG.createNode(v3);
    Node n4 = DG.createNode(v4);

    // Create edges between nodes (should work based on previous tests)
    DG.addEdge(n1, n2);
    DG.addEdge(n1, n3);
    DG.addEdge(n1, n4);

    DG.addEdge(n2, n1);
    DG.addEdge(n2, n3);

    TEST_ASSERT(DG.get_n_edges() == 5);
    TEST_MSG("Number of edges is wrong");

    DG.clearNeighbors(n1);

    // Verify that Nout key is removed
    TEST_ASSERT(!mapKeyExists(n1, DG.get_Nout()));

    // Verify that number of edges is reduced to 2
    TEST_ASSERT(DG.get_n_edges() == 2);

    // Verify that n4 has no incoming nodes
    TEST_ASSERT(DG.get_Nin()[n4].empty());

    // Clear all edges in graph
    DG.clearEdges();

    // Verify that no in-out neighbors are present
    TEST_ASSERT(!mapKeyExists(n2, DG.get_Nout()));

    TEST_ASSERT(!mapKeyExists(n1, DG.get_Nin()));
    TEST_ASSERT(!mapKeyExists(n2, DG.get_Nin()));
    TEST_ASSERT(!mapKeyExists(n3, DG.get_Nin()));
    TEST_ASSERT(!mapKeyExists(n4, DG.get_Nin()));

    // Verify that counter of edges is reduced to 0
    TEST_ASSERT(DG.get_n_edges() == 0);

    // Verify that no harm is done when clearing edges when no edges are present 
    TEST_ASSERT(DG.clearEdges());
}


TEST_LIST = {
    { "test_graphCreation", test_graphCreation },
    { "test_createNode", test_createNode },
    { "test_Edges", test_Edges },
    { "test_clear", test_clear },
    { NULL, NULL }     // zeroed record marking the end of the list
};