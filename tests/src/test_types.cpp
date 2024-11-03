#include "acutest.h"
#include "interface.hpp"

using namespace std;

void test_graphCreation(void){
    OMIT_OUTPUT;

    // Create a directed graph with euclidean distance as the distance function
    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>);

    // Address is not null
    TEST_ASSERT(&DG != NULL);
    TEST_MSG("Address is NULL");

    // Nodes is an empty unordered_set
    TEST_ASSERT(DG.getNodes().empty());
    TEST_MSG("Nodes (set) is non-empty on creation");

    // Number of nodes is zero
    TEST_ASSERT(DG.get_n_nodes() == 0);
    TEST_MSG("Number of nodes is non-zero on creation");

    // Number of edges is zero
    TEST_ASSERT(DG.get_n_edges() == 0);
    TEST_MSG("Number of edges is non-zero on creation");

}

void test_createNode(void){
    OMIT_OUTPUT;

    // Create graph (should work based on previous tests)
    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>);

    // Value for node
    vector<float> value = vector<float>{1.2f,2.4f,3.64f,4.234f,5.8f,6.0f};
    
    // Create a node inside the graph
    set<vector<float>>::iterator it = DG.createNode(value);

    // Check that node creation was successful
    TEST_ASSERT(DG.get_n_nodes() == 1);
    TEST_MSG("Node was not added successfully");
    
    // Ensure that nodeSet is non-empty
    TEST_ASSERT(!DG.getNodes().empty());
    TEST_MSG("NodeSet is empty after node creation");

    // Ensure that nodeSet includes the node
    vector<float> retValue = *it;
    TEST_ASSERT(retValue == value);
    TEST_MSG("Inserted node = Node");
    
}

void test_Edges(void){
    OMIT_OUTPUT;

    // Create graph (should work based on previous tests)
    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>);

    // Add nodes to graph (should work based on previous tests)
    vector<float> v1 = vector<float>{1.2f,2.4f,3.64f,4.234f,5.8f,6.0f};
    vector<float> v2 = vector<float>{2.1f,4.2f,6.63f,3.232f,8.5f,0.6f};
    vector<float> v3 = vector<float>{0.1f,0.2f,0.63f,0.232f,0.5f,0.6f};

    vector<float> n1 = *DG.createNode(v1);
    vector<float> n2 = *DG.createNode(v2);
    vector<float> n3 = *DG.createNode(v3);

    // Add edge
    TEST_ASSERT(DG.addEdge(n1, n2));

    // Verify that edge exists
    TEST_ASSERT(mapKeyExists(n1, DG.get_Nout()));
    TEST_MSG("Add edge, nout key does not exist");

    set<vector<float>> n1out = DG.get_Nout().at(n1);
    TEST_ASSERT(*n1out.begin() == n2);
    TEST_MSG("Add edge verification nout");
    
    DG.get_Nout();

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

    TEST_ASSERT(DG.get_n_edges() == 1);
    TEST_MSG("Failed to ddecrement n_edges");

    DG.removeEdge(n1,n3);
    TEST_ASSERT(!mapKeyExists(n1, DG.get_Nout()));
    TEST_MSG("Remove edge, failed to remove nout key");

    // CHECK SELF-LOOP (edge a -> a)
    TEST_ASSERT(DG.addEdge(n1,n1) == false);
    TEST_MSG("Add edge on same node should decline adding the edge");
    
}

void test_clear(void){
    OMIT_OUTPUT;

    // Create graph (should work based on previous tests)
    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>);

    // Add nodes to graph (should work based on previous tests)
    vector<float> v1 = vector<float>{1.2f,2.4f,3.64f,4.234f,5.8f,6.0f};
    vector<float> v2 = vector<float>{2.1f,4.2f,6.63f,3.232f,8.5f,0.6f};
    vector<float> v3 = vector<float>{0.1f,0.2f,0.63f,0.232f,0.5f,0.6f};
    vector<float> v4 = vector<float>{4.1f,4.2f,4.63f,4.232f,4.5f,4.6f};

    vector<float> n1 = *DG.createNode(v1);
    vector<float> n2 = *DG.createNode(v2);
    vector<float> n3 = *DG.createNode(v3);
    vector<float> n4 = *DG.createNode(v4);

    // Create edges between nodes (should work based on previous tests)
    DG.addEdge(n1, n2);
    DG.addEdge(n1, n3);
    DG.addEdge(n1, n4);

    DG.addEdge(n2, n1);
    DG.addEdge(n2, n3);

    TEST_ASSERT(DG.get_n_edges() == 5);
    TEST_MSG("Number of edges is wrong");

    // Check remove edge function for wrong arguments -> edge that does not exist
    TEST_ASSERT(!DG.removeEdge(n2,n4));
    TEST_MSG("Removed edge that does not exist");

    // Check remove edge function for appropriate args
    TEST_ASSERT(DG.removeEdge(n1,n2));

    TEST_ASSERT(mapKeyExists(n1,DG.get_Nout()));

    TEST_ASSERT(DG.clearNeighbors(n1));

    // Verify that Nout key is removed
    TEST_ASSERT(!mapKeyExists(n1, DG.get_Nout()));

    // Verify that number of edges is reduced to 2
    TEST_ASSERT(DG.get_n_edges() == 2);

    // Clear all edges in graph
    DG.clearEdges();

    // Verify that no in-out neighbors are present
    TEST_ASSERT(!mapKeyExists(n2, DG.get_Nout()));

    // Verify that counter of edges is reduced to 0
    TEST_ASSERT(DG.get_n_edges() == 0);

    // Verify that no harm is done when clearing edges when no edges are present 
    TEST_ASSERT(DG.clearEdges());
}

void test_medoid(void){
    OMIT_OUTPUT;

    // Create the graph
    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>);

    // Add nodes
    vector<float> v1 = {1.f, 1.f, 1.f, 1.f, 1.f, 1.f};
    vector<float> v2 = {2.f, 2.f, 2.f, 2.f, 2.f, 2.f};
    vector<float> v3 = {3.f, 3.f, 3.f, 3.f, 3.f, 3.f};
    vector<float> v4 = {4.f, 4.f, 4.f, 4.f, 4.f, 4.f};
    vector<float> v5 = {5.f, 5.f, 5.f, 5.f, 5.f, 5.f};

    // Test Case 1: Check for medoid in an empty graph
    TEST_EXCEPTION(DG.medoid(), invalid_argument);  // Should throw an exception for empty graph

    // Add nodes to the graph
    DG.createNode(v1);
    DG.createNode(v2);
    DG.createNode(v3);
    DG.createNode(v4);
    DG.createNode(v5);

    // Test Case 2: When only one or two nodes are present
    DirectedGraph<vector<float>> smallGraph(euclideanDistance<vector<float>>);
    smallGraph.createNode(v1);
    TEST_CHECK(smallGraph.medoid() == v1);  // Medoid should be v1 in single-node graph
    smallGraph.createNode(v2);
    TEST_CHECK(smallGraph.medoid() == v1);  // With two nodes, should return the first node

    // Test Case 3: Multiple nodes in the graph (check that medoid is calculated correctly)
    DG.addEdge(v1, v2);
    DG.addEdge(v1, v3);
    DG.addEdge(v1, v4);
    DG.addEdge(v2, v1);
    DG.addEdge(v2, v3);

    // Save the original value of N_THREADS
    #ifdef N_THREADS
        const int originalNThreads = N_THREADS; // Store original number of threads
    #else
        const int originalNThreads = 1; // Default if N_THREADS is not defined
    #endif

    // Test Serial Medoid
    #undef N_THREADS
    #define N_THREADS 1  // Set N_THREADS to 1 for serial execution
    vector<float> computedMedoidSerial = DG.medoid(); // Call medoid in serial mode
    TEST_CHECK(computedMedoidSerial == v3); // Check against expected medoid

    // Test Parallel Medoid
    #undef N_THREADS
    #define N_THREADS originalNThreads  // Restore original N_THREADS for parallel execution
    vector<float> computedMedoidParallel = DG.medoid(); // Call medoid in parallel mode
    TEST_CHECK(computedMedoidParallel == v3); // Check against expected medoid

    // Ensure both methods yield the same result
    TEST_CHECK(computedMedoidSerial == computedMedoidParallel); // Ensure both methods return the same result

    // Output the results for verification
    TEST_MSG("Expected medoid: [%f, %f, %f, %f, %f, %f], Got (Serial): [%f, %f, %f, %f, %f, %f] and Got (Parallel): [%f, %f, %f, %f, %f, %f]",
             v3[0], v3[1], v3[2], v3[3], v3[4], v3[5],
             computedMedoidSerial[0], computedMedoidSerial[1], computedMedoidSerial[2], computedMedoidSerial[3], computedMedoidSerial[4], computedMedoidSerial[5],
             computedMedoidParallel[0], computedMedoidParallel[1], computedMedoidParallel[2], computedMedoidParallel[3], computedMedoidParallel[4], computedMedoidParallel[5]);

    // dimension mismatch will not be tested, as we assume that all elements in the set must be able to be passed on to the given distance function without error.
    // this case is handled in the euclideanDistance unit test.
}

void test_Rgraph(void){
    OMIT_OUTPUT;

    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>);
    
    vector<float> v[100];
    vector<float> nodes[100];

    for (int i = 0; i < 100; i++){
        for (int j = 0; j < 128; j++){
            v[i].push_back((float)i);
        }
        nodes[i] = *DG.createNode(v[i]);
    }

    TEST_ASSERT(DG.get_n_nodes() == 100);

    // Default case
    TEST_CHECK(DG.Rgraph(10));
    TEST_CHECK(DG.get_n_edges() == 100*10);

    int c = 0;
    map<vector<float>, set<vector<float>>> m = DG.get_Nout();

    for (vector<float>& n : nodes){
        c+= m[n].size();
    }

    TEST_CHECK(c == 1000);

    // R == 0 <=> clear all edges
    TEST_CHECK(DG.Rgraph(0));
    TEST_CHECK(DG.get_n_edges() == 0);

    // if an edge already exists and R > 0, edges are cleared before new ones are added
    DG.addEdge(nodes[0], nodes[1]);
    TEST_ASSERT(DG.get_n_edges() == 1);
    TEST_CHECK(DG.Rgraph(10));
    TEST_CHECK(DG.get_n_edges() == 100*10);
    
    // if R == N-1 (this case might take some extra time)
    TEST_CHECK(DG.Rgraph(99));
    TEST_CHECK(DG.get_n_edges() == 100*99);

    // if R > N-1 fails
    try{
        TEST_CHECK(DG.Rgraph(100) == false);
        TEST_CHECK(false); // control should not reach here
    }catch(invalid_argument& ia){ TEST_CHECK(string(ia.what()) == "R cannot exceed N-1 (N = the total number of nodes in the Graph).\n"); }

    // if R < 0 fails
    try{
        TEST_CHECK(DG.Rgraph(-1) == false);
        TEST_CHECK(false); // control should not reach here
    }catch(invalid_argument& ia){ TEST_CHECK(string(ia.what()) == "R must be a positive integer.\n"); }
    

    // Consecutive use simply shuffles the edges
    // NOTE: This test MIGHT fail due to randomness.
    // Each of the 100 nodes can make one out of 99 possible connections => 99^{100} different ways (cycles are allowed to exist in the directed graph).
    // The probability for each specific set of edges (assuming uniform) is 1/99^{100}.
    // For this test to fail, it would mean that we drew the same number twice in a row from a uniform distribution among 99^{100} numbers.
    map<vector<float>, set<vector<float>>> before = DG.get_Nout();
    TEST_CHECK(DG.Rgraph(1));
    TEST_CHECK(DG.get_n_edges() == 100*1);
    map<vector<float>, set<vector<float>>> after = DG.get_Nout();
    TEST_CHECK(DG.Rgraph(1));
    TEST_CHECK(DG.get_n_edges() == 100*1);
    TEST_CHECK((before == after) == false); // map equality operator == is by default overloaded to them containing exactly the same items
    // https://en.cppreference.com/w/cpp/container/map/operator_cmp
}

void test_greedySearch(void){
    OMIT_OUTPUT;

    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);

    // default case

    // Read base vectors
    vector<vector<float>> vectors = read_vecs<float>("../data/siftsmall/siftsmall_base.fvecs", 10000);

    // Add nodes to graph
    for (vector<float>& v : vectors){
        DG.createNode(v);
    }
    // Verify that 10000 nodes have been added
    TEST_ASSERT(DG.get_n_nodes() == 10000);

    // Argument checks:

    // Empty starting node s
    vector<float> startingNode;
    try{
        vector<set<vector<float>>> ret = DG.greedySearch(startingNode, vectors[0], 4, 5);
        TEST_CHECK(false);  // Control should not reach here 
    }catch(invalid_argument& ia){ TEST_CHECK((string(ia.what()) == "No start node was provided.\n")); }
    
    // Starting node not in Graph's nodeSet
    for (int i=0; i<128; i++){
        startingNode.push_back(-i);
    }
    try {
        vector<set<vector<float>>> ret = DG.greedySearch(startingNode, vectors[0], 4, 5);
        TEST_CHECK(false);
    }catch(invalid_argument& ia){ TEST_CHECK((string(ia.what()) == "Starting node not in nodeSet.\n")); }

    // Empty query point xq
    startingNode = vectors[129];
    vector<float> xq;
    try{
        vector<set<vector<float>>> ret = DG.greedySearch(startingNode, xq, 4, 5);
        TEST_CHECK(false);  // Control should not reach here 
    }catch(invalid_argument& ia){ TEST_CHECK((string(ia.what()) == "No query was provided.\n")); }

    // if k <= 0
    try{
        vector<set<vector<float>>> ret = DG.greedySearch(startingNode, vectors[0], 0, 5);
        TEST_CHECK(false);  // Control should not reach here 
    }catch(invalid_argument& ia){ TEST_CHECK((string(ia.what()) == "K must be greater than 0.\n")); }

    // if L < k
    try{
        vector<set<vector<float>>> ret = DG.greedySearch(startingNode, vectors[0], 2, 1);
        TEST_CHECK(false);  // Control should not reach here 
    }catch(invalid_argument& ia){ TEST_CHECK((string(ia.what()) == "L must be greater or equal to K.\n")); }

    return;
}

void test_robustPrune(void){
    OMIT_OUTPUT;

    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);

    // Read base vectors
    vector<vector<float>> vectors = read_vecs<float>("../data/siftsmall/siftsmall_base.fvecs", 10000);

    // Add nodes to graph
    for (vector<float>& v : vectors){
        DG.createNode(v);
    }
    // Verify that 10000 nodes have been added
    TEST_ASSERT(DG.get_n_nodes() == 10000);
    set<vector<float>> nullset;

    // Valid
    DG.robustPrune(vectors[0], nullset, 1, 5);

    // Argument checks:

    // Empty Node
    vector<float> startingNode;
    try{
        DG.robustPrune(startingNode, nullset, 1, 5);
        TEST_CHECK(false);  // Control should not reach here 
    }catch(invalid_argument& ia){ TEST_CHECK((string(ia.what()) == "No node was provided.\n")); }

    // Node not in Graph's nodeSet
    for (int i=0; i<128; i++){
        startingNode.push_back(-i);
    }
    try{
        DG.robustPrune(startingNode, nullset, 1, 5);
        TEST_CHECK(false); // Control should not reach here
    }catch(invalid_argument& ia){ TEST_CHECK((string(ia.what()) == "Node not in nodeSet\n")); }
    // a < 1
    try{
        DG.robustPrune(vectors[43], nullset, 0, 5);
        TEST_CHECK(false);  // Control should not reach here 
    }catch(invalid_argument& ia){ TEST_CHECK((string(ia.what()) == "Parameter a must be >= 1.\n")); }

    // R <= 0
    try{
        // test goes here
        DG.robustPrune(vectors[43], nullset, 1, 0);
        TEST_CHECK(false);  // Control should not reach here 
    }catch(invalid_argument& ia){ TEST_CHECK((string(ia.what()) == "Parameter R must be > 0.\n")); }

    return;
}

void test_vamanaAlgorithm(void){
    OMIT_OUTPUT;

    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);

    // Argument checks

    // R <= 0
    try{
        TEST_CHECK(DG.vamanaAlgorithm(1,0,1));
        TEST_CHECK(false);  // Control should not reach here
    }catch(invalid_argument& ia){ TEST_CHECK(string(ia.what()) == "R must be a positive, non-zero integer.\n"); }

    // L <= 0
    try{
        TEST_CHECK(DG.vamanaAlgorithm(0,1,1));
        TEST_CHECK(false);  // Control should not reach here
    }catch(invalid_argument& ia){ TEST_CHECK(string(ia.what()) == "Parameter L must be >= 1.\n"); }

    // a < 1
    try{
        TEST_CHECK(DG.vamanaAlgorithm(1,1,0));
        TEST_CHECK(false);  // Control should not reach here
    }catch(invalid_argument& ia){ TEST_CHECK(string(ia.what()) == "Parameter a must be >= 1.\n"); }
    
    return;
}


TEST_LIST = {
    { "test_graphCreation", test_graphCreation },
    { "test_createNode", test_createNode },
    { "test_Edges", test_Edges },
    { "test_clear", test_clear },
    { "test_medoid", test_medoid},
    { "test_Rgraph", test_Rgraph},
    { "test_greedySearch", test_greedySearch},
    { "test_robustPrune", test_robustPrune},
    { "test_vamanaAlgorithm", test_vamanaAlgorithm},
    { NULL, NULL }     // zeroed record marking the end of the list
};