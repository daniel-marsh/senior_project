/*
Author: Daniel Marsh
Project: Yale CPSC 490 Senior Project
Description: Program to load in raw data for the roll selection network
*/
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std; 

 // A function to read in the raw data from file
vector<vector<double>> read_in() {
    vector<vector<double>> raw_data;
    string file_name = "../data/3_sided_start_inter_data.txt";
    ifstream my_file(file_name);
    string line;
    if (my_file.is_open()) {
        while (getline(my_file, line)) {
            vector<double> data_line = vector<double>(11);
            // Stop positions
            data_line[0] = double(line[7]-'0');
            data_line[1] = double(line[9]-'0');
            data_line[2] = double(line[11]-'0');
            data_line[3] = double(line[13]-'0');
            data_line[4] = double(line[15]-'0');
            // If it is a start position, runners are aligned with stops
            if (line[0] == 'S') {
                // Runner positions
                data_line[5] = double(line[7]-'0');
                data_line[6] = double(line[9]-'0');
                data_line[7] = double(line[11]-'0');
                data_line[8] = double(line[13]-'0');
                data_line[9] = double(line[15]-'0');
                // Score
                data_line[10] = stod(line.substr(19));
            }
            // If it is an inter position, get the runner positions
            else {
                // Runner positons
                data_line[5] = double(line[19]-'0');
                data_line[6] = double(line[21]-'0');
                data_line[7] = double(line[23]-'0');
                data_line[8] = double(line[25]-'0');
                data_line[9] = double(line[27]-'0');
                // Score
                data_line[10] = stod(line.substr(31));
            }
            raw_data.push_back(data_line);
        }   
    }
    return raw_data;
}

// A function to format the data into a training data set
    // Inputs for the network will be data from a single column:
        // Column progress (as a %)
        // Stop height (as a %)
        // Free runner?
    // Output from the network will be:
        // Net decrease in number of turns to win 
vector<vector<double>> training_data(vector<vector<double>> raw_data) {
    // Initialize some important values
    vector<vector<double>> formatted_data;
    vector<double> column_lens = {3.0, 5.0, 7.0, 5.0, 7.0};
    std::cout << "    Data to format: " << raw_data.size() << "\n";
    // Run through the raw data
    for (int i = 0; i < raw_data.size(); i++) {
        if (i % 100 == 99) {
            std::cout << "    Line #" << i+1 << "\n";
        }
        // Get the score of the initial position
        double start_score = raw_data[i][10];
        // If start position is a winning position, do not save data
        if (start_score == 0.0) {
            continue;
        }
        // See how many runners are on the board
        int runners_on = 0;
        for (int column = 0; column < 5; column++) {
            // If the runner height is different from the stop height
            if (raw_data[i][column] != raw_data[i][column+5]) {
                runners_on++;
            }
        }
        // If there are less than 3 runners on the board, there is a free runner
        double free_runner = 1.0;
        if (runners_on > 2) {
            free_runner = 0.0;
        }
        // For each column, test the result of moving in that column
        for (int column = 0; column < 5; column++) {
            vector<double> data_line = vector<double>(4);
            // Stop height
            data_line[0] = raw_data[i][column]/column_lens[column];
            // Runner height 
            data_line[1] = raw_data[i][column+5]/column_lens[column];
            // Free runner
            data_line[2] = free_runner;
            // If runner or stop is at top, no need to compute since you can't move
            if ((data_line[0] == 1.0) || (data_line[1] == 1.0)) {
                data_line[3] = 0.0;
                formatted_data.push_back(data_line);
                continue;
            }
            // If no free runners and no runner in this column, no need to compute since that results in a bust
            if ((data_line[2] == 0.0) && (data_line[0] == data_line[1])) {
                data_line[3] = 0.0;
                formatted_data.push_back(data_line);
                continue;
            }
            // If runner is not at the top of its column and you can move in this column:
            // Figure out benefit of making move
            double next_score = -1.0;
            // Find corresponding next state
            for (int j = 0; j < raw_data.size(); j++) {
                int match = 1;
                // Check each column of this state
                for (int k = 0; k < 10; k++) {
                    // If this column is the one we want the runner to advance one spot in
                    // Check if the runner in the state is one step ahead of the runner in the previous state
                    if (k == column+5) {
                        if (raw_data[j][k] != raw_data[i][k]+1.0) {
                            match = 0;
                            break;
                        }
                    }
                    // Otherwise make sure the stops and runners match up
                    else {
                        if (raw_data[j][k] != raw_data[i][k]) {
                            match = 0;
                            break;
                        }
                    } 
                }
                // If this is a matching state, save the value and break the loop
                if (match == 1) {
                    next_score = raw_data[j][10];
                    break;
                }
            }
            // If a match is found, save the resulting difference to the data vector and continue
            if (next_score != -1.0) { 
                data_line[3] = start_score - next_score;
                formatted_data.push_back(data_line);
            }
            // If no match is found, we cannot save the data
        }
    }
    return formatted_data;
}

// Function to write formatted data to a CSV
int write_to_csv(vector<vector<double>> formatted_data) {
    ofstream myfile;
    myfile.open("../data/roll_training_data.csv", ofstream::out | ofstream::trunc);
    // Write out a header
    myfile << "Stop Height,Runner Height,Free Runner,Score Diff\n";
    // Write out the data
    for (int i = 0; i < formatted_data.size(); i++) {
        char buffer[50];
        int buf_len = sprintf(buffer, "%lf,%lf,%lf,%lf\n", formatted_data[i][0], formatted_data[i][1], formatted_data[i][2], formatted_data[i][3]);
        myfile << buffer;
    }
    return 1;
}

int main(int argc, char** argv) {
    std::cout << "READING IN RAW DATA...\n";
    vector<vector<double>> raw_data = read_in();
    std::cout << "FORMATTING RAW DATA...\n";
    vector<vector<double>> formatted_data = training_data(raw_data);
    std::cout << "WRITING DATA TO CSV...\n";
    write_to_csv(formatted_data);
    return 1;
}