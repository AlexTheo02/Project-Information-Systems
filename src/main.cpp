#include <iostream>
#include "types.hpp"

using namespace std;

int main () {

    DirectedGraph G;

    cout << "Hello World!\n";

    set<int> n;
    n.insert(1);

    vector<float> v1,v2;
    v1.push_back(2);
    v2.push_back(2);

    cout << euclideanDistance(v1,v2) << "\n";


    cout << setIn(1, n);

    return 0;
}