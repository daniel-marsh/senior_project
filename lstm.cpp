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
    // std::cout << "LSTM TURN\n";
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

    // Generate formatted input data to LSTM network
    // std::cout << "GENERATING INPUT\n";
    vector<int> format_in;
    vector<double> prob_vals;
    double denom = double(game_board.dice_size)*double(game_board.dice_size);
    for (int i = 1; i < game_board.dice_size+1; i++) {
        prob_vals.push_back((double(i)/denom)*175.0/64.0);
    }
    for (int i = game_board.dice_size-2; i >= 0; i--) {
        prob_vals.push_back(prob_vals[i]);
    }
    // Get stop positions
    for (int i = 0; i < game_board.num_columns; i++) {
        // Get the stop positions
        double istop_perc = double(game_board.stop_positions[i][turn])/double(game_board.len_columns[i]);
        int istop_val = floor((istop_perc * 100.0) + 2.0);
        // Set runner positions by default to stop position
        int irun_val = istop_val + 101;
        int iprob_val = floor((prob_vals[i]*100.0) + 204.0);
        format_in.push_back(istop_val);
        format_in.push_back(irun_val);
        format_in.push_back(iprob_val);
    }
    // Check runners
    for (int i = 0; i < 3; i++) {
        int runner_col = game_board.runner_positions[i][0];
        double runner_pos = double(game_board.runner_positions[i][1]);
        if (runner_col != -1) {
            double runner_perc = runner_pos / double(game_board.len_columns[runner_col]);
            int runner_val = floor((runner_perc * 100.0) + 103.0);
            format_in[(3*runner_col)+1] = runner_val;
        }
    }
    // std::cout << "SENDING TO FILE\n";
    // Print to file
    ofstream lstmfile;
    lstmfile.open("lstm_input.txt", ofstream::out | ofstream::trunc);
    for (int i = 0; i < format_in.size()-1; i++) {
        char buffer [10];
        sprintf(buffer, "%d,", format_in[i]);
        lstmfile << buffer;
    }
    char buffer [10];
    sprintf(buffer, "%d\n", format_in[format_in.size()-1]);
    lstmfile << buffer;
    lstmfile.close();

    // Call LSTM network to choose roll or stop (pass all column data)
    double roll_stop = 0.0;
    std::cout << "        CALLING LSTM\n";
    // DO CALL TO NET HERE
    string command = "python3 call_lstm_model.py";
    system(command.c_str());
    // Get result
    string result_lstm;
    ifstream lstmresultfile("lstm_output.txt");
    getline(lstmresultfile, result_lstm);
    roll_stop = stod(result_lstm.c_str());
    std::cout << "           Output val: " << roll_stop << "\n";

    // If network is less than conf_val% confident with rolling, then stop some percenatage of the time
    double conf_val = 0.29;
    if (roll_stop < conf_val) {
        int rand_choice = rand() % 3;
        if (rand_choice != 0) {
            game_board.end_turn();
            return game_board;
        }
    }

    // Roll Choice (if not stop)
    // std::cout << "GENERATING ROLL DATA\n";
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
    // std::cout << "WRITING TO FILE\n";
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
    std::cout << "        CALLING ROLL\n";
    // CALL TO ROLL NET HERE
    string command_roll = "python3 call_roll_model.py";
    system(command_roll.c_str());
    // Get result
    // std::cout << "CALLED ROLL\n";
    string result;
    ifstream resultfile("roll_output.txt");
    getline(resultfile, result);
    max_roll = atoi(result.c_str());
    // std::cout << "GETTING BEST ROLL\n";

    game_board.make_move(pairs[max_roll][0], pairs[max_roll][1]);

    // std::cout << "MADE MOVE\n";

    return game_board;
}