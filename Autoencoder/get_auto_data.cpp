#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "../board.h"
using namespace std; 

vector<double> get_state(Board game_board) {
    vector<double> state;
    // Get initial state info
    for (int i = 0; i < game_board.num_columns; i++) {
        double stop_height = (double)game_board.stop_positions[i][0] / (double)game_board.len_columns[i];
        double runner_height = stop_height;
        double column_prob = game_board.column_probs[i];
        state.push_back(stop_height);
        state.push_back(runner_height);
        state.push_back(column_prob);
    }
    // Update for the runners
    for (int i = 0; i < 3; i++) {
        // If runner is not on the board, skip it
        int runner_col = game_board.runner_positions[i][0];
        if (runner_col == -1) {
            continue;
        }
        double runner_height = double(game_board.stop_positions[runner_col][0]) / double(game_board.len_columns[runner_col]);
        state[(runner_col*3)+1] = runner_height;
    }
    while (state.size() < 33) {
        state.push_back(-1.0);
    }
    return state;
}

vector<vector<double>> get_data(int dice_size) {
    vector<vector<double>> state_data;
    // Setup a game board to siimulate some games
    Board game_board;
    game_board.init(dice_size);
    // Run simulations until we have enough data
    int num_states = 0;
    int num_required = 150000;
    while (num_states < num_required) {
        // std::cout << "    Simulating new game\n    Num_states=" << num_states << "\n";
        while (game_board.game_over() == -1) {
            vector<double> cur_state = get_state(game_board);
            state_data.push_back(cur_state);
            num_states++;
            int stop = rand() % 3;
            // 1/4 of the time, stop and return the turn
            if (stop == 0) {
                game_board.end_turn();
                game_board.end_turn();
                continue;
            }
            // 3/4 of the time, roll and pick a random roll
            vector<int> roll = game_board.get_roll();
            vector<vector<int>> pairs = game_board.get_pairs(roll);
            int roll_pick = rand() % 3;
            game_board.make_move(pairs[roll_pick][0], pairs[roll_pick][1]);
            // If the player goes bust, return the turn 
            if (game_board.turn != 0) {
                game_board.end_turn();
            }
        }
        game_board.reset_board();
    }
    return state_data;
}

int write_to_csv(vector<vector<double>> formatted_data, int data_size) {
    ofstream myfile;
    myfile.open("./data/new_auto_training_data.csv", ofstream::out | ofstream::app);
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

int write_header() {
    ofstream myfile;
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
    srand(time(NULL));
    write_header();

    std::cout << "Getting data for d=" << 3 << "...\n";
    vector<vector<double>> formatted_data = get_data(3);
    std::cout << "Writing to csv...\n";
    write_to_csv(formatted_data, formatted_data[0].size());

    std::cout << "Getting data for d=" << 6 << "...\n";
    formatted_data = get_data(6);
    std::cout << "Writing to csv...\n";
    write_to_csv(formatted_data, formatted_data[0].size());
    
}
