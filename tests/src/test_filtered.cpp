#include "acutest.h"
#include "interface.hpp"

void test_filteredGreedySearch(){
    // Argument checks:

    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);


    unordered_set<Id> invalidSet, validSet{1,2,3};
    unordered_set<Id> singleSet{1};
    Id invalidId, validId = 1;
    vector<float> validVector{1,2,3};
    Query<vector<float>> validQuery(validId,1,true,validVector,vectorEmpty<float>);
    Query<vector<float>> invalidQuery;

    // Uninitialized set
    try {
        pair<unordered_set<Id>, unordered_set<Id>> ret = DG.filteredGreedySearch(invalidSet, validQuery, 4, 5);
    }catch(invalid_argument& ia) { TEST_CHECK(string(ia.what()) == "No start node was provided.\n"); }

    // if k <= 0
    try{
        pair<unordered_set<Id>, unordered_set<Id>> ret = DG.filteredGreedySearch(validSet, validQuery, -1, 5);
        TEST_CHECK(false);  // Control should not reach here 
    }catch(invalid_argument& ia){ TEST_CHECK(string(ia.what()) == "K must be greater than or equal to 0.\n"); }

    // if L < k
    try{
        pair<unordered_set<Id>, unordered_set<Id>> ret = DG.filteredGreedySearch(validSet, validQuery, 2, 1);
        TEST_CHECK(false);  // Control should not reach here 
    }catch(invalid_argument& ia){ TEST_CHECK(string(ia.what()) == "L must be greater or equal to K.\n"); }

}

void test_filteredRobustPrune(){
    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);
    unordered_set<Id> nullset;
    Id s;
    try{
        DG.filteredRobustPrune(s, nullset, 1, 5); 
        TEST_CHECK(false);  // Control should not reach here 
    }catch(invalid_argument& ia){ TEST_CHECK(string(ia.what()) == "Invalid Index was provided.\n"); }

    // a < 1
    try{
        DG.filteredRobustPrune(43, nullset, 0, 5);
        TEST_CHECK(false);  // Control should not reach here 
    }catch(invalid_argument& ia){ TEST_CHECK(string(ia.what()) == "Parameter a must be >= 1.\n"); }

    // R <= 0
    try{
        // test goes here
        DG.filteredRobustPrune(43, nullset, 1, 0);
        TEST_CHECK(false);  // Control should not reach here 
    }catch(invalid_argument& ia){ TEST_CHECK(string(ia.what()) == "Parameter R must be > 0.\n"); }

}

void test_filteredVamanaAlgorithm(){
    
    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);
    
    // R <= 0
    try{
        TEST_CHECK(DG.filteredVamanaAlgorithm(1,0,1,0.5));
        TEST_CHECK(false);  // Control should not reach here
    }catch(invalid_argument& ia){ TEST_CHECK(string(ia.what()) == "R must be a positive, non-zero integer.\n"); }

    // L <= 0
    try{
        TEST_CHECK(DG.filteredVamanaAlgorithm(0,1,1,0.5));
        TEST_CHECK(false);  // Control should not reach here
    }catch(invalid_argument& ia){ TEST_CHECK(string(ia.what()) == "Parameter L must be >= 1.\n"); }

    // a < 1
    try{
        TEST_CHECK(DG.filteredVamanaAlgorithm(1,1,0,0.5));
        TEST_CHECK(false);  // Control should not reach here
    }catch(invalid_argument& ia){ TEST_CHECK(string(ia.what()) == "Parameter a must be >= 1.\n"); }

    // t not in (0,1])
    try{
        TEST_CHECK(DG.filteredVamanaAlgorithm(1,1,1,0));
        TEST_CHECK(false);  // Control should not reach here
    }catch(invalid_argument& ia){ TEST_CHECK(string(ia.what()) == "Parameter t must be between (0,1]"); }

}

void test_stitchedVamanaAlgorithm(){
    
    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);
    
    // Lstitched < 1
    try{
        TEST_CHECK(DG.stitchedVamanaAlgorithm(0,1,1,1,1));
        TEST_CHECK(false);  // Control should not reach here
    }catch(invalid_argument& ia){ TEST_CHECK(string(ia.what()) == "Parameter L must be >= 1.\n"); }

    // Rstitched <= 0
    try{
        TEST_CHECK(DG.stitchedVamanaAlgorithm(1,0,1,1,1));
        TEST_CHECK(false);  // Control should not reach here
    }catch(invalid_argument& ia){ TEST_CHECK(string(ia.what()) == "Rstitched must be a positive, non-zero integer.\n"); }

    // Lsmall < 1
    try{
        TEST_CHECK(DG.stitchedVamanaAlgorithm(1,1,0,1,1));
        TEST_CHECK(false);  // Control should not reach here
    }catch(invalid_argument& ia){ TEST_CHECK(string(ia.what()) == "Parameter Lsmall must be >= 1.\n"); }

    // Rsmall <= 0
    try{
        TEST_CHECK(DG.stitchedVamanaAlgorithm(1,1,1,0,1));
        TEST_CHECK(false);  // Control should not reach here
    }catch(invalid_argument& ia){ TEST_CHECK(string(ia.what()) == "Rsmall must be a positive, non-zero integer.\n"); }

    // a < 1
    try{
        TEST_CHECK(DG.stitchedVamanaAlgorithm(1,1,1,1,0));
        TEST_CHECK(false);  // Control should not reach here
    }catch(invalid_argument& ia){ TEST_CHECK(string(ia.what()) == "Parameter a must be >= 1.\n"); }
}

void test_filterSet(){
    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>, vectorEmpty<float>);
    
    vector<float> v1{1,2,3,4,5};
    vector<float> v2{2,2,3,4,5};
    vector<float> v3{3,2,3,4,5};
    vector<float> v4{4,2,3,4,5};
    vector<float> v5{5,2,3,4,5};

    Id id_1 = DG.createNode(v1, 1);
    Id id_2 = DG.createNode(v2, 1);
    Id id_3 = DG.createNode(v3, 2);
    Id id_4 = DG.createNode(v4, -1);
    Id id_5 = DG.createNode(v5, 5);

    unordered_set<Id> initial_set{id_1, id_2, id_3, id_4, id_5};
    unordered_set<Id> expected_set{id_1, id_2};
    unordered_set<Id> empty_set;

    // Normal case
    TEST_CHECK(DG.filterSet(initial_set, 1) == expected_set);

    // Empty set
    expected_set.clear();
    TEST_CHECK(empty_set == expected_set);

    // Unfiltered
    TEST_CHECK(DG.filterSet(initial_set, -1) == initial_set);
}

TEST_LIST = {
    { "test_filteredGreedySearch", test_filteredGreedySearch},
    { "test_filteredRobustPrune", test_filteredRobustPrune},
    { "test_filteredVamanaAlgorithm", test_filteredVamanaAlgorithm},
    { "test_stitchedVamanaAlgorithm", test_stitchedVamanaAlgorithm},
    { "test_filterSet", test_filterSet},
    { NULL, NULL }     // zeroed record marking the end of the list
};