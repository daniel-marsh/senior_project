#include <iostream>
#include <vector>
#include "board.h"
#include "simple.h"
using namespace std; 

Board make_smart_move(Board game_board) {
    // Get info from board
    vector<vector<int>> runners = game_board.runner_positions;
    vector<int> column_lengths = game_board.len_columns;
    vector<vector<int>> stops = game_board.stop_positions;
    int current_player = game_board.turn;
    // Check if all runners are on the board
    vector<int> runner_cols;
    bool free_runner = false;
    bool runner_home =  false;
    bool runner_very_far_ahead_of_stop = false;
    int runners_far_ahead_of_stop = 0;
    int runners_just_ahead_of_stop = 0;
    for (int i = 0; i < 3; i++) {
        // If you find a runner not yet on the board, break out of the loop
        int cur_runner_column = runners[i][0];
        int cur_runner_height = runners[i][1];
        runner_cols.push_back(cur_runner_column);
        int big_dist = 3;
        int medium_dist = 2;
        int small_dist = 1;
        // If there is a free runner
        if (cur_runner_column == -1) {
            free_runner = true;
        }
        // If a runner is home
        else if (cur_runner_height == column_lengths[cur_runner_column]) {
            runner_home = true;
        }
        // If there is a runner very far ahead of the stop
        else if ((cur_runner_height - stops[cur_runner_column][current_player]) > big_dist) {
            runner_very_far_ahead_of_stop = true;
        }
        // Count the runners fairly far ahead of their stops
        else if ((cur_runner_height - stops[cur_runner_column][current_player]) > medium_dist) {
            runners_far_ahead_of_stop++;
        }
        // Count the runners less far from their stops
        else if ((cur_runner_height - stops[cur_runner_column][current_player]) > small_dist) {
            runners_just_ahead_of_stop++;
        }
    }
    // If there are no free runners, consider ending your turn
    if (!free_runner) {
        // End your turn under the following conditions:
            // A runner is home or very far ahead of its stop
            // Two runners are fairly far ahead of their stops
            // Three runners are somewhat far ahead of their stops
        if ((runner_home) || (runner_very_far_ahead_of_stop) || (runners_far_ahead_of_stop > 1) || (runners_just_ahead_of_stop > 2)) {
            game_board.end_turn();
            return game_board;
        }
    }
    // If you did not end your turn, go on (get roll and pairs)
    vector<int> roll = game_board.get_roll();
    vector<vector<int>> pairs = game_board.get_pairs(roll);
    // Pick best pairing
    int double_jump = -1;   // Index of pairing that moves one runner twice
    int two_jump = -1;      // Index of pairing that moves two runners
    int single_jump = -1;   // Index of pairing that moves one runner once
    for (int i = 0; i < 3; i++) {
        int cur_pair_0 = pairs[i][0];
        int cur_pair_1 = pairs[i][1];
        if (cur_pair_0 == cur_pair_1) {
            for (int j = 0; j < 3; j++) {
                if ((runner_cols[j] == -1) || (runner_cols[j] == cur_pair_0)) {
                    double_jump = i;
                    break;
                }
            }
        }
        else {
            bool moved_0 = false;
            bool moved_1 = false;
            for (int j = 0; j < 3; j++) {
                if (runner_cols[j] == cur_pair_0) {
                    moved_0 = true;
                }
                else if (runner_cols[j] == cur_pair_1) {
                    moved_1 = true;
                }
            }
            if ((moved_0 && moved_1) || (moved_0 && free_runner) || (moved_1 && free_runner)) {
                two_jump = i;
            }
            else if (moved_0 || moved_1 || free_runner) {
                single_jump = i;
            }
        }
    }
    // Choose double jump if possible
    if (double_jump != -1) {
        game_board.make_move(pairs[double_jump][0], pairs[double_jump][1]);
        return game_board;
    }
    // Otherwise, choose two_jump if possible
    else if (two_jump != -1) {
        game_board.make_move(pairs[two_jump][0], pairs[two_jump][1]);
        return game_board;
    }
    // Otherwise, choose single_jump if possible
    else if (single_jump != -1) {
        game_board.make_move(pairs[single_jump][0], pairs[single_jump][1]);
        return game_board;
    }
    // All pairing result in going bust so just pick the first one
    else {
        game_board.make_move(pairs[0][0], pairs[0][1]);
        return game_board;
    }
}