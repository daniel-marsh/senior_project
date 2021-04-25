/* 
Author: Daniel Marsh
Project: Yale CPSC 490 Senior Project
Description: Program to make move decisions based on some hard coded logic.
*/
#include <iostream>
#include <vector>
#include "../board.h"
#include "simple.h"
using namespace std; 

// Function to make the moce decision based on simple logic.
// Decisions are made based on runner availability and runner distance from stops
Board make_smart_move(Board game_board) {
    // Get info from board
    vector<vector<int>> runners = game_board.runner_positions;
    vector<int> column_lengths = game_board.len_columns;
    vector<vector<int>> stops = game_board.stop_positions;
    int current_player = game_board.turn;
    // Initialize variables used in logic
    vector<int> runner_cols;
    bool free_runner = false;
    bool runner_home =  false;
    bool runner_very_far_ahead_of_stop = false;
    int runners_far_ahead_of_stop = 0;
    int runners_just_ahead_of_stop = 0;
    // Values used to categorize runner distances
    int big_dist = 3;
    int medium_dist = 2;
    int small_dist = 1;
    // Check each runner and popultae variables
    for (int i = 0; i < 3; i++) {
        // Get data about the ith runner
        int cur_runner_column = runners[i][0];
        int cur_runner_height = runners[i][1];
        // Store the runner column
        runner_cols.push_back(cur_runner_column);
        // Check if the runner is free 
        if (cur_runner_column == -1) {
            free_runner = true;
        }
        // Check if a runner is at the top of its column
        else if (cur_runner_height == column_lengths[cur_runner_column]) {
            runner_home = true;
        }
        // Check if there is a runner very far ahead of the stop
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
    // Check each possible roll pairing to see if it falls into one of the three above categories
    for (int i = 0; i < 3; i++) {
        int cur_pair_0 = pairs[i][0];
        int cur_pair_1 = pairs[i][1];
        // If the two pairs in the ith pairing have the same value, check if there is runner in that column or a free runner
        if (cur_pair_0 == cur_pair_1) {
            // Check each runner
            for (int j = 0; j < 3; j++) {
                // If the jth runner is free or in the correct column, remeber this roll pairing and break
                if ((runner_cols[j] == -1) || (runner_cols[j] == cur_pair_0)) {
                    double_jump = i;
                    break;
                }
            }
        }
        // Otherwise, the two pairs do not have the same value
        else {
            bool moved_0 = false;
            bool moved_1 = false;
            // Check the three runners to see if one or both of the pairs from this roll pairing can be used
            for (int j = 0; j < 3; j++) {
                // Check the first pair
                if (runner_cols[j] == cur_pair_0) {
                    moved_0 = true;
                }
                // Check the second pair
                else if (runner_cols[j] == cur_pair_1) {
                    moved_1 = true;
                }
            }
            // Check if both pairs can be used
            if ((moved_0 && moved_1) || (moved_0 && free_runner) || (moved_1 && free_runner)) {
                two_jump = i;
            }
            // Check if either pair can be used
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
