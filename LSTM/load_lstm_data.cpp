/* 
Author: Daniel Marsh
Project: Yale CPSC 490 Senior Project
Description: Program to generate raw data to be later formatted and used to train the LSTM network.
                Raw data is generated ny randomly simulating many games. 
*/
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "../board.h"
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
            // If it is a start position, runners and stops are aligned
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
            // If it is an inter position, runner positions differ in at least one place
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

// A function to get the currenyt game state in a readable format
vector<double> get_state_raw(Board game_board) {
    vector<double> raw_state = vector<double>(10);
    // Get the state of each stop
    for (int i = 0; i < 5; i++) {
        // Set the stop position values
        raw_state[i] = double(game_board.stop_positions[i][0]);
        // Initially set the runner positions equal to the stop in that column
        raw_state[i+5] = raw_state[i];
    }
    // Check if any runner values differ from their stops
    for (int i = 0; i < 3; i++) {
        int runner_column = game_board.runner_positions[i][0];
        // If this runner is not on the board, ignore it
        if (runner_column == -1) {
            continue;
        }
        // Otherwise update the runner value in the appropriate column
        int runner_height = game_board.runner_positions[i][1];
        raw_state[runner_column+5] = double(runner_height);
    }
    return raw_state;
}
// A function to format the data into a training data set
    // Inputs for the network will be data from a single column:
        // Column progress (as a %)
        // Stop height (as a %)
    // Output from the network will be:
        // Stop (0) or roll (1)
vector<vector<double>> training_data(vector<vector<double>> raw_data) {
    vector<vector<double>> formatted_data;
    // Initialize the game board with dice with 3 sides
    Board game_board;
    game_board.init(3);
    // Generate data by playing 5 games and checking 8 different rolls and each state (to prevent too much random noise in the data)
    int num_games = 5;
    int num_diff_rolls = 8;
    // Simulate the games
    for (int game_num = 0; game_num < num_games; game_num++) {
        // Reset the board
        std::cout << game_num << "\n";
        game_board.reset_board();
        // Simulate a single game until game over
        while (game_board.game_over() == -1) {
            // Get the reward for stopping
            vector<double> raw_state = get_state_raw(game_board);
            vector<double> roll_state;
            double stop_val = -1.0;
            // Look for the state that represents stopping here
            for (int i = 0; i < raw_data.size(); i++) {
                int match = 1;
                for (int k = 0; k < 5; k++) {
                    // Are stops moved up to runners?
                    if (raw_data[i][k] != raw_state[k+5]) {
                        match = 0;
                        break;
                    }
                    // Are runners aligned with stops?
                    if (raw_data[i][k+5] != raw_state[k+5]) {
                        match = 0;
                        break;
                    }
                }
                // If this is a matching state, save the value of stopping and move one
                if (match == 1) {
                    // Add some constant as punishment for stopping
                    stop_val = raw_data[i][10] + 0.2;
                    break;
                }
            }
            // Generate some roll data
            double roll_val = 0.0;
            int roll_count = 0;
            // Test 8 different rolls from this state
            for (int i = 0; i < num_diff_rolls; i++) {
                // For each loop, get a new roll
                vector<int> roll = game_board.get_roll();
                vector<vector<int>> pairs = game_board.get_pairs(roll);
                // Make the three possible moves and pick the best one
                double min_val = 100.0;
                for (int j = 0; j < 3; j++) {
                    Board copy_board;
                    copy_board.init(3);
                    copy_board.clone(game_board);
                    copy_board.make_move(pairs[j][0], pairs[j][1]);
                    if (copy_board.turn != game_board.turn) {
                        game_board.end_turn();
                    }
                    // Find the value of the state after choosing a roll
                    roll_state = get_state_raw(copy_board);
                    // Look for the state corresponding to a given roll choice
                    for (int k = 0; k < raw_data.size(); k++) {
                        int match = 1;
                        for (int l = 0; l < 10; l++) {
                            if (raw_data[k][l] != roll_state[l]) {
                                match = 0;
                                break;
                            }
                        }
                        // Once a matching state is found, see if it is best so far and move onto the next possible roll choice
                        if (match == 1) {
                            double cur_roll_val = raw_data[k][10];
                            if (cur_roll_val < min_val) {
                                min_val = cur_roll_val;
                            }
                            break;
                        }
                    }
                }
                // If there was a valid roll, add this to the roll values
                if (min_val != 100.0) {
                    roll_val += min_val;
                    roll_count++;
                }
            }
            // Average the 8 different roll values
            if (roll_count > 0) {
                roll_val = roll_val / double(roll_count);
            }
            // Format data for the network
            vector<double> one_state_data = vector<double>(11);
            for (int i = 0; i < 5; i++) {
                one_state_data[2*i] = double(raw_state[i])/double(game_board.len_columns[i]);
                one_state_data[(2*i)+1] = double(raw_state[i+5])/double(game_board.len_columns[i]);
            }
            // Get the expected output for the network
            one_state_data[10] = 0.0;
            // If rolling is better than stopping, the expected output is 1.0, otherwise the expected output is 0.0
            if (roll_val < stop_val) {
                one_state_data[10] = 1.0;
            }
            // If there was no recognized possible rolls, do not add data (undefined behavior)
            if (roll_count > 0) {
                formatted_data.push_back(one_state_data);
            }
            // Now that data from this state has been added, move into another state randomly
            int rand_choice = rand() % 4;
            // A quater of the time stop
            if (rand_choice == 0) {
                // End turn twice to return turn to p0 (solitaire game for data generation)
                game_board.end_turn();
                game_board.end_turn();
                continue;
            }
            // Roll three quaters of the time and move randomly
            vector<int> roll = game_board.get_roll();
            vector<vector<int>> pairs = game_board.get_pairs(roll);
            rand_choice = rand() % 3;
            int temp_turn = game_board.turn;
            game_board.make_move(pairs[rand_choice][0], pairs[rand_choice][1]);
            // If player goes bust, return the turn (solitaire game for data generation)
            if (game_board.turn != temp_turn) {
                game_board.end_turn();
            }
        }
    }
    // Print statement to show how much data was generated
    std::cout << "There are " << formatted_data.size() << " elements in the formatted data\n";
    return formatted_data; 
}

// Function to write data to CSV in append mode
int write_to_csv(vector<vector<double>> formatted_data) {
    ofstream myfile;
    myfile.open("../data/stop_training_data.csv", ofstream::out | ofstream::app);
    for (int i = 0; i < formatted_data.size(); i++) {
        char buffer[100];
        int buf_len = sprintf(buffer, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", 
                formatted_data[i][0], formatted_data[i][1], formatted_data[i][2], formatted_data[i][3], formatted_data[i][4], formatted_data[i][5], 
                formatted_data[i][6], formatted_data[i][7], formatted_data[i][8], formatted_data[i][9], formatted_data[i][10]);
        myfile << buffer;
    }
    myfile.close();
    return 1;
}

// Main function to generate data
int main(int argc, char** argv) {
    srand(time(NULL));
    // Read in raw data used to determine best move
    std::cout << "READING IN RAW DATA...\n";
    vector<vector<double>> raw_data = read_in();
    // Run the loop many times to generate data and write to CSV
    for (int i = 0; i < 1000; i++) {
        std::cout << "FORMATTING RAW DATA...\n";
        vector<vector<double>> formatted_data = training_data(raw_data);
        std::cout << "WRITING DATA TO CSV...\n";
        write_to_csv(formatted_data);
    }
    return 1;
}
