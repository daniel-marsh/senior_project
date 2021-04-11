#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <stdio.h>
#include "board.h"
#include "lstm.h"
using namespace std; 


Board make_lstm_move(Board game_board) {
    // Get network input data (stop height %, runner height %, free runner)
    vector<vector<double>> input_data;
    int turn = game_board.turn;
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

    // Roll or Stop

    // Call LSTM network to choose roll or stop (pass all column data)
    int roll_stop = 0;
    if (input_data[0][2] == 1.0) {
        roll_stop = 1;
    }
    // DO CALL TO NET HERE


    if (roll_stop == 0) {
        game_board.end_turn();
        return game_board;
    }

    // Roll Choice (if not stop)

    // Get roll
    vector<int> roll = game_board.get_roll();
    vector<vector<int>> pairs = game_board.get_pairs(roll);
    vector<vector<double>> roll_data;
    for (int i = 0; i < 3; i++) {
        int roll_1 = pairs[i][0] - 2;
        int roll_2 = pairs[i][1] - 2;
        vector<double> roll_1_data = input_data[roll_1];
        vector<double> roll_2_data = input_data[roll_2];
        roll_data.push_back(roll_1_data);
        roll_data.push_back(roll_2_data);
    }
    ofstream myfile;
    myfile.open("roll_input.txt", ofstream::out | ofstream::trunc);
    for (int i = 0; i < 6; i++) {
        char buffer [50];
        sprintf(buffer, "%lf, %lf, %lf\n", roll_data[i][0], roll_data[i][1], roll_data[i][2]);
        myfile << buffer;
    }
    myfile.close();
    // Call Roll net to choose roll (pass column data for each roll and take max)
    int max_roll = 0;
    // CALL TO ROLL NET HERE
    string command = "python3 call_roll_model.py";
    system(command.c_str());
    // Get result
    string result;
    ifstream resultfile("roll_output.txt");
    getline(resultfile, result);
    max_roll = atoi(result.c_str());

    game_board.make_move(pairs[max_roll][0], pairs[max_roll][1]);

    return game_board;
}