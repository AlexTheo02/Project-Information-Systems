#include "interface.hpp"

using namespace std;

int main (int argc, char* argv[]) {
    
    // !!IMPORTANT: Arguments given are correct, no checking will be made
    // program_name data_file data_dim out_file n_reduced queries_file queries_dim groundtruth_path
    
    string data_file = argv[1];
    int data_dim = atoi(argv[2]);
    string queries_file = argv[3];
    int queries_dim = atoi(argv[4]);
    string groundtruth_path = argv[5];

    // Read data
    vector<vector<float>> data,queries;
    ReadBin(data_file, data_dim, data);
    // Read queries
    ReadBin(queries_file, queries_dim, queries);
    cout << "Data read successfully" << endl;

    vector<vector<Id>> groundtruth = generateGroundtruth(data, queries);

    ofstream groundtruthOutfile(groundtruth_path, ios::out);
    if (!groundtruthOutfile.is_open()){
        cerr << "Error: could not open " << groundtruth_path << " for writing" << endl;
        return EXIT_FAILURE;
    }

    groundtruthOutfile << groundtruth;

    groundtruthOutfile.close();

    cout << "Groundtruth stored successfully at: " << groundtruth_path << endl;

    return 0;
}