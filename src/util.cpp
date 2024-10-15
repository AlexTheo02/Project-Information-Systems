#include "util.hpp"

// Calculates the euclidian distance between two vectors of the same size
float euclideanDistance(vector<float> v1, vector<float> v2){
    int dim1 = v1.size();
    int dim2 = v2.size();

    if (dim1!=dim2){
        cout<<"Dimension mismatch, dim1 is: "<<dim1 <<" dim2 is: "<<dim2 << endl;
        return -1;
    }

    if (v1.empty()){
        cout << "Both vectors are empty" << endl;
        return -1;
    }
    
    float sum = 0;

    for (int i = 0; i < dim1; i++)
        sum += pow((v1[i] - v2[i]), 2);

    return sqrt(sum);
}
