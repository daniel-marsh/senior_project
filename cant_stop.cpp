/* 
Author: Daniel Marsh
Project: Yale CPSC 490 Senior Project
Description: This is the core program for calling the Can't Stop simulations. 
                The program takes 4 command line arguments: dice_size player_0 player_1 number_of_simulations
                The program outputs the win percentage of each player.
*/
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include "board.h"
#include "Simple_Agents/random.h"
#include "Simple_Agents/simple.h"
#include "Simple_Agents/flat_mcts.h"
#include "Q_Learning/q_learn.h"
#include "LSTM/lstm.h"
#include "Autoencoder/autoencoder.h"
using namespace std; 

// Additional function calls for the Q-Learning agent since the agent stores some information (Q-values).
Q_agent q_learn_agent_0;
Q_agent q_learn_agent_1;
Board q_agent0_move(Board game_board) {
    return q_learn_agent_0.make_move(game_board);
}
Board q_agent1_move(Board game_board) {
    return q_learn_agent_1.make_move(game_board);
}

// Test code for the Q-Learning agent
void q_testing(char** argv) {
    // Set up the initial variables
    int q_train_time = 0;
    string player0_name;
    string player1_name;
    int dice_size = stoi(argv[1]);
    int num_sims = stoi(argv[4]);
    function<Board(Board)> make_player0_move;
    function<Board(Board)> make_player1_move;
    // Outer loop is used to test different training times
    for (int timer_update = 1; timer_update < 10; timer_update++) {
        q_train_time = timer_update * 2;
        std::cout << q_train_time << " SECONDS OF TRAINING\n";
        // Keep track of certain win statistics
        double avg_win_perc0 = 0.0;
        double avg_win_perc1 = 0.0;
        double low_p1 = 100.0;
        double high_p1 = 0.0;
        int below_90_sims = 0;
        // Train the agent 20 different times at this training length
        for (int test_count = 0; test_count < 20; test_count++) {
            std::cout << "    Running test #" << test_count+1 << "\n";
            // Get player 0
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
                q_learn_agent_0.init("data/3_side_start_data.txt");
                q_learn_agent_0.train(q_train_time, dice_size);
                
                make_player0_move = q_agent0_move;
            }
            else if ((strcmp(argv[2], "lstm") == 0) or (strcmp(argv[2], "-l") == 0)) {
                player0_name = "LSTM Agent";
                make_player0_move = make_lstm_move;
            }
            else if ((strcmp(argv[2], "autoencoder") == 0) or (strcmp(argv[2], "-a") == 0)) {
                player0_name = "Autoencoder Agent";
                make_player0_move = make_autoencoder_move;
            }
            else {
                std::cout << "Invalid agent for player 0\n";
                return;
            }
            // Get player 1
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
                q_learn_agent_1.init("data/3_side_start_data.txt");
                q_learn_agent_1.train(q_train_time, dice_size);
                
                make_player1_move = q_agent1_move;
            }
            else if ((strcmp(argv[3], "lstm") == 0) or (strcmp(argv[3], "-l") == 0)) {
                player1_name = "LSTM Agent";
                make_player1_move = make_lstm_move;
            }
            else if ((strcmp(argv[3], "autoencoder") == 0) or (strcmp(argv[3], "-a") == 0)) {
                player1_name = "Autoencoder Agent";
                make_player1_move = make_autoencoder_move;
            }
            else {
                std::cout << "Invalid agent for player 1\n";
                return;
            }

            // Run the simulations (num_sims simulations for each of the 20 agents trained at this length)
            Board game_board;
            game_board.init(dice_size);
            int p0_wins = 0;
            int p1_wins = 0;
            for (int i = 0; i < num_sims; i++) {
                if (i % 500 == 0) {
                    std::cout << "        Simulating game #" << i << "...\n";
                }
                // Alternate turns
                int start_turn = i % 2;
                // While the game is not over, call an agent to make a move
                while (game_board.game_over() < 0) {
                    if (game_board.turn == start_turn) {
                        game_board = make_player0_move(game_board);
                    }
                    else {
                        game_board = make_player1_move(game_board);
                    }
                }
                // Check who won and keep track of win counts
                int winner = game_board.game_over();
                if (winner == start_turn) {
                    p0_wins++;
                }
                else {
                    p1_wins++;
                }
                // Reset board
                game_board.reset_board();
            }
            // Calculate statistics
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
            // Print some stats for this individual agent
            std::cout << "\n";
            std::cout << "        " << player0_name << " won " << p0_win_percentage << "% of games\n";
            std::cout << "        " << player1_name << " won " << p1_win_percentage << "% of games\n";
            std::cout << "\n";
        }
        // Print average stats over all agents at this training length
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

// Function to run the Can't Stop simulations
int main(int argc, char** argv) {
    // Set up initial variables
    srand(time(0));
    int q_train_time = 20;
    string player0_name;
    string player1_name;
    function<Board(Board)> make_player0_move;
    function<Board(Board)> make_player1_move;
    // If the command line arguments are invalid, return an error message
    if (argc != 5) {
        std::cout << "Incorrect number of arguments.\nUsage: ./cant_stop [dice_size] [player0_agent] [player1_agent] [num_sims]\n";
        return 0;
    }
    // Get the dice size from the command line
    int dice_size = stoi(argv[1]);
    // Get the agent to use for player 0
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
        q_learn_agent_0.init("data/3_side_start_data.txt");
        q_learn_agent_0.train(q_train_time, dice_size);
        
        make_player0_move = q_agent0_move;
    }
    else if ((strcmp(argv[2], "lstm") == 0) or (strcmp(argv[2], "-l") == 0)) {
        player0_name = "LSTM Agent";
        make_player0_move = make_lstm_move;
    }
    else if ((strcmp(argv[2], "autoencoder") == 0) or (strcmp(argv[2], "-a") == 0)) {
        player0_name = "Autoencoder Agent";
        make_player0_move = make_autoencoder_move;
    }
    // If the argument is not a valid agent name, return an error
    else {
        std::cout << "Invalid agent for player 0\n";
        return -1;
    }
    // Get the agent to use for player 1
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
        q_learn_agent_1.init("data/3_side_start_data.txt");
        q_learn_agent_1.train(q_train_time, dice_size);
        
        make_player1_move = q_agent1_move;
    }
    else if ((strcmp(argv[3], "lstm") == 0) or (strcmp(argv[3], "-l") == 0)) {
        player1_name = "LSTM Agent";   
        make_player1_move = make_lstm_move;
    }
    else if ((strcmp(argv[3], "autoencoder") == 0) or (strcmp(argv[3], "-a") == 0)) {
        player1_name = "Autoencoder Agent";
        make_player1_move = make_autoencoder_move;
    }
    // Again, if the argument given is not a valid agent, return an error 
    else {
        std::cout << "Invalid agent for player 1\n";
        return -1;
    }
    // Get the number of simulations to run from the command line
    int num_sims = stoi(argv[4]);
    // Declare and initialize the game board
    Board game_board;
    game_board.init(dice_size);
    // Simulate the games
    int p0_wins = 0;
    int p1_wins = 0;
    for (int i = 0; i < num_sims; i++) {
        // Print every so often to show that program is running smoothly
        if (i % 1 == 0) {
            std::cout << "Simulating game #" << i << "...\n";
        }
        // Alternate which agent starts to elimate first move advantage
        int start_turn = i % 2;
        int move_num_0 = 0;
        int move_num_1 = 0;
        // While the game is not over, call an agent to make a move
        while (game_board.game_over() < 0) {
            if (game_board.turn == start_turn) {
                move_num_0++;
                game_board = make_player0_move(game_board);
            }
            else {
                move_num_1++;
                game_board = make_player1_move(game_board);
            }
        }
        // Figure out who won
        int winner = game_board.game_over();
        // Keep track of win counts
        if (winner == start_turn) {
            std::cout << "P0 wins\n";
            p0_wins++;
        }
        else {
            p1_wins++;
            std::cout << "P1 wins\n";
        }
        // Reset the board and loop again
        game_board.reset_board();
    }
    
    // Output the player win %s
    std::cout << player0_name << " won " << double(p0_wins)/double(num_sims)*100.0 << "% of games\n";
    std::cout << player1_name << " won " << double(p1_wins)/double(num_sims)*100.0 << "% of games\n";
    return 0;
}