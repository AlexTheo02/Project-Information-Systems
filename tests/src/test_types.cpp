#include "acutest.h"
#include "interface.hpp"

using namespace std;

void test_graphCreation(void){
    

    // Create a directed graph with euclidean distance as the distance function
    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);

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

    // Create graph (should work based on previous tests)
    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);

    // Value for node
    vector<float> value = vector<float>{1.2f,2.4f,3.64f,4.234f,5.8f,6.0f};
    
    // Create a node inside the graph
    int id = DG.createNode(value);

    // Check that node creation was successful
    TEST_ASSERT(DG.get_n_nodes() == 1);
    TEST_MSG("Node was not added successfully");
    
    // Ensure that nodeSet is non-empty
    TEST_ASSERT(!DG.getNodes().empty());
    TEST_MSG("NodeSet is empty after node creation");

    // Ensure that nodeSet includes the node
    vector<float> retValue = DG.getNodes()[id].value;
    TEST_ASSERT(retValue == value);
    TEST_MSG("Inserted node = Node");
    
}

void test_Edges(void){ 

    // Create graph (should work based on previous tests)
    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);

    // Add nodes to graph (should work based on previous tests)
    vector<float> v1 = vector<float>{1.2f,2.4f,3.64f,4.234f,5.8f,6.0f};
    vector<float> v2 = vector<float>{2.1f,4.2f,6.63f,3.232f,8.5f,0.6f};
    vector<float> v3 = vector<float>{0.1f,0.2f,0.63f,0.232f,0.5f,0.6f};

    int id1 = DG.createNode(v1);
    int id2 = DG.createNode(v2);
    int id3 = DG.createNode(v3);

    // Add edge
    TEST_ASSERT(DG.addEdge(id1, id2));

    // Verify that edge exists
    TEST_ASSERT(mapKeyExists(id1, DG.get_Nout()));
    TEST_MSG("Add edge, nout key does not exist");

    unordered_set<int> n1out = DG.get_Nout().at(id1);
    TEST_ASSERT(*n1out.begin() == id2);
    TEST_MSG("Add edge verification nout");
    
    DG.get_Nout();

    TEST_ASSERT(DG.get_n_edges() == 1);
    TEST_MSG("Failed to increment n_edges");

    DG.addEdge(id1,id3);

    TEST_ASSERT(DG.get_n_edges() == 2);
    TEST_MSG("Failed to increment n_edges 2");

    // Remove edge
    DG.removeEdge(id1,id2);

    // Remove non-existing edge, on self loop
    TEST_ASSERT(DG.removeEdge(id1,id1) == false);

    // Verify that edge has been removed
    TEST_ASSERT(mapKeyExists(id1, DG.get_Nout()));
    TEST_MSG("Remove edge, nout key removed");

    TEST_ASSERT(DG.get_n_edges() == 1);
    TEST_MSG("Failed to ddecrement n_edges");

    DG.removeEdge(id1,id3);
    TEST_ASSERT(!mapKeyExists(id1, DG.get_Nout()));
    TEST_MSG("Remove edge, failed to remove nout key");

    // CHECK SELF-LOOP (edge a -> a)
    TEST_ASSERT(DG.addEdge(id1,id1) == false);
    TEST_MSG("Add edge on same node should decline adding the edge");
    
}

void test_clear(void){

    // Create graph (should work based on previous tests)
    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);

    // Add nodes to graph (should work based on previous tests)
    vector<float> v1 = vector<float>{1.2f,2.4f,3.64f,4.234f,5.8f,6.0f};
    vector<float> v2 = vector<float>{2.1f,4.2f,6.63f,3.232f,8.5f,0.6f};
    vector<float> v3 = vector<float>{0.1f,0.2f,0.63f,0.232f,0.5f,0.6f};
    vector<float> v4 = vector<float>{4.1f,4.2f,4.63f,4.232f,4.5f,4.6f};

    int id1 = DG.createNode(v1);
    int id2 = DG.createNode(v2);
    int id3 = DG.createNode(v3);
    int id4 = DG.createNode(v4);

    // Create edges between nodes (should work based on previous tests)
    DG.addEdge(id1, id2);
    DG.addEdge(id1, id3);
    DG.addEdge(id1, id4);

    DG.addEdge(id2, id1);
    DG.addEdge(id2, id3);

    TEST_ASSERT(DG.get_n_edges() == 5);
    TEST_MSG("Number of edges is wrong");

    // Check remove edge function for wrong arguments -> edge that does not exist
    TEST_ASSERT(!DG.removeEdge(id2,id4));
    TEST_MSG("Removed edge that does not exist");

    // Check remove edge function for appropriate args
    TEST_ASSERT(DG.removeEdge(id1,id2));

    TEST_ASSERT(mapKeyExists(id1,DG.get_Nout()));

    TEST_ASSERT(DG.clearNeighbors(id1));

    // Verify that Nout key is removed
    TEST_ASSERT(!mapKeyExists(id1, DG.get_Nout()));

    // Verify that number of edges is reduced to 2
    TEST_ASSERT(DG.get_n_edges() == 2);

    // Clear all edges in graph
    DG.clearEdges();

    // Verify that no in-out neighbors are present
    TEST_ASSERT(!mapKeyExists(id2, DG.get_Nout()));

    // Verify that counter of edges is reduced to 0
    TEST_ASSERT(DG.get_n_edges() == 0);

    // Verify that no harm is done when clearing edges when no edges are present 
    TEST_ASSERT(DG.clearEdges());
}

void test_medoid(void){  

    // Create the graph
    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);

    // Add nodes
    vector<float> v1 = {1.f, 1.f, 1.f, 1.f, 1.f, 1.f};
    vector<float> v2 = {2.f, 2.f, 2.f, 2.f, 2.f, 2.f};
    vector<float> v3 = {3.f, 3.f, 3.f, 3.f, 3.f, 3.f};
    vector<float> v4 = {4.f, 4.f, 4.f, 4.f, 4.f, 4.f};
    vector<float> v5 = {5.f, 5.f, 5.f, 5.f, 5.f, 5.f};

    // ------------------------------------------------------------------------------------------- Empty graph check
    TEST_EXCEPTION(DG.medoid(), invalid_argument);  // Should throw an exception for empty graph

    // ------------------------------------------------------------------------------------------- Graph with one or two nodes check
    DirectedGraph<vector<float>> smallGraph(euclideanDistance<vector<float>>, vectorEmpty<float>);
    smallGraph.createNode(v1);
    TEST_CHECK(smallGraph.medoid().value == v1);  // Medoid should be v1 in single-node graph

    DirectedGraph<vector<float>> smallGraph2(euclideanDistance<vector<float>>, vectorEmpty<float>);
    smallGraph2.createNode(v1);
    smallGraph2.createNode(v2);
    TEST_CHECK(smallGraph2.medoid().value == v1);  // With two nodes, should return the first node

    // ------------------------------------------------------------------------------------------- Graph with multiple nodes

    // Create a second graph
    DirectedGraph<vector<float>> DG2(euclideanDistance<vector<float>>, vectorEmpty<float>);

    // Add the same nodes to the two graphs
    int id_11 = DG.createNode(v1);
    int id_12 = DG.createNode(v2);
    int id_13 = DG.createNode(v3);
    int id_14 = DG.createNode(v4);
    int id_15 = DG.createNode(v5);

    int id_21 = DG2.createNode(v1);
    int id_22 = DG2.createNode(v2);
    int id_23 = DG2.createNode(v3);
    int id_24 = DG2.createNode(v4);
    int id_25 = DG2.createNode(v5);
    
    // Add the same edges to the two graphs
    DG.addEdge(id_11, id_12);
    DG.addEdge(id_11, id_13);
    DG.addEdge(id_11, id_14);
    DG.addEdge(id_12, id_11);
    DG.addEdge(id_12, id_13);

    DG2.addEdge(id_21, id_22);
    DG2.addEdge(id_21, id_23);
    DG2.addEdge(id_21, id_24);
    DG2.addEdge(id_22, id_21);
    DG2.addEdge(id_22, id_23);

    // Test Serial Medoid
    N_THREADS = 1;  // Set N_THREADS to 1 for serial execution
    vector<float> computedMedoidSerial = DG.medoid().value; // Call medoid in serial mode
    TEST_CHECK(computedMedoidSerial == v3); // Check against expected medoid

    // Test Parallel Medoid
    N_THREADS = 8;  // Set N_THREADS to 8 for parallel execution
    vector<float> computedMedoidParallel = DG2.medoid().value; // Call medoid in parallel mode
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

    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);
    
    vector<float> v[100];
    int nodes[100];

    for (int i = 0; i < 100; i++){
        for (int j = 0; j < 128; j++){
            v[i].push_back((float)i);
        }
        nodes[i] = DG.createNode(v[i]);
    }

    TEST_ASSERT(DG.get_n_nodes() == 100);

    // Default case
    TEST_CHECK(DG.Rgraph(10));
    TEST_CHECK(DG.get_n_edges() == 100*10);

    int c = 0;
    unordered_map<int, unordered_set<int>> m = DG.get_Nout();

    for (int n : nodes){
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
     unordered_map<int, unordered_set<int>> before = DG.get_Nout();
    TEST_CHECK(DG.Rgraph(1));
    TEST_CHECK(DG.get_n_edges() == 100*1);
     unordered_map<int, unordered_set<int>> after = DG.get_Nout();
    TEST_CHECK(DG.Rgraph(1));
    TEST_CHECK(DG.get_n_edges() == 100*1);
    TEST_CHECK((before == after) == false); // map equality operator == is by default overloaded to them containing exactly the same items
    // https://en.cppreference.com/w/cpp/container/map/operator_cmp
}


// void test_myArgMin(void){   

//     set<vector<float>> s;

//     // default case
//     s = {
//         (vector<float>) {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
//         (vector<float>) {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
//         (vector<float>) {2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f},
//         (vector<float>) {3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f},
//         (vector<float>) {4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f},
//         (vector<float>) {5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f},
//         (vector<float>) {6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f},
//     };

//     vector<float> xq = {2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f};

//     vector<float> xmin = myArgMin<vector<float>>(s, xq, euclideanDistance<vector<float>>, vectorEmpty<float>);
//     vector<float> ymin = {2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f};
//     TEST_CHECK(xmin == ymin);


//     // empty arguments check:

//     // empty query, non empty unordered_set
//     xq.clear();
//     try{
//         xmin = myArgMin<vector<float>>(s, xq, euclideanDistance<vector<float>>, vectorEmpty<float>);
//         TEST_CHECK(false);  // control should not reach here
//     }catch(invalid_argument &ia){ TEST_CHECK(string(ia.what()) == "Query container is empty.\n"); }

//     // empty unordered_set, non empty query
//     xq = {2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f};
//     s.clear();
//     try{
//         xmin = myArgMin<vector<float>>(s, xq, euclideanDistance<vector<float>>, vectorEmpty<float>);
//         TEST_CHECK(false);  // control should not reach here
//     }catch(invalid_argument &ia){ TEST_CHECK(string(ia.what()) == "Set is Empty.\n"); }

//     // both empty
//     xq.clear();
//     try{
//         xmin = myArgMin<vector<float>>(s, xq, euclideanDistance<vector<float>>, vectorEmpty<float>);
//         TEST_CHECK(false);  // control should not reach here
//     }catch(invalid_argument &ia){ TEST_CHECK(string(ia.what()) == "Set is Empty.\n"); }

// }

// void test_closestN(void){    

//     int N = 5;
//     set<vector<float>> s;

//     // Create 1000 vectors of dimension 128 with values [i, i, ..., i]
//     for (int i = 0; i < 1000; i++) {
//         vector<float> v(128, static_cast<float>(i));
//         s.insert(v);
//     }

//     // Query vector [327.3f, 327.3f, ..., 327.3f]
//     vector<float> x(128, 327.3f);

//     // Expected closest vectors: [325, 326, 327, 328, 329]
//     set<vector<float>> yclosest;
//     for (int i = 0; i < 5; i++) {
//         vector<float> v(128, static_cast<float>(325 + i));
//         yclosest.insert(v);
//     }
//     set<vector<float>> xclosest = closestN<vector<float>>(N, s, x, euclideanDistance<vector<float>>, vectorEmpty<float>);

//     TEST_CHECK(xclosest == yclosest);

//     // argument checks:
    
//     // N > |s| returns the whole set
//     N = 1001; // |s| = 1000
//     TEST_CHECK(closestN<vector<float>>(N, s, x, euclideanDistance<vector<float>>, vectorEmpty<float>) == s);

//     // N < 0
//     try{
//         xclosest = closestN<vector<float>>(-1, s, x, euclideanDistance<vector<float>>, vectorEmpty<float>);
//         TEST_CHECK(false);  // Control should not reach here
//     }catch(invalid_argument& ia){ TEST_CHECK(string(ia.what()) == "N must be greater than 0.\n"); }

//     // N == 0
//     N = 0;
//     set<vector<float>> nullset;
//     TEST_CHECK(closestN<vector<float>>(N, s, x, euclideanDistance<vector<float>>, vectorEmpty<float>) == nullset);

//     // set is empty
//     TEST_CHECK(closestN<vector<float>>(N, nullset, x, euclideanDistance<vector<float>>, vectorEmpty<float>) == nullset);

//     // Empty query
//     x.clear();  // Clear the query vector

//     try{
//         xclosest = closestN<vector<float>>(N, s, x, euclideanDistance<vector<float>>, vectorEmpty<float>);
//         TEST_CHECK(false);  // Control should not reach here
//     }catch(invalid_argument& ia){ 
//         // Check if the caught exception message matches the expected message
//         TEST_CHECK(string(ia.what()) == "Query X is empty.\n");
//     }



//     // both set and query empty
//     TEST_CHECK(closestN<vector<float>>(N, nullset, x, euclideanDistance<vector<float>>, vectorEmpty<float>) == nullset);

//     // case with ties
//     s = {
//         (vector<float>) {1,1,1,1,1},
//         (vector<float>) {2,2,2,2,2},
//         (vector<float>) {3,3,3,3,3},
//         (vector<float>) {4,4,4,4,4},
//         (vector<float>) {5,5,5,5,5}
//     };
//     x = {2.5f, 2.5f, 2.5f, 2.5f, 2.5f};
//     N = 3;
    
//     yclosest = {                        // because of ascending sort, takes the smallest value that is closest to the reference point x first.
//         (vector<float>) {1,1,1,1,1},
//         (vector<float>) {2,2,2,2,2},
//         (vector<float>) {3,3,3,3,3},
//     };
//     xclosest = closestN<vector<float>>(N, s, x, euclideanDistance<vector<float>>, vectorEmpty<float>);
//     TEST_CHECK(xclosest == yclosest);
// }















void test_greedySearch(void){ 

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
    Node<vector<float>> startingNode;
    try{
        pair<unordered_set<int>, unordered_set<int>> ret = DG.greedySearch(startingNode, vectors[0], 4, 5);
        TEST_CHECK(false);  // Control should not reach here 
    }catch(invalid_argument& ia){ TEST_CHECK((string(ia.what()) == "No start node was provided.\n")); }
    
    // Starting node not in Graph's nodeSet
    for (int i=0; i<128; i++){
        startingNode.value.push_back(-i);
    }
    try {
        pair<unordered_set<int>, unordered_set<int>> ret = DG.greedySearch(startingNode, vectors[0], 4, 5);
        TEST_CHECK(false);
    }catch(invalid_argument& ia){ TEST_CHECK((string(ia.what()) == "Starting node not in nodeSet.\n")); }

    // Empty query point xq
    startingNode = DG.getNodes()[129];
    vector<float> xq;
    try{
        pair<unordered_set<int>, unordered_set<int>> ret = DG.greedySearch(startingNode, xq, 4, 5);
        TEST_CHECK(false);  // Control should not reach here 
    }catch(invalid_argument& ia){ TEST_CHECK((string(ia.what()) == "No query was provided.\n")); }

    // if k <= 0
    try{
        pair<unordered_set<int>, unordered_set<int>> ret = DG.greedySearch(startingNode, vectors[0], 0, 5);
        TEST_CHECK(false);  // Control should not reach here 
    }catch(invalid_argument& ia){ TEST_CHECK((string(ia.what()) == "K must be greater than 0.\n")); }

    // if L < k
    try{
        pair<unordered_set<int>, unordered_set<int>> ret = DG.greedySearch(startingNode, vectors[0], 2, 1);
        TEST_CHECK(false);  // Control should not reach here 
    }catch(invalid_argument& ia){ TEST_CHECK((string(ia.what()) == "L must be greater or equal to K.\n")); }

    return;
}

void test_robustPrune(void){  

    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);

    // Read base vectors
    vector<vector<float>> vectors = read_vecs<float>("../data/siftsmall/siftsmall_base.fvecs", 10000);

    // Add nodes to graph
    for (vector<float>& v : vectors){
        DG.createNode(v);
    }
    // Verify that 10000 nodes have been added
    TEST_ASSERT(DG.get_n_nodes() == 10000);
    unordered_set<int> nullset;

    // Valid
    DG.robustPrune(DG.getNodes()[0], nullset, 1, 5);

    // Argument checks:

    // Empty Node
    Node<vector<float>> startingNode;
    try{
        DG.robustPrune(startingNode, nullset, 1, 5);
        TEST_CHECK(false);  // Control should not reach here 
    }catch(invalid_argument& ia){ TEST_CHECK((string(ia.what()) == "No node was provided.\n")); }

    // Node not in Graph's nodeSet
    for (int i=0; i<128; i++){
        startingNode.value.push_back(-i);
    }
    try{
        DG.robustPrune(startingNode, nullset, 1, 5);
        TEST_CHECK(false); // Control should not reach here
    }catch(invalid_argument& ia){ TEST_CHECK((string(ia.what()) == "Node not in nodeSet\n")); }
    // a < 1
    try{
        DG.robustPrune(DG.getNodes()[43], nullset, 0, 5);
        TEST_CHECK(false);  // Control should not reach here 
    }catch(invalid_argument& ia){ TEST_CHECK((string(ia.what()) == "Parameter a must be >= 1.\n")); }

    // R <= 0
    try{
        // test goes here
        DG.robustPrune(DG.getNodes()[43], nullset, 1, 0);
        TEST_CHECK(false);  // Control should not reach here 
    }catch(invalid_argument& ia){ TEST_CHECK((string(ia.what()) == "Parameter R must be > 0.\n")); }

    return;
}

void test_vamanaAlgorithm(void){   

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
    // { "test_myArgMin", test_myArgMin },
    // { "test_closestN", test_closestN },
    { "test_Rgraph", test_Rgraph},
    { "test_greedySearch", test_greedySearch},
    { "test_robustPrune", test_robustPrune},
    { "test_vamanaAlgorithm", test_vamanaAlgorithm},
    { NULL, NULL }     // zeroed record marking the end of the list
};