#include <iostream>
#include <vector>
#include <string>
#include "board.h"
#include "random.h"
#include "simple.h"
#include "flat_mcts.h"
#include "q_learn.h"
using namespace std; 

Q_agent q_learn_agent_0;
Q_agent q_learn_agent_1;

Board q_agent_move(Board game_board) {
    if (game_board.turn == 0) {
       return q_learn_agent_0.make_move(game_board);
    }
    else {
        return q_learn_agent_1.make_move(game_board);
    }
}

int main(int argc, char** argv) {

    int q_train_time = 100;
    string player0_name;
    string player1_name;

    if (argc != 5) {
        std::cout << "Incorrect number of arguments.\nUsage: ./cant_stop [dice_size] [player0_agent] [player1_agent] [num_sims]\n";
        return 0;
    }
    int dice_size = stoi(argv[1]);
    function<Board(Board)> make_player0_move;
    function<Board(Board)> make_player1_move;

    if ((strcmp(argv[2], "random") == 0) or (strcmp(argv[2], "-r") == 0)) {
        player0_name = "Random Agent";
        make_player0_move = make_random_move;
    }
    else if ((strcmp(argv[2], "simple") == 0) or (strcmp(argv[2], "-s") == 0)) {
        player0_name = "Game Knowledge Agent";
        make_player0_move = make_smart_move;
    }
    else if ((strcmp(argv[2], "mcts") == 0) or (strcmp(argv[2], "-m") == 0)) {
        player0_name = "Simple MCTS Agent";
        make_player0_move = make_mcts_move;
    }
    else if ((strcmp(argv[2], "q_learn_scratch") == 0) or (strcmp(argv[2], "-q0") == 0)) {
        player0_name = "Q-Learn from scratch Agent";
        std::cout << "Player 0: Q-Learning from Scratch\n";
        q_learn_agent_0.init("");
        q_learn_agent_0.train(q_train_time, dice_size);
        
        make_player0_move = q_agent_move;
    }
    else if ((strcmp(argv[2], "q_learn_init") == 0) or (strcmp(argv[2], "-q1") == 0)) {
        player0_name = "Q-Learn with Init Agent";
        std::cout << "Player 0: Q-Learning with Initial Values\n";
        q_learn_agent_0.init("3_side_start_data.txt");
        q_learn_agent_0.train(q_train_time, dice_size);
        
        make_player0_move = q_agent_move;
    }
    else {
        std::cout << "Invalid agent for player 0\n";
        return -1;
    }

    if ((strcmp(argv[3], "random") == 0) or (strcmp(argv[3], "-r") == 0)) {
        player1_name = "Random Agent";
        make_player1_move = make_random_move;
    }
    else if ((strcmp(argv[3], "simple") == 0) or (strcmp(argv[3], "-s") == 0)) {
        player1_name = "Game Knowledge Agent";
        make_player1_move = make_smart_move;
    }
    else if ((strcmp(argv[3], "mcts") == 0) or (strcmp(argv[3], "-m") == 0)) {
        player1_name = "Simple MCTS Agent";
        make_player1_move = make_mcts_move;
    }
    else if ((strcmp(argv[3], "q_learn_scratch") == 0) or (strcmp(argv[3], "-q0") == 0)) {
        player1_name = "Q-Learn from scratch Agent";
        std::cout << "Player 1: Q-Learning from Scratch\n";
        q_learn_agent_1.init("");
        q_learn_agent_1.train(q_train_time, dice_size);
        
        make_player1_move = q_agent_move;
    }
    else if ((strcmp(argv[3], "q_learn_init") == 0) or (strcmp(argv[3], "-q1") == 0)) {
        player1_name = "Q-Learn with Init Agent";
        std::cout << "Player 1: Q-Learning with initial values\n";
        q_learn_agent_1.init("3_side_start_data.txt");
        q_learn_agent_1.train(q_train_time, dice_size);
        
        make_player1_move = q_agent_move;
    }
    else {
        std::cout << "Invalid agent for player 1\n";
        return -1;
    }

    srand(time(0));
    Board game_board;
    game_board.init(dice_size);
    int num_sims = stoi(argv[4]);
    int p0_wins = 0;
    int p1_wins = 0;
    for (int i = 0; i < num_sims; i++) {
        if (i % 500 == 0) {
            std::cout << "Simulating game #" << i << "...\n";
        }
        while (game_board.game_over() < 0) {
            if (game_board.turn == 0) {
                game_board = make_player0_move(game_board);
            }
            else {
                game_board = make_player1_move(game_board);
            }
        }
        int winner = game_board.game_over();
        if (winner == 0) {
            p0_wins++;
        }
        else {
            p1_wins++;
        }
        game_board.reset_board();
    }
    
    // game_board.display_board();
    // std::cout << "\n\nPlayer " << winner << " wins!\n";
    // std::cout << "The Q Learning agent won " << p1_wins << " games out of 10000 simulations\n";
    std::cout << player0_name << " won " << double(p0_wins)/double(num_sims)*100.0 << "% of games\n";
    std::cout << player1_name << " won " << double(p1_wins)/double(num_sims)*100.0 << "% of games\n";
    return 0;
}