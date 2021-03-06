#include <iostream>
#include <vector>
#include <string>
#include "board.h"
#include "random.h"
#include "simple.h"
#include "flat_mcts.h"
using namespace std; 

int main(int argc, char** argv) {

    if (argc != 4) {
        std::cout << "Incorrect number of arguments.\nUsage: ./cant_stop [dice_size] [player0_agent] [player1_agent]\n";
        return 0;
    }
    int dice_size = stoi(argv[1]);

    function<Board(Board)> make_player0_move;
    function<Board(Board)> make_player1_move;

    if ((strcmp(argv[2], "random") == 0) or (strcmp(argv[2], "-r") == 0)) {
        make_player0_move = make_random_move;
    }
    else if ((strcmp(argv[2], "simple") == 0) or (strcmp(argv[2], "-s") == 0)) {
        make_player0_move = make_smart_move;
    }
    else if ((strcmp(argv[2], "mcts") == 0) or (strcmp(argv[2], "-m") == 0)) {
        make_player0_move = make_mcts_move;
    }

    if ((strcmp(argv[3], "random") == 0) or (strcmp(argv[3], "-r") == 0)) {
        make_player1_move = make_random_move;
    }
    else if ((strcmp(argv[3], "simple") == 0) or (strcmp(argv[3], "-s") == 0)) {
        make_player1_move = make_smart_move;
    }
    else if ((strcmp(argv[3], "mcts") == 0) or (strcmp(argv[3], "-m") == 0)) {
        make_player1_move = make_mcts_move;
    }

    srand(time(0));
    Board game_board;
    game_board.init(dice_size);
    int count = 1;
    while (game_board.game_over() < 0) {
        if (count % 10 == 0) {
            std::cout << "Move Number " << count << "\n";
        }
        if (game_board.turn == 0) {
            game_board = make_player0_move(game_board);
        }
        else {
            game_board = make_player1_move(game_board);
        }
        count++;
    }
    int winner = game_board.game_over();
    // game_board = make_random_move(game_board);
    game_board.display_board();
    // game_board.print_state();
    std::cout << "\n\nPlayer " << winner << " wins!\n";
    return 0;
}