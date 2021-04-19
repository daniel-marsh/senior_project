#include <iostream>
#include <vector>
#include "../board.h"
#include "random.h"
using namespace std; 

Board make_random_move(Board game_board) {
    int rand_choice = rand() % 3;
    if (rand_choice == 0) {
        game_board.end_turn();
        return game_board;
    }
    vector<int> roll = game_board.get_roll();
    vector<vector<int>> pairs = game_board.get_pairs(roll);
    rand_choice = rand() % 3;
    vector<int> chosen_pair = pairs[rand_choice];
    game_board.make_move(chosen_pair[0], chosen_pair[1]);
    return game_board;
}