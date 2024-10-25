#include <iostream>

#include "util.hpp"
#include "types.hpp"


using namespace std;

int main () {

    DirectedGraph<vector<float>> DG(euclideanDistance<vector<float>>);

    return 0;

}