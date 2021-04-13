#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include "board.h"
#include "random.h"
#include "simple.h"
#include "flat_mcts.h"
#include "q_learn.h"
#include "lstm.h"
using namespace std; 

Q_agent q_learn_agent_0;
Q_agent q_learn_agent_1;

Board q_agent0_move(Board game_board) {
    return q_learn_agent_0.make_move(game_board);
}

Board q_agent1_move(Board game_board) {
    return q_learn_agent_1.make_move(game_board);
}

void q_testing(char** argv) {
    int q_train_time = 0;
    string player0_name;
    string player1_name;
    int dice_size = stoi(argv[1]);
    int num_sims = stoi(argv[4]);
    function<Board(Board)> make_player0_move;
    function<Board(Board)> make_player1_move;

    for (int timer_update = 1; timer_update < 10; timer_update++) {
        q_train_time = timer_update * 2;
        std::cout << q_train_time << " SECONDS OF TRAINING\n";
        double avg_win_perc0 = 0.0;
        double avg_win_perc1 = 0.0;
        double low_p1 = 100.0;
        double high_p1 = 0.0;
        int below_90_sims = 0;
        for (int test_count = 0; test_count < 20; test_count++) {
            std::cout << "    Running test #" << test_count+1 << "\n";
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
                q_learn_agent_0.init("");
                q_learn_agent_0.train(q_train_time, dice_size);
                
                make_player0_move = q_agent0_move;
            }
            else if ((strcmp(argv[2], "q_learn_init") == 0) or (strcmp(argv[2], "-q1") == 0)) {
                player0_name = "Q-Learn with Init Agent";
                q_learn_agent_0.init("3_side_start_data.txt");
                q_learn_agent_0.train(q_train_time, dice_size);
                
                make_player0_move = q_agent0_move;
            }
            else {
                std::cout << "Invalid agent for player 0\n";
                return;
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
                q_learn_agent_1.init("");
                q_learn_agent_1.train(q_train_time, dice_size);
                
                make_player1_move = q_agent1_move;
            }
            else if ((strcmp(argv[3], "q_learn_init") == 0) or (strcmp(argv[3], "-q1") == 0)) {
                player1_name = "Q-Learn with Init Agent";
                q_learn_agent_1.init("3_side_start_data.txt");
                q_learn_agent_1.train(q_train_time, dice_size);
                
                make_player1_move = q_agent1_move;
            }
            else {
                std::cout << "Invalid agent for player 1\n";
                return;
            }

            
            Board game_board;
            game_board.init(dice_size);
            int p0_wins = 0;
            int p1_wins = 0;
            for (int i = 0; i < num_sims; i++) {
                if (i % 500 == 0) {
                    std::cout << "        Simulating game #" << i << "...\n";
                }
                int start_turn = i % 2;
                while (game_board.game_over() < 0) {
                    if (game_board.turn == start_turn) {
                        game_board = make_player0_move(game_board);
                    }
                    else {
                        game_board = make_player1_move(game_board);
                    }
                }
                int winner = game_board.game_over();
                if (winner == start_turn) {
                    p0_wins++;
                }
                else {
                    p1_wins++;
                }
                game_board.reset_board();
            }
            double p0_win_percentage =  double(p0_wins)/double(num_sims)*100.0;
            avg_win_perc0 += p0_win_percentage;
            double p1_win_percentage =  double(p1_wins)/double(num_sims)*100.0;
            avg_win_perc1 += p1_win_percentage;
            if (p1_win_percentage > high_p1) {
                high_p1 = p1_win_percentage;
            }
            if (p1_win_percentage < low_p1) {
                low_p1 = p1_win_percentage;
            }
            if (p1_win_percentage < 90) {
                below_90_sims++;
            }
            std::cout << "\n";
            std::cout << "        " << player0_name << " won " << p0_win_percentage << "% of games\n";
            std::cout << "        " << player1_name << " won " << p1_win_percentage << "% of games\n";
            std::cout << "\n";
        }
        avg_win_perc0 = avg_win_perc0 / 20;
        avg_win_perc1 = avg_win_perc1 / 20;
        std::cout << "\n\n";
        std::cout << "    WITH " << q_train_time << " SECONDS OF TRAINING:\n";
        std::cout << "        " << player0_name << " won an average of " << avg_win_perc0 << "% of games\n";
        std::cout << "        " << player1_name << " won an average of " << avg_win_perc1 << "% of games\n\n";
        std::cout << "        " << player1_name << " low = " << low_p1 << "\n";
        std::cout << "        " << player1_name << " high = " << high_p1 << "\n";
        std::cout << "        " << player1_name << " had " << below_90_sims << " training sessions where it did not win 90% of games\n\n\n";
    }
    return;
}

int main(int argc, char** argv) {

    // srand(time(0));
    // q_testing(argv);
    // return 1;
    int q_train_time = 10;
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
        
        make_player0_move = q_agent0_move;
    }
    else if ((strcmp(argv[2], "q_learn_init") == 0) or (strcmp(argv[2], "-q1") == 0)) {
        player0_name = "Q-Learn with Init Agent";
        std::cout << "Player 0: Q-Learning with Initial Values\n";
        q_learn_agent_0.init("3_side_start_data.txt");
        q_learn_agent_0.train(q_train_time, dice_size);
        
        make_player0_move = q_agent0_move;
    }
    else if ((strcmp(argv[2], "lstm") == 0) or (strcmp(argv[2], "-l") == 0)) {
        player0_name = "LSTM Agent";

        // Train Roll Choice
        // std::cout << "Training Roll Choice Network\n";
        // string command = "python3 roll_net.py";
        // system(command.c_str());
        // Train Stop or Roll
        // std::cout << "Training Stop or Roll Choice Network\n";
        
        make_player0_move = make_lstm_move;
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
        
        make_player1_move = q_agent1_move;
    }
    else if ((strcmp(argv[3], "q_learn_init") == 0) or (strcmp(argv[3], "-q1") == 0)) {
        player1_name = "Q-Learn with Init Agent";
        std::cout << "Player 1: Q-Learning with initial values\n";
        q_learn_agent_1.init("3_side_start_data.txt");
        q_learn_agent_1.train(q_train_time, dice_size);
        
        make_player1_move = q_agent1_move;
    }
    else if ((strcmp(argv[3], "lstm") == 0) or (strcmp(argv[3], "-l") == 0)) {
        player1_name = "LSTM Agent";

        // Train Roll Choice
        // std::cout << "Training Roll Choice Network\n";
        // string command = "python3 roll_net.py";
        // system(command.c_str());
        // // Train Stop or Roll
        // std::cout << "Training Stop or Roll Choice Network\n";
        
        make_player1_move = make_lstm_move;
    }
    else {
        std::cout << "Invalid agent for player 1\n";
        return -1;
    }

    
    Board game_board;
    game_board.init(dice_size);
    int num_sims = stoi(argv[4]);
    int p0_wins = 0;
    int p1_wins = 0;
    for (int i = 0; i < num_sims; i++) {
        if (i % 1 == 0) {
            std::cout << "Simulating game #" << i << "...\n";
        }
        int start_turn = i % 2;
        int move_num_0 = 0;
        int move_num_1 = 0;
        while (game_board.game_over() < 0) {
            if (game_board.turn == start_turn) {
                move_num_0++;
                std::cout << "    P0 making move #" << move_num_0 << "\n";
                game_board = make_player0_move(game_board);
            }
            else {
                move_num_1++;
                std::cout << "    P1 making move #" << move_num_1 << "\n";
                game_board = make_player1_move(game_board);
            }
            // std::cout << "END LOOP\n";
        }
        int winner = game_board.game_over();
        if (winner == start_turn) {
            std::cout << "P0 wins\n";
            p0_wins++;
        }
        else {
            p1_wins++;
            std::cout << "P1 wins\n";
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