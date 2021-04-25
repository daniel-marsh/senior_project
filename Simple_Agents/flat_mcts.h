/* 
Author: Daniel Marsh
Project: Yale CPSC 490 Senior Project
Description: Header file for the flat MCTS agent.
*/
#include <iostream>
#include <vector>
#include "../board.h"
using namespace std; 

int choose_pairs(Board game_board, vector<vector<int>> pairs);
int stop_or_roll(Board copy_game_board);
Board make_mcts_move(Board game_board);