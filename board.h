#include <iostream>
#include <vector>
#include <string>
using namespace std; 

#ifndef BOARD_H
#define BOARD_H

class Board {
    public:
        int turn = 0;
        int dice_size;
        int num_columns;
        vector<vector<int>> runner_positions = {{-1, 0}, {-1, 0}, {-1, 0}};
        vector<vector<int>> stop_positions;
        vector<int> len_columns;
        vector<int> base_column_len = {3, 5, 7, 9, 11, 13};
        // Setup the values of all pieces on the board
        int init(int given_dice_size) {
            dice_size = given_dice_size;
            num_columns = (dice_size * 2) - 1;
            int middle_column = (num_columns / 2) + 1;
            for (int i = 0; i < middle_column; i++) {
                len_columns.push_back(base_column_len[i]);
                stop_positions.push_back({0, 0});
            }
            for (int i = 0; i + middle_column < num_columns; i++){
                len_columns.push_back(base_column_len[middle_column - i - 2]);
                stop_positions.push_back({0, 0});
            }
            return 1;
        }

        // Reset the board to the initial state
        int reset_board() {
            turn = 0;
            runner_positions = {{-1, 0}, {-1, 0}, {-1, 0}};
            int middle_column = (num_columns / 2) + 1;
            for (int i = 0; i < num_columns; i++){
                stop_positions[i] = {0, 0};
            }
            return 1;
        }
            

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

        // Player chooses to stop
        // Advance any stops in rows with runners
        // Reset the runners to off the board
        // Switch turn
        int end_turn() {
            int ith_runner_column;
            int ith_runner_height;
            for (int i = 0; i < 3; i++) {
                ith_runner_column = runner_positions[i][0];
                ith_runner_height = runner_positions[i][1];
                if (ith_runner_height > 0) {
                    stop_positions[ith_runner_column][turn] = ith_runner_height;
                }
                runner_positions[i] = {-1, 0};
            }
            turn = (turn + 1) % 2;
            return 1;
        }

        // Obtain a new roll
        vector<int> get_roll() {
            // Get four random number from 1-dice_size
            vector<int> new_roll;
            for (int i = 0; i < 4; i++) {
                new_roll.push_back((rand() % dice_size) + 1);
            }
            return new_roll;
        }

        // Check if a player had won (1 == game over, -1 == game continues)
        int game_over() {
            // Count how many columns each player has won
            int won_columns_p0 = 0;
            int won_columns_p1 = 0;
            for (int i = 0; i < num_columns; i++) {
                if (stop_positions[i][0] == len_columns[i]) {
                    won_columns_p0++;
                }
                else if (stop_positions[i][1] == len_columns[i]) {
                    won_columns_p1++;
                }
                // If either player has won three columns, return 1
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

        // Try moving runners in two given columns (given as the number that would be shown on the board)
        // Go bust if no runners can be moved
        int make_move(int column_a, int column_b = -1) {
            // Try moving in each column (Use -2 to shift from roll value to column index)
            int move_a = move_runner(column_a - 2);
            int move_b = -1;
            if (column_b != -1) {
                move_b = move_runner(column_b - 2);
            }
            // If either cloumn contains a valid move, return 1 (after having made the moves)
            if ((move_a == 1) || (move_b == 1)) {
                return 1;
            }
            // If neither column has a valid move, go_bust and return -1
            go_bust();
            return -1;
        }

        // Check if a roll pairing makes you go bust
        int goes_bust(int column_a, int column_b) {
            int move_a = try_runner(column_a);
            int move_b = try_runner(column_a);
            // If either cloumn contains a valid move, return 0 since you do not go bust
            if ((move_a == 1) || (move_b == 1)) {
                return 0;
            }
            // If neither column has a valid move, return 1 since you will go bust
            return 1;
        }

        // Given four numbers from the roll, make the three possible pairs
        vector<vector<int>> get_pairs(vector<int> cur_roll) {
            vector<vector<int>> pairs;
            pairs.push_back({cur_roll[0] + cur_roll[1], cur_roll[2] + cur_roll[3]});
            pairs.push_back({cur_roll[0] + cur_roll[2], cur_roll[2] + cur_roll[3]});
            pairs.push_back({cur_roll[0] + cur_roll[3], cur_roll[1] + cur_roll[2]});
            return pairs;
        }

        vector<double> get_runner_diffs() {
            vector<double> runner_diffs = {0.0, 0.0, 0.0};
            for (int i = 0; i < 3; i++) {
                int runner_column = runner_positions[i][0];
                if (runner_column != -1) {
                    double runner_height = double(runner_positions[i][1]);
                    double stop_height = double(stop_positions[runner_column][turn]);
                    double column_length = double(len_columns[runner_column]);
                    runner_diffs[i] = (runner_height - stop_height) / column_length;
                }
            }
            return runner_diffs;
        }

        // Print the position of runners, stops etc.
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

        // Display the board
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
        // Check if a runner can move in the given column
        int try_runner(int runner_column) {
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


        // Move a runner in a given column if possible, otheriwse, return -1 
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