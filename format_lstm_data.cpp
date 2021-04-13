#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <math.h>
using namespace std; 



// A TensorFlow LSTM network can only take integer values as input so the data must be reformtted
int main(int argc, char** argv) {

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
    vector<vector<int>> formatted_data;
    for (int i = 0; i < full_data.size(); i++) {
        vector<int> formatted_line;
        // Reformat and add the stop and runner values
        for (int j = 0; j < 5; j++) {
            double stop_entry = full_data[i][2*j];
            double runner_entry = full_data[i][(2*j) + 1];
            int stop_int = floor((stop_entry * 100.0) + 2.0);
            int runner_int = floor((runner_entry * 100.0) + 103.0);
            formatted_line.push_back(stop_int);
            formatted_line.push_back(runner_int);
        }
        // Reformat and add the roll/stop score
        formatted_line.push_back(int(full_data[i][10]));
        formatted_data.push_back(formatted_line);
    }

    // Output to a new csv
    ofstream myfile;
    myfile.open("formatted_stop_training_data.csv", ofstream::out | ofstream::trunc);
    myfile << "SH0,RH0,SH1,RH1,SH2,RH2,SH3,RH3,SH4,RH4,OUTPUT\n";
    for (int i = 0; i < formatted_data.size(); i++) {
        char buffer[100];
        int buf_len = sprintf(buffer, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", 
                formatted_data[i][0], formatted_data[i][1], formatted_data[i][2], formatted_data[i][3], formatted_data[i][4], formatted_data[i][5], 
                formatted_data[i][6], formatted_data[i][7], formatted_data[i][8], formatted_data[i][9], formatted_data[i][10]);
        myfile << buffer;
    }
    myfile.close();
}