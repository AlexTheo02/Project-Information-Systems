#include "acutest.h"
#include "interface.hpp"

using namespace std;

void clearFileContents(const std::string& filename) {

    // Open the file in write mode with truncation
    ofstream file(filename, ios::trunc);
    if (!file) {
        cerr << "Error opening file: " << filename << endl;
    }
    file.close(); // Close the file
}

void test_Store_and_Load(){

    string filename = "graph_instance.txt";

    // Create a DG
    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);

    // Create a second directed graph to compare the two
    DirectedGraph<vector<float>> DG2(euclideanDistance<vector<float>>, vectorEmpty<float>);

    // ------------------------------------------------------------------------------------------- Empty graphs check

    // Store the graph in a textfile
    DG.store(filename);

    // Load the previously stored data of the first graph into the second
    DG2.load(filename);

    // Check if the number of the edges are equal
    TEST_CHECK(DG.get_n_edges() == DG2.get_n_edges());

    // Check if the number of the nodes are equal
    TEST_CHECK(DG.get_n_nodes() == DG2.get_n_nodes());

    // Retrieve the nodes of the two DGs
    vector<Node<vector<float>>> nodes1 = DG.getNodes();
    vector<Node<vector<float>>> nodes2 = DG2.getNodes();

    // Compare the two nodes if they are similar
    TEST_CHECK(nodes1 == nodes2);

    // Retrieve the two maps containing the neighbors
    unordered_map<int, unordered_set<int>> m1 = DG.get_Nout();
    unordered_map<int, unordered_set<int>> m2 = DG2.get_Nout();

    // Compare the two maps if they are equal
    TEST_CHECK(m1 == m2);

    clearFileContents(filename);

    // ------------------------------------------------------------------------------------------- Graph with only nodes check (0 edges)

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
    int n1 = DG.createNode(v1);
    int n2 = DG.createNode(v2);
    int n3 = DG.createNode(v3);
    int n4 = DG.createNode(v4);
    int n5 = DG.createNode(v5);
    int n6 = DG.createNode(v6);
    int n7 = DG.createNode(v7);
    int n8 = DG.createNode(v8);
    int n9 = DG.createNode(v9);
    int n10 = DG.createNode(v10);

    // Store the graph in a textfile
    DG.store(filename);

    // Load the previously stored data of the first graph into the second
    DG2.load(filename);

    // Check if the number of the edges are equal
    TEST_CHECK(DG.get_n_edges() == DG2.get_n_edges());

    // Check if the number of the nodes are equal
    TEST_CHECK(DG.get_n_nodes() == DG2.get_n_nodes());

    // Retrieve the nodes of the two DGs
    nodes1 = DG.getNodes();
    nodes2 = DG2.getNodes();

    // Compare the two nodes if they are similar
    TEST_CHECK(nodes1 == nodes2);

    // Retrieve the two maps containing the neighbors
    m1 = DG.get_Nout();
    m2 = DG2.get_Nout();

    // Compare the two maps if they are equal
    TEST_CHECK(m1 == m2);

    clearFileContents(filename);

    // ------------------------------------------------------------------------------------------- Graphs with edges

    // Create a random graph with R = 5
    DG.Rgraph(5);

    // Store the graph in a textfile
    DG.store(filename);

    // Load the previously stored data of the first graph into the second
    DG2.load(filename);

    // Check if the number of the edges are equal
    TEST_CHECK(DG.get_n_edges() == DG2.get_n_edges());

    // Check if the number of the nodes are equal
    TEST_CHECK(DG.get_n_nodes() == DG2.get_n_nodes());

    // Retrieve the nodes of the two DGs
    nodes1 = DG.getNodes();
    nodes2 = DG2.getNodes();

    // Compare the two nodes if they are similar
    TEST_CHECK(nodes1 == nodes2);

    // Retrieve the two maps containing the neighbors
    m1 = DG.get_Nout();
    m2 = DG2.get_Nout();

    // Compare the two maps if they are equal
    TEST_CHECK(m1 == m2);

    // Delete the temporary file for storing and loading the contents of a graph
    remove(filename.c_str());
}

TEST_LIST = {
    { "test_Store_and_Load", test_Store_and_Load},
    { NULL, NULL }     // zeroed record marking the end of the list
};