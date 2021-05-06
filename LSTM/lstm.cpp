/* 
Author: Daniel Marsh
Project: Yale CPSC 490 Senior Project
Description: Main program for the LSTM agent.
*/
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>
#include <stdio.h>
#include "../board.h"
#include "lstm.h"
using namespace std; 

// Function called to make a move with the LSTM agent
Board make_lstm_move(Board game_board) {
    // Get network input data (stop height %, runner height %, free runner)
    vector<vector<double>> input_data;
    int turn = game_board.turn;
    // Get data about each column
    for (int i = 0; i < game_board.num_columns; i++) {
        vector<double> column_data = vector<double>(3);
        double col_height = double(game_board.len_columns[i]);
        // Stop position
        column_data[0] = double(game_board.stop_positions[i][turn])/col_height;
        // Set runner pos to stop pos by default
        column_data[1] = column_data[0];
        // Set free runner to false by default
        column_data[2] = 0.0;
        // Get true runner position and free runner value
        for (int j = 0; j < 3; j++) {
            if (game_board.runner_positions[j][0] == -1) {
                column_data[2] = 1.0;
            }
            else if (game_board.runner_positions[j][0] == i) {
                column_data[1] = double(game_board.runner_positions[j][1])/col_height;
            }
        }
        input_data.push_back(column_data);
    }
    // Format input data to LSTM network
    vector<double> format_in;
    // Run through each column
    for (int i = 0; i < game_board.num_columns; i++) {
        // Get the stop positions as a perecentage
        double istop_perc = double(game_board.stop_positions[i][turn])/double(game_board.len_columns[i]);
        format_in.push_back(istop_perc);
        // Add the stop position as the runner position initially (this gets updated later)
        format_in.push_back(istop_perc);
        format_in.push_back(game_board.column_probs[i]);
    }
    // Check runner positions
    for (int i = 0; i < 3; i++) {
        // Get data about this runner
        int runner_col = game_board.runner_positions[i][0];
        double runner_pos = double(game_board.runner_positions[i][1]);
        // If this runner is on the board, update the data about the corresponding column
        if (runner_col != -1) {
            double runner_perc = runner_pos / double(game_board.len_columns[runner_col]);
            format_in[(3*runner_col)+1] = runner_perc;
        }
    }
    // Send data to LSTM input file
    ofstream lstmfile;
    lstmfile.open("LSTM/lstm_input.txt", ofstream::out | ofstream::trunc);
    for (int i = 0; i < format_in.size()-1; i++) {
        char buffer [20];
        sprintf(buffer, "%lf,", format_in[i]);
        lstmfile << buffer;
    }
    char buffer [20];
    sprintf(buffer, "%lf\n", format_in[format_in.size()-1]);
    lstmfile << buffer;
    lstmfile.close();
    // Call LSTM network to choose whether to roll or stop
    double roll_stop = 0.0;
    std::cout << "        CALLING LSTM\n";
    // System call to the LSTM network
    string command = "python3 LSTM/call_lstm_model.py";
    system(command.c_str());
    // Get result from LSTM output file
    string result_lstm;
    ifstream lstmresultfile("LSTM/lstm_output.txt");
    getline(lstmresultfile, result_lstm);
    roll_stop = stod(result_lstm.c_str());
    std::cout << "           Output val: " << roll_stop << "\n";

    // If network is less than 99% confident with rolling, then stop some percenatage of the time
    double conf_val = 0.99;
    if (roll_stop < conf_val) {
        // Function to determine how often to stop (more likley to stop at lower confidence values)
        int uncert = ceil((1.0 - roll_stop)*15.0) + 1;
        // Make a weighted choice as to whether to stop
        int rand_choice = rand() % uncert;
        if (rand_choice != 0) {
            game_board.end_turn();
            return game_board;
        }
    }
    // If the agent did not stop, choose which roll to select
    // Get roll and pairs
    vector<int> roll = game_board.get_roll();
    vector<vector<int>> pairs = game_board.get_pairs(roll);
    vector<vector<double>> roll_data;
    // For each possible roll pairing, save the data about the columns that will be moved in
    for (int i = 0; i < 3; i++) {
        // Get roll values (-2 since we need to 0 index rather than 2 index)
        int roll_1 = pairs[i][0] - 2;
        int roll_2 = pairs[i][1] - 2;
        // Store data about each column that will be moved in by this roll
        vector<double> roll_1_data = input_data[roll_1];
        vector<double> roll_2_data = input_data[roll_2];
        roll_data.push_back(roll_1_data);
        roll_data.push_back(roll_2_data);
    }
    // Write the roll data to the roll network input file
    ofstream myfile;
    myfile.open("Roll_Network/roll_input.txt", ofstream::out | ofstream::trunc);
    for (int i = 0; i < 6; i++) {
        char buffer [50];
        sprintf(buffer, "%lf, %lf, %lf\n", roll_data[i][0], roll_data[i][1], roll_data[i][2]);
        myfile << buffer;
    }
    myfile.close();
    // Call the Roll network to choose a pair
    int max_roll = 0;
    std::cout << "        CALLING ROLL\n";
    // System call to the roll network
    string command_roll = "python3 Roll_Network/call_roll_model.py";
    system(command_roll.c_str());
    // Get result from roll network output file
    string result;
    ifstream resultfile("Roll_Network/roll_output.txt");
    getline(resultfile, result);
    max_roll = atoi(result.c_str());
    // Make the move suggested by the roll network
    game_board.make_move(pairs[max_roll][0], pairs[max_roll][1]);
    return game_board;
}