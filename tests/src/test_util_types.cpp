#include <iostream>
#include "acutest.h"
#include "util_types.hpp"
#include <set>
#include <vector>

using namespace std;

void test_eudcileanDistance(){
    float tol = 0.001f; // For result comparison (precision issues)

    // ------------------------------------------------------------------------------------------- Empty values check
    Node n1 = new vector<float>;
    Node n2 = new vector<float>;
    TEST_CHECK(euclideanDistance(n1,n2) == -1);
    TEST_MSG("Empty values check (empty,empty)");

    delete n1;
    delete n2;

    // ------------------------------------------------------------------------------------------- Different dimensions check
    n1 = new vector<float>{0.23f, 1.01f, 33.0f};
    n2 = new vector<float>{0.232f, 1.02f};
    TEST_CHECK(euclideanDistance(n1,n2) == -1);
    TEST_MSG("Different dimensions check, (3,2)");

    TEST_CHECK(euclideanDistance(n2,n1) == -1);
    TEST_MSG("Different dimensions check, (2,3)");

    delete n1;
    delete n2;

    // ------------------------------------------------------------------------------------------- Normal values testing
    n1 = new vector<float>{1.5f, 2.5f, 3.5f};
    n2 = new vector<float>{3.5f, 4.5f, 6.5f};
    float expected = 5.196f;
    float result = euclideanDistance(n1,n2);
    TEST_CHECK(fabs(result - expected) <= tol);
    TEST_MSG("Normal values, ({1.5f, 2.5f, 3.5f}, {4.5f, 5.5f, 6.5f}) between [5.196-tol, 5.196+tol]");

    // ------------------------------------------------------------------------------------------- Symmetry check
    result = euclideanDistance(n2,n1);
    TEST_CHECK(fabs(result - expected) <= tol);
    TEST_MSG("Symmetry check");

    delete n1;
    delete n2;
}

void reset_myArgMin(vector<float>* q, set<Node>* nodeSet, Node* result, Node* expected){
    for (auto node : *nodeSet){
        delete node;
    }
    nodeSet->clear();
    q->clear();
    if (*result != nullptr){
        delete *result;
        *result = nullptr;
    }
    if (*expected != nullptr){
        delete *expected;
        *expected = nullptr;
    }
}

// Given that eudclidean distance function works as intended
void test_myArgMin(){
    vector<float> q;
    set<Node> nodeSet;
    Node result,expected;

    // ------------------------------------------------------------------------------------------- Both empty check
    expected = NULL;
    result = myArgMin(nodeSet, q, euclideanDistance);

    TEST_CHECK(result == expected);
    TEST_MSG("Empty Set and Empty q");
    reset_myArgMin(&q, &nodeSet, &result, &expected);

    // ------------------------------------------------------------------------------------------- Empty set check

    // Create q vector
    for (int i=0; i<10; i++)
        q.push_back((1.24f * i) /(0.32 + (i* 0.5)));

    expected = NULL;
    result = myArgMin(nodeSet, q, euclideanDistance);

    TEST_CHECK(result == expected);
    TEST_MSG("Empty Set");
    reset_myArgMin(&q, &nodeSet, &result, &expected);

    // ------------------------------------------------------------------------------------------- Empty q(vector) check
    
    // Create nodes to add in nodeSet
    for (int i=0; i<10; i++){
        Node n = new vector<float>{2.5f * i, 2.4f * i, 2.3f * i, 2.2f * i, 2.1f * i};
        nodeSet.insert(n);
    }

    expected = NULL;
    result = myArgMin(nodeSet, q, euclideanDistance);

    TEST_CHECK(result == expected);
    TEST_MSG("Empty q");
    reset_myArgMin(&q, &nodeSet, &result, &expected);


    // ------------------------------------------------------------------------------------------- Different dimensions in nodeSet

    // Create nodes, with different dimensions each, to add in nodeSet
    for (int i=0; i<10; i++){
        Node n = new vector<float>;
        for (int j=0; j<i; j++){
            n->push_back(3.2f * i);
        }
        nodeSet.insert(n);
    }

    expected = NULL;
    result = myArgMin(nodeSet, q, euclideanDistance);

    TEST_CHECK(result == expected);
    TEST_MSG("Different dimensions in nodeSet");
    reset_myArgMin(&q, &nodeSet, &result, &expected);

    // ------------------------------------------------------------------------------------------- Different dimensions in q <-> nodeSet check

    // Create q vector (10dim)
    for (int i=0; i<10; i++)
        q.push_back((1.24f * i) /(0.32 + (i* 0.5)));

    // Create nodes to add in nodeSet (5dim)
    for (int i=0; i<10; i++){
        Node n = new vector<float>{2.5f * i, 2.4f * i, 2.3f * i, 2.2f * i, 2.1f * i};
        nodeSet.insert(n);
    }

    expected = NULL;
    result = myArgMin(nodeSet, q, euclideanDistance);

    TEST_CHECK(result == expected);
    TEST_MSG("Different dimensions between q and nodeSet");
    reset_myArgMin(&q, &nodeSet, &result, &expected);

    // ------------------------------------------------------------------------------------------- Normal values testing

    // Create nodes to add in nodeSet
    Node n1 = new vector<float> {2.5f, 3.5f, 4.5f};
    Node n2 = new vector<float> {3.2f, 1.5f, 0.5f};
    Node n3 = new vector<float> {34.5f, 20.5f, 10.5f};
    Node n4 = new vector<float> {0.0f, 1.0f, 1000.0f};

    // Insert nodes in nodeSet
    nodeSet.insert(n1);
    nodeSet.insert(n2);
    nodeSet.insert(n3);
    nodeSet.insert(n4);

    // Create q vector
    q.assign({30.0f, 20.0f, 10.0f});

    expected = n3;
    result = myArgMin(nodeSet, q, euclideanDistance);

    TEST_CHECK(result == expected);
    TEST_MSG("Normal values");
    reset_myArgMin(&q, &nodeSet, &result, &expected);
}

void test_closestN(){

}

TEST_LIST = {
    { "test_eudcileanDistance", test_eudcileanDistance },
    { "test_myArgMin", test_myArgMin },
    { "test_closestN", test_closestN },
    { NULL, NULL }     // zeroed record marking the end of the list
};