#include <iostream>
#include "util.hpp"
#include "acutest.h"
#include <vector>

using namespace std;

// This function tests the eudclideanDistance function from util.hpp
void test_euclideanDistance(){
    vector<float> v1,v2;

// Different dimensions check
    v1.push_back(0.5f);
    TEST_CHECK(euclideanDistance(v1,v2) == -1);

// Another different dimensions check
    v2.push_back(0.4f);
    v2.push_back(0.6f);
    TEST_CHECK(euclideanDistance(v1,v2) == -1);

// Zero dimensions check (empty vectors)
    v1.clear();
    v2.clear();

    TEST_CHECK(euclideanDistance(v1,v2) == -1);

// Same vector check
    v1.push_back(0.348f);
    v1.push_back(0.9348f);
    v1.push_back(0.666f);

    v2.push_back(0.348f);
    v2.push_back(0.9348f);
    v2.push_back(0.666f);

    TEST_CHECK(euclideanDistance(v1,v2) == 0);

// Some normal values testing

}

TEST_LIST = {
    { "test_euclideanDistance", test_euclideanDistance },
    { NULL, NULL }     // zeroed record marking the end of the list
};