/* 
Author: Daniel Marsh
Project: Yale CPSC 490 Senior Project
Description: This is the core game play/rule implementaion for Can't Stop
*/
#include <iostream>
#include <vector>
#include <string>
#include <set>
using namespace std; 
// Only define BOARD_H if it has not yet been defined
#ifndef BOARD_H
#define BOARD_H

// Main Board class
class Board {
    public:
        // Public variables that are used to define the game state
        int turn = 0;
        int dice_size;
        int num_columns;
        // Runner positions: A vector of three pairs (one per runner) each storing {runner column, runner height}
                        //   If a runner is not yet in a column, their column is set to -1
        vector<vector<int>> runner_positions = {{-1, 0}, {-1, 0}, {-1, 0}};
        // Stop positions: A vector of x pairs where x is the number of columns, each pair stores {p0 stop height, p1 stop height}
        vector<vector<int>> stop_positions;
        // Column Lengths: A vector of ints used to define the length of each column
        vector<int> len_columns;
        // Base Column Lengths: A vector of ints used to define the column lengths of the first 6 columns of the full game
                            //  This vector is used to determine the length of all columns in the game depending in dice size
        vector<int> base_column_len = {3, 5, 7, 9, 11, 13};
        // Column Probabilities: A vector of doubles representing the probability that a given column is rolled
        vector<double> column_probs;

        // A function to setup the initial state of the board
        int init(int given_dice_size) {
            vector<double> column_counts;
            // Get the dice size
            dice_size = given_dice_size;
            // Calculate the number of columns
            num_columns = (dice_size * 2) - 1;
            // Figure out the middle column (used for getting column lengths)
            int middle_column = (num_columns / 2) + 1;
            // Until the middle column, draw column lengths directly from the base_column_len vector
            for (int i = 0; i < middle_column; i++) {
                len_columns.push_back(base_column_len[i]);
                stop_positions.push_back({0, 0});
                column_counts.push_back(0.0);
            }
            // After the middle column, start decreasing the column length
            for (int i = 0; i + middle_column < num_columns; i++){
                len_columns.push_back(base_column_len[middle_column - i - 2]);
                stop_positions.push_back({0, 0});
                column_counts.push_back(0.0);
            }
            // Get the column probabilities by iterating through all possible rolls
            for (int a = 0; a < dice_size; a++) {
                for (int b = 0; b < dice_size; b++) {
                    for (int c = 0; c < dice_size; c++) {
                        for (int d = 0; d < dice_size; d++) {
                            set<double> combos;
                            combos.insert(double(a + b));
                            combos.insert(double(a + c));
                            combos.insert(double(a + d));
                            combos.insert(double(b + c));
                            combos.insert(double(b + d));
                            combos.insert(double(c + d));
                            vector<double> combos_vec(combos.begin(), combos.end());
                            for (int i = 0; i < combos.size(); i++) {
                                column_counts[combos_vec[i]]++;
                            }
                        }
                    }
                }
            }
            // Normalize the probabilities (0 to 1)
            for (int i = 0; i < num_columns; i++) {
                double total_rolls = double(dice_size*dice_size*dice_size*dice_size);
                column_probs.push_back(column_counts[i]/total_rolls);
            }
            return 1;
        }

        // A function to reset the board to the initial state
        int reset_board() {
            // Reset turn
            turn = 0;
            // Take all runners off the board
            runner_positions = {{-1, 0}, {-1, 0}, {-1, 0}};
            int middle_column = (num_columns / 2) + 1;
            // Reset the stops
            for (int i = 0; i < num_columns; i++){
                stop_positions[i] = {0, 0};
            }
            return 1;
        }
            
        // A function to make a clone of another board 
        // (other board must be of same size)
        // (current board must be initialized)
        int clone(Board original) {
            // Set turn
            turn = original.turn;
            // Set dice size
            dice_size = original.dice_size;
            // Set num columns
            num_columns = original.num_columns;
            // Set runners
            for (int i = 0; i < 3; i++) {
                runner_positions[i][0] = original.runner_positions[i][0];
                runner_positions[i][1] = original.runner_positions[i][1];
            }
            // Set stop positions and column lengths
            for (int i  = 0; i < num_columns; i++) {
                stop_positions[i][0] = original.stop_positions[i][0];
                stop_positions[i][1] = original.stop_positions[i][1];
                len_columns[i] = original.len_columns[i];
            }
            return 1;
        }

        // A function to end a given turn
        // This advances stop positions, removes runners, and changes turn
        int end_turn() {
            int ith_runner_column;
            int ith_runner_height;
            // For each runner, check if it is on the board
            for (int i = 0; i < 3; i++) {
                ith_runner_column = runner_positions[i][0];
                ith_runner_height = runner_positions[i][1];
                // If the runner is on the board, move the corresponding stop forward accordingly
                if (ith_runner_height > 0) {
                    stop_positions[ith_runner_column][turn] = ith_runner_height;
                }
                // Reset the runner
                runner_positions[i] = {-1, 0};
            }
            // Change turn
            turn = (turn + 1) % 2;
            return 1;
        }

        // A function to generate a new roll
        vector<int> get_roll() {
            // Get four random numbers from 1 to dice_size
            vector<int> new_roll;
            for (int i = 0; i < 4; i++) {
                new_roll.push_back((rand() % dice_size) + 1);
            }
            // Return the randomly generated roll
            return new_roll;
        }

        // A function to check if a player has won (1 or 0 == game over, -1 == game continues)
        int game_over() {
            int won_columns_p0 = 0;
            int won_columns_p1 = 0;
            // Count how many columns each player has won
            for (int i = 0; i < num_columns; i++) {
                // If player 0 has won the ith column, add to p0s count
                if (stop_positions[i][0] == len_columns[i]) {
                    won_columns_p0++;
                }
                // If player 1 has won the ith column, add to p1s count
                else if (stop_positions[i][1] == len_columns[i]) {
                    won_columns_p1++;
                }
                // If either player has won three columns at any point, return the winner value
                if (won_columns_p0 == 3) {
                    return 0;
                }
                if (won_columns_p1 == 3) {
                    return 1;
                }
            }
            // If no player has won three columns return -1
            return -1;
        }

        // A function to make a given move
        int make_move(int column_a, int column_b = -1) {
            // Try moving in each column (Use -2 to shift from roll value to column index)
            // First try moving in column a
            int move_a = move_runner(column_a - 2);
            // Column b is an optional argument, so only move in that column if an argument is provided
            int move_b = -1;
            if (column_b != -1) {
                move_b = move_runner(column_b - 2);
            }
            // If either cloumn contains a valid move, return 1 (after having made the move(s))
            if ((move_a == 1) || (move_b == 1)) {
                return 1;
            }
            // If neither column has a valid move, go_bust and return -1
            go_bust();
            return -1;
        }

        // A function to check if a roll pairing makes you go bust
        int goes_bust(int column_a, int column_b) {
            // Check each column
            int move_a = try_runner(column_a - 2);
            int move_b = try_runner(column_a - 2);
            // If either cloumn contains a valid move, return 0 since you do not go bust
            if ((move_a == 1) || (move_b == 1)) {
                return 0;
            }
            // If neither column has a valid move, return 1 since you will go bust
            return 1;
        }

        // A function to generate all possible dice pairings given a roll
        vector<vector<int>> get_pairs(vector<int> cur_roll) {
            vector<vector<int>> pairs;
            // Manually generated the three possible pairings
            pairs.push_back({cur_roll[0] + cur_roll[1], cur_roll[2] + cur_roll[3]});
            pairs.push_back({cur_roll[0] + cur_roll[2], cur_roll[2] + cur_roll[3]});
            pairs.push_back({cur_roll[0] + cur_roll[3], cur_roll[1] + cur_roll[2]});
            return pairs;
        }

        // A function to get the height difference between runners and their stops
        vector<double> get_runner_diffs() {
            vector<double> runner_diffs = {0.0, 0.0, 0.0};
            // For each runner, get the height difference
            for (int i = 0; i < 3; i++) {
                int runner_column = runner_positions[i][0];
                // If the runner is on the board
                if (runner_column != -1) {
                    // Get the runner difference (0 to 1)
                    double runner_height = double(runner_positions[i][1]);
                    double stop_height = double(stop_positions[runner_column][turn]);
                    double column_length = double(len_columns[runner_column]);
                    runner_diffs[i] = (runner_height - stop_height) / column_length;
                    if (runner_diffs[i] > 1) {
                        runner_diffs[i] = 1.0;
                    }
                }
            }
            return runner_diffs;
        }

        // A function to print the position of runners, stops etc.
        // Used in debugging
        int print_state() {
            std::cout << "It is Player " << turn << "'s turn\n";
            std::cout << "Player 0's stops:\n";
            for (int i = 0; i < num_columns; i++) {
                std::cout << i+2 << ": " << stop_positions[i][0] << "\n";
            }
            std::cout << "Player 1's stops:\n";
            for (int i = 0; i < num_columns; i++) {
                std::cout << i+2 << ": " << stop_positions[i][1] << "\n";
            }
            std::cout << "Current Runners:\n";
            for (int i = 0; i < 3; i++) {
                std::cout << "Column: " << runner_positions[i][0] << " - Height: " << runner_positions[i][1] << "\n";
            }
            return 1;
        }

        // A function to display the board
        // Used in debugging
        int display_board() {
            vector<vector<char>> board_image;
            int middle_column_index = (num_columns / 2);
            int max_height = len_columns[middle_column_index] + 1;
            for (int i = 0; i < max_height; i++) {
                vector<char> cur_line;
                for (int j = 0; j < num_columns; j++) {
                    if (i > len_columns[j]) {
                        cur_line.push_back('-');
                    }
                    else if ((stop_positions[j][0] == i) && (stop_positions[j][1] == i)) {
                        cur_line.push_back('B');
                    }
                    else if (stop_positions[j][0] == i) {
                        cur_line.push_back('0');
                        continue;
                    }
                    else if (stop_positions[j][1] == i) {
                        cur_line.push_back('1');
                        continue;
                    }
                    else if (i == len_columns[j]) {
                        cur_line.push_back('*');
                    }
                    else {
                        cur_line.push_back('+');
                    }
                }
                board_image.push_back(cur_line);
            }
            int count = 0;
            while ((count < num_columns) && (count < 7)) {
                std::cout << count+2 << "   ";
                count++;
            }
            while (count < num_columns) {
                std::cout << count+2 << "  ";
                count++;
            }
            std::cout << "\n";
            for (int i = max_height - 1; i > -1; i--) {
                for (int j = 0; j < num_columns; j++) {
                    std::cout << board_image[i][j] << "   ";
                }
                std::cout << "\n";
            }
            count = 0;
            while ((count < num_columns) && (count < 7)) {
                std::cout << count+2 << "   ";
                count++;
            }
            while (count < num_columns) {
                std::cout << count+2 << "  ";
                count++;
            }
            std::cout << "\n";
            return 1;
        }



    private:
        // A function to check if a runner can move in the given column
        int try_runner(int runner_column) {
            // If either player has locked the column, return -1 (cannot advance runner)
            int column_height = len_columns[runner_column];
            if ((stop_positions[runner_column][0] == column_height) || (stop_positions[runner_column][1] == column_height)) {
                return -1;
            }
            // Look if there is a runner in this column yet
            int free_runner = -1;
            for (int i = 0; i < 3; i++) {
                // If there is a runner in this column
                if (runner_positions[i][0] == runner_column) {
                    // If that runner is already at the top, return -1 (cannot move any more)
                    if (runner_positions[i][1] == len_columns[runner_column]) {
                        return -1;
                    }
                    // Otherwise return 1 (you can move the runner)
                    return 1;
                }
                // If the current runner is not in the right column, check if it is free to be added (remember this for later)
                else if (runner_positions[i][0] == -1) {
                    free_runner = i;
                }
            }
            // If none of the runners are in the column already and there are no free runners, return -1 (cannot move runner)
            if (free_runner == -1) {
                return -1;
            }
            // If there is a free runner, you can advance a runner in that column, return 1
            return 1;
        }

        // A functin to move a runner in a given column if possible, otheriwse, return -1 
        int move_runner(int runner_column) {
            // If either player has locked the column, return -1 (cannot advance runner)
            if ((stop_positions[runner_column][0] == len_columns[runner_column]) || (stop_positions[runner_column][1] == len_columns[runner_column])) {
                return -1;
            }
            // Look if there is a runner in this column yet
            int free_runner = -1;
            for (int i = 0; i < 3; i++) {
                // If there is a runner in this column
                if (runner_positions[i][0] == runner_column) {
                    // If that runner is already at the top, return -1 (cannot move any more)
                    if (runner_positions[i][1] == len_columns[runner_column]) {
                        return -1;
                    }
                    // Otherwise advance the runner and return 1 (success)
                    runner_positions[i][1]++;
                    return 1;
                }
                // If the current runner is not in the right column, check if it is free to be added (remember this for later)
                else if (runner_positions[i][0] == -1) {
                    free_runner = i;
                }
            }
            // If none of the runners are in the column already and there are no free runners, return -1 (cannot move runner)
            if (free_runner == -1) {
                return -1;
            }
            // If there is a free runner, add it to the board one space ahead of the current players stopper in that column
            int column_stop = stop_positions[runner_column][turn];
            runner_positions[free_runner] = {runner_column, column_stop + 1};
            return 1;
        }

        // If player has no valid moves, reset runners and change turn
        int go_bust() {
            runner_positions = {{-1, 0}, {-1, 0}, {-1, 0}};
            turn = (turn + 1) % 2;
            return 1;
        }

};
#endif
