#include <iostream>
#include <vector>
#include "simple.h"
#include "board.h"
#include "flat_mcts.h"
using namespace std; 

int choose_pairs(Board game_board, vector<vector<int>> pairs) {
    // Find which pair results in most wins
    vector<int> wins;
    // Try each pair
    for (int i = 0; i < 3; i++) {
        int ith_pair_wins = 0;
        // Try each pair three times
        for (int j = 0; j < 3; j++) {
            // Make a clean copy of the board to play out
            Board clean_copy;
            clean_copy.init(game_board.dice_size);
            clean_copy.clone(game_board);
            // Make the ith pair move
            clean_copy.make_move(pairs[i][0], pairs[i][1]);
            // Playout the game
            while (clean_copy.game_over() == -1) {
                clean_copy = make_smart_move(clean_copy);
            }
            // Check if you win
            if (clean_copy.game_over() == game_board.turn) {
                ith_pair_wins++;
            }
        }
        // Store the win numbers for this pair
        wins.push_back(ith_pair_wins);
    }
    // Get best move index
    int max_wins = 0;
    int max_wins_index = 0;
    for (int i = 0; i < 3; i++) {
        if (wins[i] > max_wins) {
            max_wins = wins[i];
            max_wins_index = i;
        }
    }
    return max_wins_index;
}

int stop_or_roll(Board copy_game_board) {
    // Check if stopping or rolling leads to more wins
    int stop_wins = 0;
    int roll_wins = 0;
    // Try stopping 15 times
    for (int i = 0; i < 15; i++) {
        // Make a clean board copy for the playout
        Board clean_copy;
        clean_copy.init(copy_game_board.dice_size);
        clean_copy.clone(copy_game_board);
        // End the turn
        clean_copy.end_turn();
        // Playout the game
        while (clean_copy.game_over() == -1) {
            clean_copy = make_smart_move(clean_copy);
        }
        // Check if you won
        if (clean_copy.game_over() == copy_game_board.turn) {
            stop_wins++;
        }
    }
    // Try rolling 15 times
    for (int i = 0; i < 15; i++) {
        // Make a clean board copy for the playout
        Board clean_copy;
        clean_copy.init(copy_game_board.dice_size);
        clean_copy.clone(copy_game_board);
        // Get one set of possible moves (given a random roll)
        vector<int> roll = clean_copy.get_roll();
        vector<vector<int>> pairs = clean_copy.get_pairs(roll);
        // Find the best move given this roll
        int best_move = choose_pairs(clean_copy, pairs);
        // Make the move
        clean_copy.make_move(pairs[best_move][0], pairs[best_move][1]);
        // Playout the game
        while (clean_copy.game_over() == -1) {
            clean_copy = make_smart_move(clean_copy);
        }
        // Check if you won
        if (clean_copy.game_over() == copy_game_board.turn) {
            roll_wins++;
        }
    }
    // Return the optimal move
    if (stop_wins > roll_wins) {
        return 0;
    }
    return 1;
}

Board make_mcts_move(Board game_board) {
    // Make a copy of the board 
    Board board_copy;
    board_copy.init(game_board.dice_size);
    board_copy.clone(game_board);
    // Check if stopping or rolling is better
    int keep_going = stop_or_roll(board_copy);
    // If stopping seems better, end the turn
    if (keep_going == 0) {
        game_board.end_turn();
        return game_board;
    }
    // If rolling seems better, roll
    else {
        // Get one set of possible moves (given a random roll)
        vector<int> roll = game_board.get_roll();
        vector<vector<int>> pairs = game_board.get_pairs(roll);
        // Find the best move given this roll
        int best_move = choose_pairs(game_board, pairs);
        game_board.make_move(pairs[best_move][0], pairs[best_move][1]);
        return game_board;
    }
}