#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "board.h"
using namespace std; 

vector<vector<double>> read_in() {
    // A function to read in the raw data from file
    vector<vector<double>> raw_data;
    string file_name = "3_sided_start_inter_data.txt";
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
            // If it is a start position
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
            // If it is an inter position
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


vector<double> get_state_raw(Board game_board) {
    vector<double> raw_state = vector<double>(10);
    for (int i = 0; i < 5; i++) {
        raw_state[i] = double(game_board.stop_positions[i][0]);
        raw_state[i+5] = raw_state[i];
    }
    for (int i = 0; i < 3; i++) {
        int runner_column = game_board.runner_positions[i][0];
        if (runner_column == -1) {
            continue;
        }
        int runner_height = game_board.runner_positions[i][1];
        raw_state[runner_column+5] = double(runner_height);
    }
    return raw_state;
}

vector<vector<double>> training_data(vector<vector<double>> raw_data) {
    // A function to format the data into a training data set
    // Inputs for the network will be data from a single column:
        // Column progress (as a %)
        // Stop height (as a %)
    // Output from the network will be:
        // Stop (0) or roll (1)
    vector<vector<double>> formatted_data;
    Board game_board;
    game_board.init(3);
    int num_games = 1000;
    int num_diff_rolls = 8;
    for (int game_num = 0; game_num < num_games; game_num++) {
        std::cout << game_num << "\n";
        game_board.reset_board();
        while (game_board.game_over() == -1) {
            // Get stop value
            vector<double> raw_state = get_state_raw(game_board);
            vector<double> roll_state;
            double stop_val = -1.0;
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
                if (match == 1) {
                    // Add some constant as punishment for stopping
                    stop_val = raw_data[i][10] + 0.2;
                    break;
                }
            }
            // Generate some roll data
            double roll_val = 0.0;
            int roll_count = 0;
            // Roll num_diff_rolls times
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
                    for (int k = 0; k < raw_data.size(); k++) {
                        int match = 1;
                        for (int l = 0; l < 10; l++) {
                            if (raw_data[k][l] != roll_state[l]) {
                                match = 0;
                                break;
                            }
                        }
                        // Once a matching state is found, see if it is best so far
                        if (match == 1) {
                            // std::cout << "HERE\n";
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
            // Average roll values
            if (roll_count > 0) {
                roll_val = roll_val / double(roll_count);
            }
            // Format data for the network
            vector<double> one_state_data = vector<double>(11);
            for (int i = 0; i < 5; i++) {
                one_state_data[2*i] = double(raw_state[i])/double(game_board.len_columns[i]);
                one_state_data[(2*i)+1] = double(raw_state[i+5])/double(game_board.len_columns[i]);
            }
            // Get the expected output from the network
            one_state_data[10] = 0.0;
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
                // End turn twice to return turn to p0 (solitaire)
                game_board.end_turn();
                game_board.end_turn();
                continue;
            }
            // Three quaters of the time roll and move randomly
            vector<int> roll = game_board.get_roll();
            vector<vector<int>> pairs = game_board.get_pairs(roll);
            rand_choice = rand() % 3;
            int temp_turn = game_board.turn;
            game_board.make_move(pairs[rand_choice][0], pairs[rand_choice][1]);
            // If player goes bust, return the turn
            if (game_board.turn != temp_turn) {
                game_board.end_turn();
            }
        }
    }
    std::cout << "There are " << formatted_data.size() << " elements in the formatted data\n";
    return formatted_data; 
}

int write_to_csv(vector<vector<double>> formatted_data) {
    ofstream myfile;
    myfile.open("stop_training_data.csv", ofstream::out | ofstream::trunc);
    myfile << "SH0,RH0,SH1,RH1,SH2,RH2,SH3,RH3,SH4,RH4,OUTPUT\n";
    for (int i = 0; i < formatted_data.size(); i++) {
        char buffer[100];
        int buf_len = sprintf(buffer, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", 
                formatted_data[i][0], formatted_data[i][1], formatted_data[i][2], formatted_data[i][3], formatted_data[i][4], formatted_data[i][5], 
                formatted_data[i][6], formatted_data[i][7], formatted_data[i][8], formatted_data[i][9], formatted_data[i][10]);
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