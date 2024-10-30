#include <iostream>
#include "util.hpp"
#include "acutest.h"
#include <vector>
#include <string.h>
#include "types.hpp"

using namespace std;

void test_euclideanDistance(void){
    OMIT_OUTPUT;

    float tol = 0.001f; // For result comparison (precision issues)

    // ------------------------------------------------------------------------------------------- Empty values check
    vector<float> v1;
    vector<float> v2;
    try{
        float res = euclideanDistance(v1,v2);
        TEST_CHECK(false); // control should not reach here
    }catch(const invalid_argument& ia) { TEST_CHECK(string(ia.what()) == "Argument Containers are empty"); };

    // ------------------------------------------------------------------------------------------- Different dimensions check
    v1 = {0.23f, 1.01f, 33.0f};
    v2 = {0.232f, 1.02f};
    
    TEST_MSG("Different dimensions check, (3,2)");
    try{
        float res = euclideanDistance(v1,v2);
        TEST_CHECK(false); // control should not reach here
    }catch(const invalid_argument& ia) { TEST_CHECK(string(ia.what()) == "Dimension Mismatch between Arguments"); };

    // symmetry check
    try{
        float res = euclideanDistance(v2,v1);
        TEST_CHECK(false); // control should not reach here
    }catch(const invalid_argument& ia) { TEST_CHECK(string(ia.what()) == "Dimension Mismatch between Arguments"); };

    v1.clear();
    v2.clear();
    // ------------------------------------------------------------------------------------------- Normal values testing
    v1 = {1.5f, 2.5f, 3.5f};
    v2 = {4.5f, 5.5f, 6.5f};
    float expected = 27.0f; // Squared distance expected value
    tol = 0.001f;     // Define tolerance level

    float result = euclideanDistance(v1, v2);

    TEST_CHECK(fabs(result - expected) <= tol);
    TEST_MSG("Squared distance values, ({1.5f, 2.5f, 3.5f}, {4.5f, 5.5f, 6.5f}) between [%.3f, %.3f], but got %.3f", 
             expected - tol, expected + tol, result);


    // ------------------------------------------------------------------------------------------- Symmetry check
    result = euclideanDistance(v2,v1);
    TEST_CHECK(fabs(result - expected) <= tol);
    TEST_MSG("Symmetry check");
}

void test_setIn(void){
    OMIT_OUTPUT;

    set<int> s;
    s.insert((int)1);

    TEST_CHECK(setIn((int)1, s) == true);
    TEST_CHECK(setIn((int)2, s) == false);
}

void test_mapKeyExists(void){
    OMIT_OUTPUT;

    map<int,int> map;
    map[1] = 1;

    TEST_CHECK(mapKeyExists((int)1, map) == true);
    TEST_CHECK(mapKeyExists((int)2, map) == false);
}

void test_setSubtraction(void){
    OMIT_OUTPUT;
    
    set<int> A,B,C,T,TA,TB,N;

    // default case 1: when A ∩ B = ∅
    A = {1,2};
    B = {3,4};
    TA = A;
    TB = B;

    TEST_CHECK(setSubtraction(A,B) == TA);
    TEST_CHECK(setSubtraction(B,A) == TB);

    // default case 2: A ∩ B ≠ ∅
    A = {1,2};
    B = {2,3};
    TA = {1};
    TB = {3};

    TEST_CHECK(setSubtraction(A,B) == TA);
    TEST_CHECK(setSubtraction(B,A) == TB);

    // default case 3: A ⊆ B
    A = {1,2};
    B = {1,2,3};
    TA = N;
    TB = {3};

    TEST_CHECK(setSubtraction(A,B) == TA);
    TEST_CHECK(setSubtraction(B,A) == TB);

    // subtraction with ∅
    TEST_CHECK(setSubtraction(A,N) == A);
    TEST_CHECK(setSubtraction(N,A) == N);

    // subtraction with self A \ A = ∅
    TEST_CHECK(setSubtraction(A,A) == N);
}

void test_setUnion(void){
    OMIT_OUTPUT;

    set<int> A,B,C,T,N;

    // default case 1: when A ∩ B = ∅
    A = {1,2};
    B = {3,4};
    T = {1,2,3,4};

    TEST_CHECK(setUnion(A,B) == T);

    // default case 2: A ∩ B ≠ ∅
    A = {1,2};
    B = {2,3};
    T = {1,2,3};

    TEST_CHECK(setUnion(A,B) == T);

    // default case 3: A ⊆ B
    A = {1,2};
    B = {1,2,3};
    T = B;

    TEST_CHECK(setUnion(A,B) == T);

    // union with ∅
    TEST_CHECK(setUnion(A,N) == A);
    TEST_CHECK(setUnion(N,A) == A);
    TEST_CHECK(setUnion(N,N) == N);

    // union with self
    TEST_CHECK(setUnion(A,A) == A);

    // symmetry check
    TEST_CHECK(setUnion(A,B) == T);
    TEST_CHECK(setUnion(B,A) == T);

    // associative property check (A ∪ B) ∪ C = A ∪ (B ∪ C)
    C = {3,4};
    TEST_CHECK(setUnion(setUnion(A,B),C) == setUnion(A, setUnion(B,C)));
}

void test_sampleFromSet(void){
    OMIT_OUTPUT;
    
    set<int> s;

    // default case - check that sampled element exists in unordered_set
    for (int i = 0; i < 10000; i++)
        s.insert(i);
    
    int r = sampleFromSet(s);
    TEST_CHECK(setIn(r, s));

    s.clear();
    
    // empty unordered_set check
    try {
        r = sampleFromSet(s);
        TEST_CHECK(false); // control should not reach here
    }catch(const invalid_argument& ia){ TEST_CHECK(string(ia.what()) == "Set is empty.\n"); }

    // |s| is 1 always brings the same element
    s = {1};
    for (int i = 0; i < 10; i++){
        r = sampleFromSet(s);
        TEST_CHECK(r == 1);
    }

    // |s| > 1 might bring the same or a different element
    s = {1,2};
    bool changed = false, same = false;
    r = sampleFromSet(s);
    for (int i = 0; i < 1000000; i++){  // high number of iterations to make it almost certain that from a unordered_set of |s| = 2, the same element will be resampled.
        int ri = sampleFromSet(s);
        if (ri == r)
            same = true;
        else if (ri != r)
            changed = true;

        if (same && changed)    // check that re-sample can bring the same element more than once
            break;
    }
    TEST_CHECK(same && changed);
}

void test_myArgMin(void){
    OMIT_OUTPUT;

    set<vector<float>> s;

    // default case
    s = {
        (vector<float>) {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
        (vector<float>) {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
        (vector<float>) {2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f},
        (vector<float>) {3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f},
        (vector<float>) {4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f},
        (vector<float>) {5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f},
        (vector<float>) {6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f, 6.0f},
    };

    vector<float> xq = {2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f};

    vector<float> xmin = myArgMin<vector<float>>(s, xq, euclideanDistance<vector<float>>, vectorEmpty<float>);
    vector<float> ymin = {2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f};
    TEST_CHECK(xmin == ymin);


    // empty arguments check:

    // empty query, non empty unordered_set
    xq.clear();
    try{
        xmin = myArgMin<vector<float>>(s, xq, euclideanDistance<vector<float>>, vectorEmpty<float>);
        TEST_CHECK(false);  // control should not reach here
    }catch(invalid_argument &ia){ TEST_CHECK(string(ia.what()) == "Query container is empty.\n"); }

    // empty unordered_set, non empty query
    xq = {2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f, 2.3f};
    s.clear();
    try{
        xmin = myArgMin<vector<float>>(s, xq, euclideanDistance<vector<float>>, vectorEmpty<float>);
        TEST_CHECK(false);  // control should not reach here
    }catch(invalid_argument &ia){ TEST_CHECK(string(ia.what()) == "Set is Empty.\n"); }

    // both empty
    xq.clear();
    try{
        xmin = myArgMin<vector<float>>(s, xq, euclideanDistance<vector<float>>, vectorEmpty<float>);
        TEST_CHECK(false);  // control should not reach here
    }catch(invalid_argument &ia){ TEST_CHECK(string(ia.what()) == "Set is Empty.\n"); }

}

void test_closestN(void){
    OMIT_OUTPUT;

    int N = 5;
    set<vector<float>> s;

    // Create 1000 vectors of dimension 128 with values [i, i, ..., i]
    for (int i = 0; i < 1000; i++) {
        vector<float> v(128, static_cast<float>(i));
        s.insert(v);
    }

    // Query vector [327.3f, 327.3f, ..., 327.3f]
    vector<float> x(128, 327.3f);

    // Expected closest vectors: [325, 326, 327, 328, 329]
    set<vector<float>> yclosest;
    for (int i = 0; i < 5; i++) {
        vector<float> v(128, static_cast<float>(325 + i));
        yclosest.insert(v);
    }
    set<vector<float>> xclosest = closestN<vector<float>>(N, s, x, euclideanDistance<vector<float>>, vectorEmpty<float>);

    TEST_CHECK(xclosest == yclosest);

    // argument checks:
    
    // N > |s| returns the whole set
    N = 1001; // |s| = 1000
    TEST_CHECK(closestN<vector<float>>(N, s, x, euclideanDistance<vector<float>>, vectorEmpty<float>) == s);

    // N < 0
    try{
        xclosest = closestN<vector<float>>(-1, s, x, euclideanDistance<vector<float>>, vectorEmpty<float>);
        TEST_CHECK(false);  // Control should not reach here
    }catch(invalid_argument& ia){ TEST_CHECK(string(ia.what()) == "N must be greater than 0.\n"); }

    // N == 0
    N = 0;
    set<vector<float>> nullset;
    TEST_CHECK(closestN<vector<float>>(N, s, x, euclideanDistance<vector<float>>, vectorEmpty<float>) == nullset);

    // set is empty
    TEST_CHECK(closestN<vector<float>>(N, nullset, x, euclideanDistance<vector<float>>, vectorEmpty<float>) == nullset);

    // Empty query
    x.clear();  // Clear the query vector

    try{
        xclosest = closestN<vector<float>>(N, s, x, euclideanDistance<vector<float>>, vectorEmpty<float>);
        TEST_CHECK(false);  // Control should not reach here
    }catch(invalid_argument& ia){ 
        // Check if the caught exception message matches the expected message
        TEST_CHECK(string(ia.what()) == "Query X is empty.\n");
    }



    // both set and query empty
    TEST_CHECK(closestN<vector<float>>(N, nullset, x, euclideanDistance<vector<float>>, vectorEmpty<float>) == nullset);

    // case with ties
    s = {
        (vector<float>) {1,1,1,1,1},
        (vector<float>) {2,2,2,2,2},
        (vector<float>) {3,3,3,3,3},
        (vector<float>) {4,4,4,4,4},
        (vector<float>) {5,5,5,5,5}
    };
    x = {2.5f, 2.5f, 2.5f, 2.5f, 2.5f};
    N = 3;
    
    yclosest = {                        // because of ascending sort, takes the smallest value that is closest to the reference point x first.
        (vector<float>) {1,1,1,1,1},
        (vector<float>) {2,2,2,2,2},
        (vector<float>) {3,3,3,3,3},
    };
    xclosest = closestN<vector<float>>(N, s, x, euclideanDistance<vector<float>>, vectorEmpty<float>);
    TEST_CHECK(xclosest == yclosest);
}

void test_permutation(void){
    OMIT_OUTPUT;

    set<int> numbers;

    // default case
    for (int i = 0; i < 1000000; i++)
        numbers.insert(i);

    vector<int> before(numbers.begin(), numbers.end()); // transforming unordered_set into vector
    vector<int> after = permutation(numbers);           // a same permutation for a unordered_set of 1000000 elements is very highly unlikely
    TEST_CHECK((before == after) == false);

    numbers.clear();before.clear();after.clear();

    // empty unordered_set check
    after = permutation(numbers);
    TEST_CHECK(before == after);

    // unordered_set with exactly one element
    numbers.insert(1);
    before = vector<int>(numbers.begin(), numbers.end());  // transforming unordered_set into vector
    after = permutation(numbers);
    TEST_CHECK(before == after);

    // ensure that it is possible to achieve a permutation identical to the original, without requiring the unordered_set to remain unchanged.
    numbers.insert(2);
    before = vector<int>(numbers.begin(), numbers.end());  // transforming unordered_set into vector
    bool same = false;
    bool changed = false;
    for (int i = 0; i < 1000000; i++){  // big number of iterations to make it "impossible" for edge cases to appear
        after = permutation(numbers);
        if (!same && before == after)
            same = true;
        if (!changed && before != after)
            changed = true;
        if (same && changed)
            break;
    }
    TEST_CHECK(changed && same);
}

TEST_LIST = {
    { "test_euclideanDistance", test_euclideanDistance },
    { "test_setIn", test_setIn },
    { "test_mapKeyExists", test_mapKeyExists},
    { "test_setSubtraction", test_setSubtraction },
    { "test_setUnion", test_setUnion },
    { "test_sampleFromSet", test_sampleFromSet },
    { "test_myArgMin", test_myArgMin },
    { "test_closestN", test_closestN },
    { "test_permutation", test_permutation },
    { NULL, NULL }     // zeroed record marking the end of the list
};