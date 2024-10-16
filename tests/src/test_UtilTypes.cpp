#include <iostream>
#include "acutest.h"
#include "util_types.hpp"
#include <set>
#include <vector>

using namespace std;

void test_setSubtraction(){

    // Create nodes to insert to sets
    Node n1 = Node(), n2 = Node(), n3 = Node(), n4 = Node(), n5 = Node(), n6 = Node();

    // Assign ids to nodes
    n1.setId(1); n2.setId(2); n3.setId(3);
    n4.setId(4); n5.setId(5); n6.setId(6);

    // Create values to assign to nodes
    vector<float> v1,v2,v3,v4,v5,v6;
    v1.insert(v1.end(), { 0.432f, 0.234f, 21.35f, 43.0f });
    v2.insert(v2.end(), { 0.33f, 74.01f, 2.54f, 3.69f });
    v3.insert(v3.end(), { 6.04f, 6.29f, 66.21f, 66.9f });
    v4.insert(v4.end(), { 8.31f, 2.24f, 24.023f, 5.613f });
    v5.insert(v5.end(), { 3.215f, 6.234f, 27.44f, 3.14f });
    v6.insert(v6.end(), { 98.36f, 0.285f, 25.6315f, 12.4f });

    // Assign values to nodes
    n1.setValue(v1);
    n2.setValue(v2);
    n3.setValue(v3);
    n4.setValue(v4);
    n5.setValue(v5);
    n6.setValue(v6);

    // Create sets
    set<Node> s1,s2,resultSet,emptySet;

    // Insert nodes to sets
    s1.insert(n1);
    s1.insert(n3);
    s1.insert(n4);
    s1.insert(n5);

    s2.insert(n4);
    s2.insert(n5);
    s2.insert(n6);

    resultSet.insert(n1);
    resultSet.insert(n3);

    TEST_CHECK(setSubtraction(s1,s2) == resultSet);
    TEST_MSG("Normal values");

    resultSet.clear();
    resultSet.insert(n6);

    TEST_CHECK(setSubtraction(s2,s1) == resultSet);
    TEST_MSG("Normal values");

    // EMPTY - EMPTY = EMPTY
    s1.clear();
    s2.clear();
    resultSet.clear();
    TEST_CHECK(setSubtraction(emptySet,emptySet) == emptySet);
    TEST_MSG("EMPTY - EMPTY = EMPTY");

    // S1 - EMPTY = S1
    s1.insert(n1);
    s1.insert(n2);
    TEST_CHECK(setSubtraction(s1,emptySet) == s1);
    TEST_MSG("S1 - EMPTY = S1");

    // EMPTY - S1 = EMPTY
    TEST_CHECK(setSubtraction(emptySet,s1) == emptySet);
    TEST_MSG("EMPTY - S1 = EMPTY");

    // S1 - S1 = EMPTY
    TEST_CHECK(setSubtraction(s1,s1) == emptySet);
    TEST_MSG("S1 - S1 = EMPTY");

    // S1 - S2 (no common elements)
    // s1 -> 1,2,3
    // s2 -> 3,4,5
    s1.clear();
    s1.insert(n1);
    s1.insert(n2);
    s1.insert(n3);

    s2.clear();
    s2.insert(n4);
    s2.insert(n5);
    s2.insert(n6);

    resultSet.clear();
    resultSet.insert(n1);
    resultSet.insert(n2);
    resultSet.insert(n3);

    // {1,2,3} - {4,5,6} = {1,2,3}
    TEST_CHECK(setSubtraction(s1,s2) == resultSet);
    TEST_MSG("{1,2,3} - {4,5,6} = {1,2,3}");

    resultSet.clear();
    resultSet.insert(n4);
    resultSet.insert(n5);
    resultSet.insert(n6);

    // {4,5,6} - {1,2,3} = {4,5,6}
    TEST_CHECK(setSubtraction(s2,s1) == resultSet);
    TEST_MSG("{4,5,6} - {1,2,3} = {4,5,6}");

}

void test_setUnion(){
    // Create nodes to insert to sets
    Node n1 = Node(), n2 = Node(), n3 = Node(), n4 = Node(), n5 = Node(), n6 = Node();

    // Assign ids to nodes
    n1.setId(1); n2.setId(2); n3.setId(3);
    n4.setId(4); n5.setId(5); n6.setId(6);

    // Create values to assign to nodes
    vector<float> v1,v2,v3,v4,v5,v6;
    v1.insert(v1.end(), { 0.432f, 0.234f, 21.35f, 43.0f });
    v2.insert(v2.end(), { 0.33f, 74.01f, 2.54f, 3.69f });
    v3.insert(v3.end(), { 6.04f, 6.29f, 66.21f, 66.9f });
    v4.insert(v4.end(), { 8.31f, 2.24f, 24.023f, 5.613f });
    v5.insert(v5.end(), { 3.215f, 6.234f, 27.44f, 3.14f });
    v6.insert(v6.end(), { 98.36f, 0.285f, 25.6315f, 12.4f });

    // Assign values to nodes
    n1.setValue(v1);
    n2.setValue(v2);
    n3.setValue(v3);
    n4.setValue(v4);
    n5.setValue(v5);
    n6.setValue(v6);

    // Create sets
    set<Node> s1,s2,resultSet,emptySet;

    // EMPTY U EMPTY = EMPTY
    TEST_CHECK(setUnion(emptySet,emptySet) == emptySet);
    TEST_MSG("EMPTY U EMPTY = EMPTY");

    // EMPTY U S1 = S1
    s1.insert(n1);
    s1.insert(n2);
    s1.insert(n5);

    TEST_CHECK(setUnion(emptySet,s1) == s1);
    TEST_MSG("EMPTY U S1 = S1");

    // S1 U EMPTY = S1
    TEST_CHECK(setUnion(s1,emptySet) == s1);
    TEST_MSG("S1 U EMPTY");

    // S1 U S1 = S1
    TEST_CHECK(setUnion(s1,s1) == s1);
    TEST_MSG("S1 U S1");

    // S1 U S2 with common elements
    s2.insert(n1);
    s2.insert(n4);

    resultSet.insert(n1);
    resultSet.insert(n2);
    resultSet.insert(n4);
    resultSet.insert(n5);

    TEST_CHECK(setUnion(s1,s2) == resultSet);
    TEST_MSG("S1 U S2 with common elements");

    // S1 U S2 with no common elements
    s1.clear();
    s1.insert(n1);
    s1.insert(n4);

    s2.clear();
    s2.insert(n2);
    s2.insert(n3);

    resultSet.clear();

    resultSet.insert(n1);
    resultSet.insert(n2);
    resultSet.insert(n3);
    resultSet.insert(n4);

    TEST_CHECK(setUnion(s1,s2) == resultSet);
    TEST_MSG("S1 U S2 with no common elements");

    TEST_CHECK(setUnion(s2,s1) == resultSet);
    TEST_MSG("S2 U S1 with no common elements");

}

void test_myArgMin(){

}

void test_closestN(){

}

TEST_LIST = {
    { "test_setSubtraction", test_setSubtraction },
    { "test_setUnion", test_setUnion },
    { "test_myArgMin", test_myArgMin },
    { "test_closestN", test_closestN },
    { NULL, NULL }     // zeroed record marking the end of the list
};