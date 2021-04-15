#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <math.h>
using namespace std; 



// A TensorFlow LSTM network can only take integer values as input so the data must be reformtted
int main(int argc, char** argv) {

    double col0_prob = 33.0/81.0;
    double col1_prob = 50.0/81;
    double col2_prob = 70.0/81.0;
    vector<double> col_probs = {col0_prob, col1_prob, col2_prob, col1_prob, col0_prob};

    // Read in the data
    fstream fin;
    fin.open("stop_training_data.csv", ios::in);
    string line, entry;
    vector<vector<double>> full_data;
    getline(fin, line);
    while (getline(fin, line)) {
        vector<double> row;
        stringstream s(line);
        while (getline(s, entry, ',')) {
            double entry_val = stod(entry);
            row.push_back(entry_val);
        }
        full_data.push_back(row);
    }

    // Reformat for an LSTM network
        // The stop values will be between 2 and 102
        // The runner values will be between 103 and 203
    vector<vector<double>> formatted_data;
    for (int i = 0; i < full_data.size(); i++) {
        vector<double> formatted_line;
        // Reformat and add the stop, runner, and probability values
        for (int j = 0; j < 5; j++) {
            double stop_entry = full_data[i][2*j];
            double runner_entry = full_data[i][(2*j) + 1];
            formatted_line.push_back(stop_entry);
            formatted_line.push_back(runner_entry);
            formatted_line.push_back(col_probs[j]);
        }
        // Reformat and add the roll/stop score
        formatted_line.push_back(full_data[i][10]);
        formatted_data.push_back(formatted_line);
    }

    // Output to a new csv
    ofstream myfile;
    myfile.open("formatted_stop_training_data.csv", ofstream::out | ofstream::trunc);
    myfile << "SH0,RH0,P0,SH1,RH1,P1,SH2,RH2,P2,SH3,RH3,P3,SH4,RH4,P4,OUTPUT\n";
    for (int i = 0; i < formatted_data.size(); i++) {
        char buffer[200];
        int buf_len = sprintf(buffer, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", 
                formatted_data[i][0], formatted_data[i][1], formatted_data[i][2], formatted_data[i][3], formatted_data[i][4], formatted_data[i][5], 
                formatted_data[i][6], formatted_data[i][7], formatted_data[i][8], formatted_data[i][9], formatted_data[i][10],
                formatted_data[i][11], formatted_data[i][12], formatted_data[i][13], formatted_data[i][14], formatted_data[i][15]);
        myfile << buffer;
    }
    myfile.close();
}