#include "interface.hpp"

using namespace std;

int main (int argc, char* argv[]) {
    
    // Arguments given are correct, no checking will be made
    // program_name data_file data_dim out_file n_reduced queries_file queries_dim groundtruth_path
    
    string data_file = argv[1];
    int data_dim = atoi(argv[2]);
    string out_file = argv[3];
    int n_reduced = atoi(argv[4]);
    string queries_file = argv[5];
    int queries_dim = atoi(argv[6]);
    string groundtruth_path = argv[7];

    // Read data
    vector<vector<float>> data,queries;
    ReadBin(data_file, data_dim, data);
    // Read queries
    ReadBin(queries_file, queries_dim, queries);
    cout << "Data read successfully" << endl;

    vector<vector<float>> reducedData;
    unordered_set<int> visited;
    int ri = 0;
    srand(time(nullptr));
    for (int i=0; i<atoi(argv[4]); i++){
        // Sample a random index from the original file
        do{
            ri = rand() % data.size();
        } while (visited.find(ri) != visited.end());
        visited.insert(ri);
        reducedData.push_back(data[ri]);
    }

    // Write reduced data to output file
    ofstream outfile(out_file, ios::out);
    if (!outfile.is_open()){
        cerr << "Error: could not open " << out_file << " for writing" << endl;
        return EXIT_FAILURE;
    }

    outfile << reducedData;

    outfile.close();
    cout << "Reduced data successfuly to " << n_reduced << " points and stored at: " << out_file << endl;

    vector<vector<Id>> groundtruth = generateGroundtruth(reducedData, queries);

    ofstream groundtruthOutfile(groundtruth_path, ios::out);
    if (!groundtruthOutfile.is_open()){
        cerr << "Error: could not open " << groundtruth_path << " for writing" << endl;
        return EXIT_FAILURE;
    }

    groundtruthOutfile << groundtruth;

    groundtruthOutfile.close();

    cout << "Reduced groundtruth stored successfully at: " << groundtruth_path << endl;

    return 0;
}