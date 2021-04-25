/* 
Author: Daniel Marsh
Project: Yale CPSC 490 Senior Project
Description: Core program for making move decisions with the Autoencoder.
*/
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <stdio.h>
#include "../board.h"
#include "autoencoder.h"
using namespace std; 

// Function to make a move with the saved autoencoder model
Board make_autoencoder_move(Board game_board) {
    // Generate the input data
    vector<vector<double>> input_data;
    int turn = game_board.turn;
    // Get data from each column
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
    // Format the input data
    vector<double> format_in;
    // Get stop positions
    for (int i = 0; i < game_board.num_columns; i++) {
        // Get the stop positions
        double istop_perc = double(game_board.stop_positions[i][turn])/double(game_board.len_columns[i]);
        format_in.push_back(istop_perc);
        format_in.push_back(istop_perc);
        format_in.push_back(game_board.column_probs[i]);
    }
    // Check runners to see if their column data needs updating
    for (int i = 0; i < 3; i++) {
        int runner_col = game_board.runner_positions[i][0];
        double runner_pos = double(game_board.runner_positions[i][1]);
        if (runner_col != -1) {
            double runner_perc = runner_pos / double(game_board.len_columns[runner_col]);
            format_in[(3*runner_col)+1] = runner_perc;
        }
    }
    // Print data to autoencoder input file
    ofstream lstmfile;
    lstmfile.open("Autoencoder/autoencoder_input.txt", ofstream::out | ofstream::trunc);
    for (int i = 0; i < format_in.size()-1; i++) {
        char buffer [20];
        sprintf(buffer, "%lf,", format_in[i]);
        lstmfile << buffer;
    }
    char buffer [20];
    sprintf(buffer, "%lf\n", format_in[format_in.size()-1]);
    lstmfile << buffer;
    lstmfile.close();
    // Call Autoencoder network to choose roll or stop (pass all column data)
    double roll_stop = 0.0;
    std::cout << "        CALLING AUTOENCODER\n";
    // System call to autoencoder network
    string command = "python3 Autoencoder/call_autoencoder.py";
    system(command.c_str());
    // Get result from autoencoder output file
    string result_auto;
    ifstream autoresultfile("Autoencoder/autoencoder_output.txt");
    getline(autoresultfile, result_auto);
    roll_stop = stod(result_auto.c_str());
    std::cout << "           Output val: " << roll_stop << "\n";
    // If network is less than conf_val% confident with rolling, then stop some percenatage of the time
    double conf_val = 0.85;
    if (roll_stop < conf_val) {
        int uncert = ceil((conf_val - roll_stop)*15.0) + 1;
        int rand_choice = rand() % uncert;
        // Stop some percentage of the time
        if (rand_choice != 0) {
            game_board.end_turn();
            return game_board;
        }
    }
    // If the autoencoder network says not to stop, then roll
    vector<int> roll = game_board.get_roll();
    vector<vector<int>> pairs = game_board.get_pairs(roll);
    // Generate roll network input
    vector<vector<double>> roll_data;
    for (int i = 0; i < 3; i++) {
        int roll_1 = pairs[i][0] - 2;
        int roll_2 = pairs[i][1] - 2;
        vector<double> roll_1_data = input_data[roll_1];
        vector<double> roll_2_data = input_data[roll_2];
        roll_data.push_back(roll_1_data);
        roll_data.push_back(roll_2_data);
    }
    // Send roll data to roll network input file
    ofstream myfile;
    myfile.open("Roll_Network/roll_input.txt", ofstream::out | ofstream::trunc);
    for (int i = 0; i < 6; i++) {
        char buffer [50];
        sprintf(buffer, "%lf, %lf, %lf\n", roll_data[i][0], roll_data[i][1], roll_data[i][2]);
        myfile << buffer;
    }
    myfile.close();
    // Call Roll net to choose roll
    int max_roll = 0;
    std::cout << "        CALLING ROLL\n";
    // System call to roll network
    string command_roll = "python3 Roll_Network/call_roll_model.py";
    system(command_roll.c_str());
    // Get result from roll network output file
    string result;
    ifstream resultfile("Roll_Network/roll_output.txt");
    getline(resultfile, result);
    max_roll = atoi(result.c_str());
    // Make move as directed by the output from the roll network
    game_board.make_move(pairs[max_roll][0], pairs[max_roll][1]);
    return game_board;
}
