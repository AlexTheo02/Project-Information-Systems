#include "acutest.h"
#include "interface.hpp"

using namespace std;

void test_euclideanDistance(void){

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

void test_simd_euclideanDistance(void){

    float tol = 0.001f; // For result comparison (precision issues)

    // ------------------------------------------------------------------------------------------- Empty values check
    vector<float> v1;
    vector<float> v2;
    try{
        float res = simd_euclideanDistance(v1,v2);
        TEST_CHECK(false); // control should not reach here
    }catch(const invalid_argument& ia) { TEST_CHECK(string(ia.what()) == "Argument Containers are not of 100 or 128 dimensions"); };

    // ------------------------------------------------------------------------------------------- Different dimensions check
    v1 = {0.23f, 1.01f, 33.0f};
    v2 = {0.232f, 1.02f};
    
    TEST_MSG("Different dimensions check, (3,2)");
    try{
        float res = simd_euclideanDistance(v1,v2);
        TEST_CHECK(false); // control should not reach here
    }catch(const invalid_argument& ia) { TEST_CHECK(string(ia.what()) == "Dimension Mismatch between Arguments"); };

    // symmetry check
    try{
        float res = simd_euclideanDistance(v2,v1);
        TEST_CHECK(false); // control should not reach here
    }catch(const invalid_argument& ia) { TEST_CHECK(string(ia.what()) == "Dimension Mismatch between Arguments"); };

    v1.clear();
    v2.clear();
    // ------------------------------------------------------------------------------------------- Normal values testing

    vector<float> v128_1(128, 1.0f);
    vector<float> v128_2(128, 0.0f);

    float expected = 128.0f; // Squared distance expected value
    tol = 0.001f;     // Define tolerance level

    float result = simd_euclideanDistance(v128_1, v128_2);

    TEST_CHECK(fabs(result - expected) <= tol);

    // ------------------------------------------------------------------------------------------- Symmetry check
    result = simd_euclideanDistance(v128_2,v128_1);
    TEST_CHECK(fabs(result - expected) <= tol);
    TEST_MSG("Symmetry check");
}

void test_setIn(void){  

    unordered_set<int> s;
    s.insert((int)1);

    TEST_CHECK(setIn((int)1, s) == true);
    TEST_CHECK(setIn((int)2, s) == false);
}

void test_mapKeyExists(void){  

    unordered_map<int,int> map;
    map[1] = 1;

    TEST_CHECK(mapKeyExists((int)1, map) == true);
    TEST_CHECK(mapKeyExists((int)2, map) == false);
}

void test_setSubtraction(void){    
    
    unordered_set<int> A,B,C,T,TA,TB,N;

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

void test_PQSubtraction(void){    
    
    function<bool(int, int)> comparator = less<int>();
    priority_queue<int, std::vector<int>, function<bool(int, int)>> pq(comparator, {1, 2});

    pq.push(1);
    pq.push(2);

    unordered_set<int> A,B,C,T,TA,TB,N;
    A = {1, 2};

    // default case 1: when A ∩ B = ∅
    B = {3,4};

    // Test assertion (equivalent to TEST_CHECK)
    TEST_CHECK(PQSubtraction(pq, B) == A);

    // default case 2: A ∩ B ≠ ∅
    B = {2,3};
    TA = {1};
    TB = {3};

    TEST_CHECK(PQSubtraction(pq,B) == TA);

    // default case 3: A ⊆ B
    A = {1,2};
    B = {1,2,3};
    TB = {3};

    // Expecting empty set
    TEST_CHECK(PQSubtraction(pq,B).empty());

    // subtraction with ∅
    TEST_CHECK(PQSubtraction(pq,{}) == A);
    TEST_CHECK(PQSubtraction({},A) == N);

    // subtraction with self pq(A) \ A = ∅
    TEST_CHECK(PQSubtraction(pq,A) == N);
}

void test_setUnion(void){   

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
    
    unordered_set<int> s;

    // default case - check that sampled element exists in unordered_set
    for (int i = 0; i < 10000; i++)
        s.insert(i);
    
    int r = sampleFromContainer(s);
    TEST_CHECK(s.find(r) != s.end());

    s.clear();
    
    // empty unordered_set check
    try {
        r = sampleFromContainer(s);
        TEST_CHECK(false); // control should not reach here
    }catch(const invalid_argument& ia){ TEST_CHECK(string(ia.what()) == "Container is empty.\n"); }

    // |s| is 1 always brings the same element
    s = {1};
    for (int i = 0; i < 10; i++){
        r = sampleFromContainer(s);
        TEST_CHECK(r == 1);
    }

    // |s| > 1 might bring the same or a different element
    s = {1,2};
    bool changed = false, same = false;
    r = sampleFromContainer(s);
    for (int i = 0; i < 1000000; i++){  // high number of iterations to make it almost certain that from a unordered_set of |s| = 2, the same element will be resampled.
        int ri = sampleFromContainer(s);
        if (ri == r)
            same = true;
        else if (ri != r)
            changed = true;

        if (same && changed)    // check that re-sample can bring the same element more than once
            break;
    }
    TEST_CHECK(same && changed);
}

void test_permutation(void){    

    vector<int> numbers;

    // default case
    for (int i = 0; i < 1000000; i++)
        numbers.push_back(i);

    vector<int> before(numbers.begin(), numbers.end()); // transforming unordered_set into vector
    vector<int> after = permutation(numbers);           // a same permutation for a unordered_set of 1000000 elements is very highly unlikely
    TEST_CHECK((before == after) == false);

    numbers.clear();before.clear();after.clear();

    // empty unordered_set check
    after = permutation(numbers);
    TEST_CHECK(before == after);

    // unordered_set with exactly one element
    numbers.push_back(1);
    before = vector<int>(numbers.begin(), numbers.end());  // transforming unordered_set into vector
    after = permutation(numbers);
    TEST_CHECK(before == after);

    // ensure that it is possible to achieve a permutation identical to the original, without requiring the unordered_set to remain unchanged.
    numbers.push_back(2);
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
    { "test_simd_euclideanDistance", test_simd_euclideanDistance },
    { "test_setIn", test_setIn },
    { "test_mapKeyExists", test_mapKeyExists},
    { "test_setSubtraction", test_setSubtraction },
    { "test_PQSubtraction", test_PQSubtraction},
    { "test_setUnion", test_setUnion },
    { "test_sampleFromSet", test_sampleFromSet },
    { "test_permutation", test_permutation },
    { NULL, NULL }     // zeroed record marking the end of the list
};