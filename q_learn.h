#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <numeric>
#include <iostream>
#include <fstream>
#include "board.h"
using namespace std; 

// A Q-learning agent
class Q_agent {
    public:
        // Q values of the buckets
        vector<vector<vector<vector<double>>>> q_vals;

        // A function to initialize the q values
        // The values are initialized to 0.5 if no file is given
        // Or, the data in a given file is used to initialize the buckets
        int init(string file_name) {
            vector<vector<vector<int>>> bucket_counts;
            q_vals = vector<vector<vector<vector<double>>>>(10);
            bucket_counts = vector<vector<vector<int>>>(10);
            // First set all values to 0.5
            for (int x = 0; x < 10; x++) {
                q_vals[x] = vector<vector<vector<double>>>(10);
                bucket_counts[x] = vector<vector<int>>(10);
                for (int y = 0; y < 10; y++) {
                    q_vals[x][y] = vector<vector<double>>(10);
                    bucket_counts[x][y] = vector<int>(10);
                    for (int z = 0; z < 10; z++) {
                        q_vals[x][y][z] = {0.5, 0.5};
                        bucket_counts[x][y][z] = 0;
                    }
                }
            }
            // If there is no file given, return
            if (file_name.compare("") == 0) {
                return 1;
            }
            // Otherwise open the file and read each line
            ifstream my_file(file_name);
            string line;
            if (my_file.is_open()) {
                // Read each line
                while (getline(my_file, line)) {
                    // Each line in the file contains two key pieces data:
                        // The position of the player's stops
                        // The expected number of turns to complete three columns (noted here as score)
                    vector<int> board_pos = {line[7]-'0', line[9]-'0', line[11]-'0', line[13]-'0', line[15]-'0'};
                    vector<int> bucket_vals = get_bucket(board_pos, {0.0, 0.0, 0.0});
                    double score = stod(line.substr(19));
                    // Update the buckets
                    for (int z = 0; z < 10; z++) {
                        q_vals[bucket_vals[0]][bucket_vals[1]][z][0] += score;
                        q_vals[bucket_vals[0]][bucket_vals[1]][z][1] += score;
                        bucket_counts[bucket_vals[0]][bucket_vals[1]][z]++;
                    }
                }
                // Go through and normalize any buckets that were added to more than once
                for (int x = 0; x < 10; x++) {
                    for (int y = 0; y < 10; y++) {
                        for (int z = 0; z < 10; z++) {
                            if (bucket_counts[x][y][z] > 0) {
                                double new_val = (q_vals[x][y][z][0] - 0.5) / double(bucket_counts[x][y][z]);
                                q_vals[x][y][z] = {new_val, new_val};
                            }
                        }
                    }
                }
            }
            // If the file did not open, return an error message (but not exit execution)
            else {
                std::cout << line << " is not a valid file name\n";
                return 0;
            }
            // All has completed, so return
            return 1;
        }

        // A function to train the q_learning agent
        int train() {
            return 1;
        }

        // A function to make move decision based on the trained agent
        Board make_move(Board game_board) {
            // Figure out the curreny bucket
            vector<int> bucket_ind = get_bucket_helper(game_board);
            // Get the action values
            double stop_val = q_vals[bucket_ind[0]][bucket_ind[1]][bucket_ind[2]][0];
            double roll_val = q_vals[bucket_ind[0]][bucket_ind[1]][bucket_ind[2]][1];
            // If stopping predicts a lesser number of turns to win, stop
            if (stop_val < roll_val) {
                game_board.end_turn();
                return game_board;
            }
            // Otherwise, roll, get pairs and then see which move would result in best q_val
            vector<int> roll = game_board.get_roll();
            vector<vector<int>> pairs = game_board.get_pairs(roll);
            double min_moves = INFINITY;
            int min_index = 0;
            for (int i = 0; i < 3; i++) {
                Board copy_board;
                copy_board.init(game_board.dice_size);
                copy_board.clone(game_board);
                copy_board.make_move(pairs[i][0], pairs[i][1]);
                vector<int> copy_bucket_ind = get_bucket_helper(copy_board);
                double best_move = min(q_vals[copy_bucket_ind[0]][copy_bucket_ind[1]][copy_bucket_ind[2]][0], q_vals[copy_bucket_ind[0]][copy_bucket_ind[1]][copy_bucket_ind[2]][1]);
                if (best_move < min_moves) {
                    min_moves = best_move;
                    min_index = i;
                }
            }
            // Make the best estimated move and return the resulting game board
            game_board.make_move(pairs[min_index][0], pairs[min_index][1]);
            game_board.end_turn();
            return game_board;
        }

    private:
        static vector<int> get_bucket_helper(Board game_board) {
            // Get all salient board info
            // Then figure out which bucket we are in
            int turn = game_board.turn;
            int num_columns = game_board.num_columns;
            vector<int> my_progress;
            vector<vector<int>> all_stops = game_board.stop_positions;
            for (int i = 0; i < num_columns; i++) {
                my_progress.push_back(all_stops[i][turn]);
            } 
            vector<vector<int>> my_runners = game_board.runner_positions;
            for (int i = 0; i < 3; i++) {
                if (my_runners[i][0] != -1) {
                    int runner_column = my_runners[i][0];
                    my_progress[runner_column] = my_runners[i][1];
                }
            }
            vector<double> runner_diffs = game_board.get_runner_diffs();
            return get_bucket(my_progress, runner_diffs);
        }
        // A function to determine the current bucket
        static vector<int> get_bucket(vector<int> progress, vector<double> runner_diff_percentages) {
            // Get the number of columns and the middle column (used for column lengths)
            int num_columns = progress.size();
            int mid_column = (num_columns+1)/2;
            vector<double> column_lengths = {3.0, 5.0, 7.0, 9.0, 11.0, 13.0};
            // Keep track of the three bucket indicators:
                // Total runner progress
                // Top three runner progress
                // Distance between runners and stops
            double total_progress = 0.0;
            vector<double> top_three_progress = {0.0, 0.0, 0.0};
            // Iterate through the columns and track each value
            for (int i = 0; i < mid_column; i++) {
                double column_progress = double(progress[i]) / column_lengths[i];
                total_progress += column_progress;
                for (int j = 0; j < 3; j++) {
                    if (column_progress > top_three_progress[j]) {
                        for (int k = 2; k > j; k--) {
                            top_three_progress[k] = top_three_progress[k-1];
                        }
                        top_three_progress[j] = column_progress;
                        break;
                    }
                }
            }
            // Continue the iteration (column lengths decrease at this point)
            for (int i = 0; i + mid_column < num_columns; i++) {
                double column_progress = progress[i + mid_column] / column_lengths[mid_column - i - 1];
                total_progress += column_progress;
                for (int j = 0; j < 3; j++) {
                    if (column_progress > top_three_progress[j]) {
                        for (int k = 2; k > j; k--) {
                            top_three_progress[k] = top_three_progress[k-1];
                        }
                        top_three_progress[j] = column_progress;
                        break;
                    }
                }
            }
            // Normalize the values found and return the bucket
            total_progress = total_progress / double(num_columns);
            double top_three_progress_val = accumulate(top_three_progress.begin(), top_three_progress.end(), 0.0);
            top_three_progress_val = top_three_progress_val / 3.0;
            double runner_diffs = accumulate(runner_diff_percentages.begin(), runner_diff_percentages.end(), 0.0);
            vector<int> return_vals = {int(trunc(total_progress*10)), int(trunc(top_three_progress_val*10)), int(trunc(runner_diffs*10))};
            // If any normalized value is exactly 1.0, there would be a seg fault as we try to access too far out. (Just include these in previous bucket)
            for (int i = 0; i < 3; i++) {
                if (return_vals[i] == 10) {
                    return_vals[i]--;
                }
            }
            // Return the bucket
            return return_vals;
        }
};
