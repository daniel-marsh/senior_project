/* 
Author: Daniel Marsh
Project: Yale CPSC 490 Senior Project
Description: Program to generate data to train the autoencoder network.
*/
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "../board.h"
using namespace std; 

// Function used to create a vector representation of the current state
vector<double> get_state(Board game_board) {
    vector<double> state;
    // Get initial state info (stop height, column probabilities)
    // Initialize runner heights to stop height (gets checked/updated later)
    for (int i = 0; i < game_board.num_columns; i++) {
        double stop_height = (double)game_board.stop_positions[i][0] / (double)game_board.len_columns[i];
        double runner_height = stop_height;
        double column_prob = game_board.column_probs[i];
        state.push_back(stop_height);
        state.push_back(runner_height);
        state.push_back(column_prob);
    }
    // Update the runners if needed
    for (int i = 0; i < 3; i++) {
        // If runner is not on the board, skip it
        int runner_col = game_board.runner_positions[i][0];
        if (runner_col == -1) {
            continue;
        }
        // Otherwise update the corresponding runner height value accordingly
        double runner_height = double(game_board.stop_positions[runner_col][0]) / double(game_board.len_columns[runner_col]);
        state[(runner_col*3)+1] = runner_height;
    }
    // Pad the data for smaller boards
    while (state.size() < 33) {
        state.push_back(-1.0);
    }
    return state;
}

// Function to generate the data
vector<vector<double>> get_data(int dice_size) {
    vector<vector<double>> state_data;
    // Setup a game board to simulate some games
    Board game_board;
    game_board.init(dice_size);
    int num_states = 0;
    int num_required = 150000;
    // Run simulations until we have enough data
    while (num_states < num_required) {
        // Play the game until game over
        while (game_board.game_over() == -1) {
            // Get the current state in vector form 
            vector<double> cur_state = get_state(game_board);
            state_data.push_back(cur_state);
            num_states++;
            // Play randomly (stop 1/3 of the time)
            int stop = rand() % 3;
            // 1/3 of the time, stop and return the turn (solitaire for data generation)
            if (stop == 0) {
                game_board.end_turn();
                game_board.end_turn();
                continue;
            }
            // 2/3 of the time, roll and pick a random roll
            vector<int> roll = game_board.get_roll();
            vector<vector<int>> pairs = game_board.get_pairs(roll);
            int roll_pick = rand() % 3;
            game_board.make_move(pairs[roll_pick][0], pairs[roll_pick][1]);
            // If the player goes bust, return the turn (solitaire for data generation)
            if (game_board.turn != 0) {
                game_board.end_turn();
            }
        }
        // Reset the game board for the next game simulation
        game_board.reset_board();
    }
    return state_data;
}

// Function to write the generated data to a file in append mode
int write_to_csv(vector<vector<double>> formatted_data, int data_size) {
    ofstream myfile;
    myfile.open("./data/new_auto_training_data.csv", ofstream::out | ofstream::app);
    // Write each line of data to the CSV
    for (int i = 0; i < formatted_data.size(); i++) {
        for (int j = 0; j < data_size-1; j++) {
            char buffer[20];
            sprintf(buffer, "%lf,", formatted_data[i][j]);
            myfile << buffer;
        }
        char buffer[20];
        sprintf(buffer, "%lf\n", formatted_data[i][data_size-1]);
        myfile << buffer;
    }
    myfile.close();
    return 1;
}

// Function used to erase a CSV file and set the header
int write_header() {
    ofstream myfile;
    // Truncate mode used to erase file
    myfile.open("./data/new_auto_training_data.csv", ofstream::out | ofstream::trunc);
    for (int i = 0; i < 10; i++) {
        char buffer[20];
        sprintf(buffer, "SH%d,RH%d,CP%d,", i, i, i);
        myfile << buffer;
    }
    char buffer[20];
    sprintf(buffer, "SH10,RH10,CP10\n");
    myfile << buffer;
    myfile.close();
    return 1;
}

int main(int argc, char** argv) {
    // Set a new seed each time to get new data with each call
    srand(time(NULL));
    // Write the header/erase the file
    write_header();
    // Get data for the game with 3-sided dice
    std::cout << "Getting data for d=" << 3 << "...\n";
    vector<vector<double>> formatted_data = get_data(3);
    // Write to CSV
    std::cout << "Writing to csv...\n";
    write_to_csv(formatted_data, formatted_data[0].size());
    // Get data for the full game (6-sided dice)
    std::cout << "Getting data for d=" << 6 << "...\n";
    formatted_data = get_data(6);
    // Write to CSV
    std::cout << "Writing to csv...\n";
    write_to_csv(formatted_data, formatted_data[0].size());
}
