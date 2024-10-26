#include <iostream>
#include "acutest.h"
#include "types.hpp"

// void test_graphCreation(){

//     // Create a directed graph with euclidean distance as the distance function
//     DirectedGraph<Node> DG(euclideanDistance<Node>);

//     // Address is not null
//     TEST_ASSERT(&DG != NULL);
//     TEST_MSG("Address is NULL");

//     // Nodes is an empty set
//     TEST_ASSERT(DG.getNodes().empty());
//     TEST_MSG("Nodes (set) is non-empty on creation");

//     // Number of nodes is zero
//     TEST_ASSERT(DG.get_n_nodes() == 0);
//     TEST_MSG("Number of nodes is non-zero on creation");

//     // Number of edges is zero
//     TEST_ASSERT(DG.get_n_edges() == 0);
//     TEST_MSG("Number of edges is non-zero on creation");

// }

// void test_createNode(){

//     // Create graph (should work based on previous tests)
//     DirectedGraph<Node> DG(euclideanDistance<Node>);

//     // Value for node
//     vector<float> value = vector<float>{1.2f,2.4f,3.64f,4.234f,5.8f,6.0f};
    
//     // Create a node inside the graph
//     Node n = DG.createNode(value);

//     // Check that node creation was successful
//     TEST_ASSERT(DG.get_n_nodes() == 1);
//     TEST_MSG("Node was not added successfully");
    
//     // Ensure that nodeSet is non-empty
//     TEST_ASSERT(!DG.getNodes().empty());
//     TEST_MSG("NodeSet is empty after node creation");

//     // Ensure that nodeSet includes the node
//     set<Node>::iterator it = DG.getNodes().begin();
//     Node nset = *(it);
//     TEST_ASSERT(nset == n);
//     TEST_MSG("Inserted node = Node");

//     // TEST_ASSERT(*nset == *n);
//     // TEST_MSG("Values do not match");
    
// }

// void test_Edges(){

//     // Create graph (should work based on previous tests)
//     DirectedGraph<Node> DG(euclideanDistance<Node>);

//     // Add nodes to graph (should work based on previous tests)
//     vector<float> v1 = vector<float>{1.2f,2.4f,3.64f,4.234f,5.8f,6.0f};
//     vector<float> v2 = vector<float>{2.1f,4.2f,6.63f,3.232f,8.5f,0.6f};
//     vector<float> v3 = vector<float>{0.1f,0.2f,0.63f,0.232f,0.5f,0.6f};

//     Node n1 = DG.createNode(v1);
//     Node n2 = DG.createNode(v2);
//     Node n3 = DG.createNode(v3);
    
//     // Add edge
//     TEST_ASSERT(DG.addEdge(n1, n2));

//     // Verify that edge exists
//     TEST_ASSERT(mapKeyExists(n1, DG.get_Nout()));
//     TEST_MSG("Add edge, nout key does not exist");
    
//     TEST_ASSERT(mapKeyExists(n2, DG.get_Nin()));
//     TEST_MSG("Add edge, nin key does not exist");

//     set<Node> n1out = DG.get_Nout()[n1];
//     TEST_ASSERT(*n1out.begin() == n2);
//     TEST_MSG("Add edge verification nout");
    
//     set<Node> n2in = DG.get_Nin()[n2];
//     TEST_ASSERT(*n2in.begin() == n1);
//     TEST_MSG("Add edge verification nin");

//     TEST_ASSERT(DG.get_n_edges() == 1);
//     TEST_MSG("Failed to increment n_edges");

//     DG.addEdge(n1,n3);

//     TEST_ASSERT(DG.get_n_edges() == 2);
//     TEST_MSG("Failed to increment n_edges 2");

//     // Remove edge
//     DG.removeEdge(n1,n2);

//     // Remove non-existing edge, on self loop
//     TEST_ASSERT(DG.removeEdge(n1,n1) == false);

//     // Verify that edge has been removed
//     TEST_ASSERT(mapKeyExists(n1, DG.get_Nout()));
//     TEST_MSG("Remove edge, nout key removed");

//     TEST_ASSERT(!mapKeyExists(n2, DG.get_Nin()));
//     TEST_MSG("Remove edge, failed to remove nin");

//     TEST_ASSERT(DG.get_n_edges() == 1);
//     TEST_MSG("Failed to ddecrement n_edges");

//     DG.removeEdge(n1,n3);
//     TEST_ASSERT(!mapKeyExists(n1, DG.get_Nout()));
//     TEST_MSG("Remove edge, failed to remove nout key");

//     // CHECK SELF-LOOP (edge a -> a)
//     TEST_ASSERT(DG.addEdge(n1,n1) == false);
//     TEST_MSG("Add edge on same node should decline adding the edge");
    
// }

// void test_clear(){
//     // Create graph (should work based on previous tests)
//     DirectedGraph<Node> DG(euclideanDistance<Node>);

//     // Add nodes to graph (should work based on previous tests)
//     vector<float> v1 = vector<float>{1.2f,2.4f,3.64f,4.234f,5.8f,6.0f};
//     vector<float> v2 = vector<float>{2.1f,4.2f,6.63f,3.232f,8.5f,0.6f};
//     vector<float> v3 = vector<float>{0.1f,0.2f,0.63f,0.232f,0.5f,0.6f};
//     vector<float> v4 = vector<float>{4.1f,4.2f,4.63f,4.232f,4.5f,4.6f};

//     Node n1 = DG.createNode(v1);
//     Node n2 = DG.createNode(v2);
//     Node n3 = DG.createNode(v3);
//     Node n4 = DG.createNode(v4);

//     // Create edges between nodes (should work based on previous tests)
//     DG.addEdge(n1, n2);
//     DG.addEdge(n1, n3);
//     DG.addEdge(n1, n4);

//     DG.addEdge(n2, n1);
//     DG.addEdge(n2, n3);

//     TEST_ASSERT(DG.get_n_edges() == 5);
//     TEST_MSG("Number of edges is wrong");

//     DG.clearNeighbors(n1);

//     // Verify that Nout key is removed
//     TEST_ASSERT(!mapKeyExists(n1, DG.get_Nout()));

//     // Verify that number of edges is reduced to 2
//     TEST_ASSERT(DG.get_n_edges() == 2);

//     // Verify that n4 has no incoming nodes
//     TEST_ASSERT(DG.get_Nin()[n4].empty());

//     // Clear all edges in graph
//     DG.clearEdges();

//     // Verify that no in-out neighbors are present
//     TEST_ASSERT(!mapKeyExists(n2, DG.get_Nout()));

//     TEST_ASSERT(!mapKeyExists(n1, DG.get_Nin()));
//     TEST_ASSERT(!mapKeyExists(n2, DG.get_Nin()));
//     TEST_ASSERT(!mapKeyExists(n3, DG.get_Nin()));
//     TEST_ASSERT(!mapKeyExists(n4, DG.get_Nin()));

//     // Verify that counter of edges is reduced to 0
//     TEST_ASSERT(DG.get_n_edges() == 0);

//     // Verify that no harm is done when clearing edges when no edges are present 
//     TEST_ASSERT(DG.clearEdges());
// }

// void test_Rgraph(){

//     DirectedGraph<Node> DG(euclideanDistance<Node>);
    
//     vector<float> v[100];
//     Node nodes[100];

//     for (int i = 0; i < 100; i++){
//         for (int j = 0; j < 128; j++){
//             v[i].push_back((float)j);
//         }
//         nodes[i] = DG.createNode(v[i]);
//     }

//     // Default case
//     TEST_CHECK(DG.Rgraph(10));
//     TEST_CHECK(DG.get_n_edges() == 100*10);

//     // R == 0 <=> clear all edges
//     TEST_CHECK(DG.Rgraph(0));
//     TEST_CHECK(DG.get_n_edges() == 0);

//     // if an edge already exists and R > 0, edges are cleared before new ones are added
//     DG.addEdge(nodes[0], nodes[1]);
//     TEST_ASSERT(DG.get_n_edges() == 1);
//     TEST_CHECK(DG.Rgraph(10));
//     TEST_CHECK(DG.get_n_edges() == 100*10);
    
//     // if R == N-1 (this case might take some extra time)
//     TEST_CHECK(DG.Rgraph(99));
//     TEST_CHECK(DG.get_n_edges() == 100*99);

//     // if R > N-1 fails
//     TEST_CHECK(DG.Rgraph(100) == false);

//     // if R < 0 fails
//     TEST_CHECK(DG.Rgraph(-1) == false);

//     // Consecutive use simply shuffles the edges
//     // NOTE: This test MIGHT fail due to randomness.
//     // Each of the 100 nodes can make one out of 99 possible connections => 99^{100} different ways (cycles are allowed to exist in the directed graph).
//     // The probability for each specific set of edges (assuming uniform) is 1/99^{100}.
//     // For this test to fail, it would mean that we drew the same number twice in a row from a uniform distribution among 99^{100} numbers.
//     unordered_map<Node, set<Node>> before = DG.get_Nout();
//     TEST_CHECK(DG.Rgraph(1));
//     TEST_CHECK(DG.get_n_edges() == 100*1);
//     unordered_map<Node, set<Node>> after = DG.get_Nout();
//     TEST_CHECK(DG.Rgraph(1));
//     TEST_CHECK(DG.get_n_edges() == 100*1);
//     TEST_CHECK((before == after) == false); // unordered_map equality operator == is by default overloaded to them containing exactly the same items
//     // https://en.cppreference.com/w/cpp/container/unordered_map/operator_cmp
// }


TEST_LIST = {
    // { "test_graphCreation", test_graphCreation },
    // { "test_createNode", test_createNode },
    // { "test_Edges", test_Edges },
    // { "test_clear", test_clear },
    // { "test_Rgraph", test_Rgraph},
    { NULL, NULL }     // zeroed record marking the end of the list
};