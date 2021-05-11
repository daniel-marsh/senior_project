/* 
Author: Daniel Marsh
Project: Yale CPSC 490 Senior Project
Description: Program to make move decisions based on random selection.
*/
#include <iostream>
#include <vector>
#include "../board.h"
#include "random.h"
using namespace std; 

// Function used to make move decisions.
Board make_random_move(Board game_board) {
    // Generate a random number between 0 and 2
    int rand_choice = rand() % 3;
    // Don't stop at the start of a new turn
    if (game_board.start_turn() == -1) {
        // Stop 1/3 of the time (if the number generated was 0)
        if (rand_choice == 0) {
            game_board.end_turn();
            return game_board;
        }
    }
    // Otherwise, roll the other 2/3 of the time
    // Get the roll and pairings
    vector<int> roll = game_board.get_roll();
    vector<vector<int>> pairs = game_board.get_pairs(roll);
    // Choose a random pairing
    rand_choice = rand() % 3;
    vector<int> chosen_pair = pairs[rand_choice];
    // Make move based on random selection
    game_board.make_move(chosen_pair[0], chosen_pair[1]);
    return game_board;
}
