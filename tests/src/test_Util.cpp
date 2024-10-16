#include <iostream>
#include "util.hpp"
#include "acutest.h"
#include <vector>

using namespace std;

// This function tests the eudclideanDistance function from util.hpp
void test_euclideanDistance(){
    vector<float> v1,v2,v3;

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

// Expected result check

    v1.clear();
    v2.clear();

    v1.push_back(2);
    v1.push_back(4);
    v1.push_back(6);

    v2.push_back(4);
    v2.push_back(8);
    v2.push_back(2);

    TEST_CHECK(euclideanDistance(v1,v2) == 6);

// Symmetric value check

    for (int i = 0; i < 5; i++){
        v1.clear();
        v2.clear();

        for (int j = 0; j < 10; j++){
            v1.push_back( (i+j) / (i*j + 1) );
            v2.push_back( (i-j) / (i*j + 1) );
        }
        TEST_CHECK(euclideanDistance(v1,v2) == euclideanDistance(v2,v1));
    }

// Triangular Inequality check
    v1.clear();
    v2.clear();

    for (int i = 0; i < 5; i++){
        v1.clear();
        v2.clear();
        v3.clear();

        for (int j = 0; j < 10; j++){
            v1.push_back( (i+j) / (i*j + 1) );
            v2.push_back( (i-j) / (i*j + 1) );
            v3.push_back( (i + i/(j+1)) * (i*j + 1) );
        }
        TEST_CHECK(euclideanDistance(v1,v3) <= (euclideanDistance(v1,v2) + euclideanDistance(v2,v3)));
    }
}

// test template functions

void test_setIn(){

}



TEST_LIST = {
    { "test_euclideanDistance", test_euclideanDistance },
    { "test_setIn", test_setIn },
    // 
    { NULL, NULL }     // zeroed record marking the end of the list
};