#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <numeric>
#include <fstream>
#include "../board.h"
using namespace std; 

// A Q-learning agent
class Q_agent {
    public:
        // Q values of the buckets
        vector<vector<vector<vector<double>>>> q_vals;
        vector<vector<vector<vector<double>>>> learning_rate;
        double default_val = 1.0;
        double alpha_init = 0.47;
        // A function to initialize the q values
        // The values are initialized to 0.5 if no file is given
        // Or, the data in a given file is used to initialize the buckets
        int init(string file_name) {
            vector<vector<vector<int>>> bucket_counts;
            q_vals = vector<vector<vector<vector<double>>>>(10);
            learning_rate = vector<vector<vector<vector<double>>>>(10);
            bucket_counts = vector<vector<vector<int>>>(10);
            // First set all values to 0.5
            for (int x = 0; x < 10; x++) {
                q_vals[x] = vector<vector<vector<double>>>(10);
                learning_rate[x] = vector<vector<vector<double>>>(10);
                bucket_counts[x] = vector<vector<int>>(10);
                for (int y = 0; y < 10; y++) {
                    q_vals[x][y] = vector<vector<double>>(10);
                    learning_rate[x][y] = vector<vector<double>>(10);
                    bucket_counts[x][y] = vector<int>(10);
                    for (int z = 0; z < 10; z++) {
                        q_vals[x][y][z] = {default_val, default_val};
                        learning_rate[x][y][z] = {alpha_init, alpha_init};
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
                                double new_val = (q_vals[x][y][z][0] - default_val) / double(bucket_counts[x][y][z]);
                                q_vals[x][y][z] = {new_val, new_val};
                            }
                        }
                    }
                }
            }
            // If the file did not open, return an error message (but not exit execution)
            else {
                std::cout << file_name << " is not a valid file name\n";
                return 0;
            }
            // All has completed, so return
            return 1;
        }

        // A function to train the q_learning agent
        int train(int seconds, int dice_size) {
            // Create the training board
            Board game_board;
            game_board.init(dice_size);

            // TRAINING PARAMS
            double epsilon = 0.3;
            double min_epsilon = 0.085;
            // double alpha = 0.1;
            double gamma = 0.99;
            double e_decay = 0.97;
            double a_decay = 0.991;
            // REWARD SHAPING
            // The penalty for stopping or going bust
            double stop_reward = 0.02;
            // The reward for successfully rolling
            double bust_init = -0.04;
            // Cost for going bust
            double bust_cost = 0.01;
            // Reward for winning
            double win_reward = -1.0;

            // Time to stop training
            time_t start = time(NULL);
            int game_count = 1;
            // While there is time left, train
            while(time(NULL) - start < seconds) {
                // After a certain number of games, reduce the exploration rate (epsilon)
                    // Decay rate varies depending on how long the training session is
                if ((epsilon > min_epsilon) && (game_count % seconds == 0)) {
                    epsilon = epsilon * e_decay;
                    // DEBUG PRINTS
                    // std::cout << "Trained on " << game_count << " simulations: ";
                    // std::cout << seconds - int(trunc(time(NULL) - start)) << " seconds of training remaining\n";
                }
                game_count++;
                // Play the game to a terminal state
                while (game_board.game_over() == -1) {
                    vector<int> cur_bucket_inds = get_bucket_helper(game_board);
                    vector<int> next_bucket_inds;
                    // Decide whether to explore or exploit
                    double r = ((double)rand() / (RAND_MAX));
                    // If r is less than epsilon, explore
                    if (r < epsilon) {
                        r = ((double)rand() / (RAND_MAX));
                        // Stop half of the time, 
                        if (r < 0.5) {
                            game_board.end_turn();
                            double reward_shape = stop_reward;
                            if (game_board.game_over() != -1) {
                                reward_shape = win_reward;
                            }
                            // cur_q_val is q val for stopping in current state
                            double cur_q_val = q_vals[cur_bucket_inds[0]][cur_bucket_inds[1]][cur_bucket_inds[2]][0];
                            // next_q_val is q val of rolling in next state (next state is same as current state except runner_diffs = 0)
                            double next_q_val = q_vals[cur_bucket_inds[0]][cur_bucket_inds[1]][0][1];
                            // Get current learning rate, and decrease it for future visits
                            double cur_alpha = learning_rate[cur_bucket_inds[0]][cur_bucket_inds[1]][cur_bucket_inds[2]][0];
                            learning_rate[cur_bucket_inds[0]][cur_bucket_inds[1]][cur_bucket_inds[2]][0] = cur_alpha * a_decay;
                            // Update current q_val
                            q_vals[cur_bucket_inds[0]][cur_bucket_inds[1]][cur_bucket_inds[2]][0] = cur_q_val + cur_alpha * (reward_shape + gamma * (next_q_val - cur_q_val));
                            // Then give the turn back (solitaire game mode for training)
                            game_board.end_turn();
                        }
                        // roll half of the time
                        else {
                            vector<int> roll = game_board.get_roll();
                            vector<vector<int>> pairs = game_board.get_pairs(roll);
                            // Current q value is value of rolling from current state
                            double cur_q_val = q_vals[cur_bucket_inds[0]][cur_bucket_inds[1]][cur_bucket_inds[2]][1];
                            // Figure out the best expected roll
                            // Store best pair so far
                            double min_q_val = INFINITY;
                            int min_index = -1;
                            // Keep track of whether a given pair makes you go bust
                            vector<double> bust_ind = {bust_init, bust_init, bust_init};
                            for (int i = 0; i < 3; i++) {
                                // Copy the board and play out the move
                                Board copy_board;
                                copy_board.init(game_board.dice_size);
                                copy_board.clone(game_board);
                                copy_board.make_move(pairs[i][0], pairs[i][1]);
                                // If the move makes you go bust, store the penalty and give the turn back (solitaire for training)
                                if (copy_board.turn != game_board.turn) {
                                    bust_ind[i] = bust_cost;
                                    copy_board.end_turn();
                                }
                                // Get bucket of next state
                                vector<int> possible_next_bucket = get_bucket_helper(copy_board);
                                // Get min of two actions (stop or roll) from that bucket (add the bust value)
                                double next_q_val_stop = q_vals[possible_next_bucket[0]][possible_next_bucket[1]][possible_next_bucket[2]][0];
                                double next_q_val_roll = q_vals[possible_next_bucket[0]][possible_next_bucket[1]][possible_next_bucket[2]][1];
                                double next_q_val = min(next_q_val_roll, next_q_val_stop) + bust_ind[i];
                                // Check if the q value from this roll is better than best seen so far
                                if (next_q_val < min_q_val) {
                                    min_q_val = next_q_val;
                                    min_index = i;
                                }
                            }
                            // Get the best next q value and the reward shape
                            double next_q_val = min_q_val-bust_ind[min_index];
                            double reward_shape = bust_ind[min_index];
                            // Get current learning rate, and decrease it for future visits
                            double cur_alpha = learning_rate[cur_bucket_inds[0]][cur_bucket_inds[1]][cur_bucket_inds[2]][0];
                            learning_rate[cur_bucket_inds[0]][cur_bucket_inds[1]][cur_bucket_inds[2]][0] = cur_alpha * a_decay;
                            // Update the current q_val (rolling in current bucket)
                            q_vals[cur_bucket_inds[0]][cur_bucket_inds[1]][cur_bucket_inds[2]][1] = cur_q_val + cur_alpha * (reward_shape + gamma * (next_q_val - cur_q_val));
                            // Choose the best pair
                            int current_turn = game_board.turn;
                            // Make the move
                            game_board.make_move(pairs[min_index][0], pairs[min_index][1]);
                            // If the move makes the player go bust, give the turn back
                            if (game_board.turn != current_turn) {
                                game_board.end_turn();
                            }
                        }
                    }
                    // If r is greater than epsilon, then exploit
                    else {
                        // See which action has best expected result
                        double stop_val = q_vals[cur_bucket_inds[0]][cur_bucket_inds[1]][cur_bucket_inds[2]][0];
                        double roll_val = q_vals[cur_bucket_inds[0]][cur_bucket_inds[1]][cur_bucket_inds[2]][1];
                        double stop_equal_rand = ((double)rand() / (RAND_MAX));
                        // If stopping is best, stop and update, OR if stopping is the same as rolling, stop half of the time
                        if ((stop_val < roll_val) || ((stop_val == roll_val) && (stop_equal_rand < 0.5))) {
                            game_board.end_turn();
                            double reward_shape = stop_reward;
                            if (game_board.game_over() != -1) {
                                reward_shape = win_reward;
                            }
                            // cur_q_val is q val for stopping in current state
                            double cur_q_val = q_vals[cur_bucket_inds[0]][cur_bucket_inds[1]][cur_bucket_inds[2]][0];
                            // next_q_val is q val of rolling in next state (next state is same as current state except runner_diffs = 0)
                            double next_q_val = q_vals[cur_bucket_inds[0]][cur_bucket_inds[1]][0][1];
                            // Get current learning rate, and decrease it for future visits
                            double cur_alpha = learning_rate[cur_bucket_inds[0]][cur_bucket_inds[1]][cur_bucket_inds[2]][0];
                            learning_rate[cur_bucket_inds[0]][cur_bucket_inds[1]][cur_bucket_inds[2]][0] = cur_alpha * a_decay;
                            // Update current q_val
                            q_vals[cur_bucket_inds[0]][cur_bucket_inds[1]][cur_bucket_inds[2]][0] = cur_q_val + cur_alpha * (reward_shape + gamma * (next_q_val - cur_q_val));
                            // Then give the turn back (solitaire game mode for training)
                            game_board.end_turn();
                        }
                        // Otherwise rolling is better so roll and update
                        else {
                            vector<int> roll = game_board.get_roll();
                            vector<vector<int>> pairs = game_board.get_pairs(roll);
                            // Current q value is value of rolling from current state
                            double cur_q_val = q_vals[cur_bucket_inds[0]][cur_bucket_inds[1]][cur_bucket_inds[2]][1];
                            // Figure out the best expected pair
                            double min_q_val = INFINITY;
                            int min_index = -1;
                            // Store which pairs go bust
                            vector<double> bust_ind = {bust_init, bust_init, bust_init};
                            for (int i = 0; i < 3; i++) {
                                // Copy the board and play out the move
                                Board copy_board;
                                copy_board.init(game_board.dice_size);
                                copy_board.clone(game_board);
                                copy_board.make_move(pairs[i][0], pairs[i][1]);
                                // If the move makes the player go bust, give the turn back (solitaire for training)
                                if (game_board.turn != copy_board.turn) {
                                    bust_ind[i] = bust_cost;
                                    copy_board.end_turn();
                                }
                                // Get bucket of next state
                                vector<int> possible_next_bucket = get_bucket_helper(copy_board);
                                // Get min of two actions from that bucket (stop or roll from next state)
                                double next_q_val_stop = q_vals[possible_next_bucket[0]][possible_next_bucket[1]][possible_next_bucket[2]][0];
                                double next_q_val_roll = q_vals[possible_next_bucket[0]][possible_next_bucket[1]][possible_next_bucket[2]][1];
                                double next_q_val = min(next_q_val_roll, next_q_val_stop) + bust_ind[i];
                                // Check if the q value from this roll is better than best seen so far
                                if (next_q_val < min_q_val) {
                                    min_q_val = next_q_val;
                                    min_index = i;
                                }
                            }
                            // Get the best next q value and the reward shape
                            double next_q_val = min_q_val-bust_ind[min_index];
                            double reward_shape = bust_ind[min_index];
                            // Get current learning rate, and decrease it for future visits
                            double cur_alpha = learning_rate[cur_bucket_inds[0]][cur_bucket_inds[1]][cur_bucket_inds[2]][0];
                            learning_rate[cur_bucket_inds[0]][cur_bucket_inds[1]][cur_bucket_inds[2]][0] = cur_alpha * a_decay;
                            // Update the current q_val (accounting for bust if that occurs)
                            q_vals[cur_bucket_inds[0]][cur_bucket_inds[1]][cur_bucket_inds[2]][1] = cur_q_val + cur_alpha * (reward_shape + gamma * (next_q_val - cur_q_val));
                    
                            // Make the move and update q vals
                            int current_turn = game_board.turn;
                            game_board.make_move(pairs[min_index][0], pairs[min_index][1]);
                            // If the player goes bust, give the turn back
                            if (game_board.turn != current_turn) {
                                game_board.end_turn();
                            }
                        }
                    }
                }
                // Once the game is over, reset the game_board
                game_board.reset_board();
            }
            // DEBUG PRINTS
            // dump_q_vals();
            // std::cout << "NUMBER OF GAMES SIMULATED = " << game_count << "\n";
            // std::cout << epsilon << "\n";

            // Once time is up, return
            return 1;
        }

        // A function to make move decision based on the trained agent
        Board make_move(Board game_board) {
            // Figure out the current bucket
            vector<int> bucket_ind = get_bucket_helper(game_board);
            // Get the action values
            double stop_val = q_vals[bucket_ind[0]][bucket_ind[1]][bucket_ind[2]][0];
            double roll_val = q_vals[bucket_ind[0]][bucket_ind[1]][bucket_ind[2]][1];
            // If stopping predicts a lesser number of turns to win, stop
            if (stop_val < roll_val) {
                game_board.end_turn();
                return game_board;
            }
            // If stopping is no different from rolling, stop half of the time, roll half of the time
            if (stop_val == roll_val) {
                double r = ((double)rand() / (RAND_MAX));
                if (r < 0.5) {
                    game_board.end_turn();
                    return game_board;
                }
            }
            // Otherwise rolling is better, so roll, get pairs, and then see which move would result in best q_val
            vector<int> roll = game_board.get_roll();
            vector<vector<int>> pairs = game_board.get_pairs(roll);
            // Keep track of best move so far
            double min_moves = INFINITY;
            int min_index = -1;
            for (int i = 0; i < 3; i++) {
                // Make a copy of the board
                Board copy_board;
                copy_board.init(game_board.dice_size);
                copy_board.clone(game_board);
                // Make the move on the copied board
                copy_board.make_move(pairs[i][0], pairs[i][1]);
                // If the move makes the player go bust, return the turn (this is just to get an accurate q_val)
                if (copy_board.turn != game_board.turn) {
                    copy_board.end_turn();
                }
                // Get the resulting bucket
                vector<int> copy_bucket_ind = get_bucket_helper(copy_board);
                // See if stopping or rolling is better in next bucket
                double best_move = min(q_vals[copy_bucket_ind[0]][copy_bucket_ind[1]][copy_bucket_ind[2]][0], q_vals[copy_bucket_ind[0]][copy_bucket_ind[1]][copy_bucket_ind[2]][1]);
                // If current move is better than best so far, update best so far
                if (best_move < min_moves) {
                    min_moves = best_move;
                    min_index = i;
                }
            }
            // Make the best estimated move and return the resulting game board
            game_board.make_move(pairs[min_index][0], pairs[min_index][1]);
            return game_board;
        }

    private:

        int dump_q_vals() {
            for (int i = 0; i < 10; i++) {
                for (int j = 0; j < 10; j++) {
                    for (int k = 0; k < 10; k++) {
                        std::cout << "(" << q_vals[i][j][k][0] << ", " << q_vals[i][j][k][1] << ") ";
                    }
                    std::cout << "\n";
                }
                std::cout << "\n";
            }
            return 1;
        }

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
                // Top three column progress
                // Progress in all other columns as a factor of top three progress
                    // % progress in all other columns / % progress in top three columns
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
            double top_three_progress_val = 0;
            for (int i = 0; i < 3; i++) {
                total_progress -= top_three_progress[i];
                top_three_progress_val += top_three_progress[i];
            }
            total_progress = total_progress / double(num_columns - 3);
            top_three_progress_val = top_three_progress_val / 3.0;
            if (top_three_progress_val != 0) {
                total_progress = total_progress / top_three_progress_val;
            }
            double runner_diffs = accumulate(runner_diff_percentages.begin(), runner_diff_percentages.end(), 0.0) / 3;
            vector<int> return_vals = {int(trunc(top_three_progress_val*10)), int(trunc(total_progress*10)), int(trunc(runner_diffs*10))};
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