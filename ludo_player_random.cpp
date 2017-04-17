#include "ludo_player_random.h"

ludo_player_random::ludo_player_random():ludo_player(),
    rd(),
    gen(rd())
{
}

int ludo_player_random::make_decision(){
    /*//print debug info
    print_player_piece_positions();
    print_dice_roll();
    // Wait for ENTER key between turns
    std::cout << "Press Enter to Continue";
    std::cin.ignore();
    */
    std::vector<int> valid_moves;
    // Pieces which can be released out of jail
    if(dice_roll == 6){
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i]<0){
                valid_moves.push_back(i);
            }
        }
    }
    // Pieces in the playing circle and home stretch
    for(int i = 0; i < 4; ++i){
        if(pos_start_of_turn[i]>=0 && pos_start_of_turn[i] != 99){
            valid_moves.push_back(i);
        }
    }
    // Pieces trapped in the jail
    if(valid_moves.size()==0){
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i] != 99){
                valid_moves.push_back(i);
            }
        }
    }
    std::uniform_int_distribution<> piece(0, valid_moves.size()-1);
    int select = piece(gen);
    return valid_moves[select];
}

