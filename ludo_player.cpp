#include "ludo_player.h"
#include <random>

ludo_player::ludo_player():
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    dice_roll(0)
{
}

int ludo_player::make_decision()
{
    /*
     * make_decision function general for all players
     * if child class implements this function, polymorphed function
     * from subclass is used instead
     * -> make_decision() is VIRTUAL function
     *
     * specialized players should implement this function in their
     * respective subclasses
     */

    // After rolling 6
    if(dice_roll == 6){
        for(int i = 0; i < 4; ++i){
            // Pieces which can be released out of jail
            // If there is piece in jail -> prefer this move
            if(pos_start_of_turn[i]<0){
                return i;
            }
        }

        // Pieces in the game
        // No Piece in jail -> play with some other one
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i]>=0 && pos_start_of_turn[i] != 99){
                return i;
            }
        }
    }
    // Other roll than 6
    else {
        for(int i = 0; i < 4; ++i){
            // Pieces in the game
            if(pos_start_of_turn[i]>=0 && pos_start_of_turn[i] != 99){
                return i;
            }
        }
        for(int i = 0; i < 4; ++i){ //maybe they are all locked in
            if(pos_start_of_turn[i]<0){
                return i;
            }
        }
    }
    return -1;
}

void ludo_player::start_turn(positions_and_dice relative){
    pos_start_of_turn = relative.pos;
    dice_roll = relative.dice;
    int decision = make_decision();
    emit select_piece(decision);
}

void ludo_player::post_game_analysis(std::vector<int> relative_pos){
    pos_end_of_turn = relative_pos;
    bool game_complete = true;
    for(int i = 0; i < 4; ++i){
        if(pos_end_of_turn[i] < 99){
            game_complete = false;
        }
    }
    emit turn_complete(game_complete);
}

std::vector<int> ludo_player::get_move_candidates()
{
    /*
     * function should return possible candidates
     * for the next player move
    */

    std::vector<int> valid_moves;

    for(int i = 0; i < 4; ++i){
        if(pos_start_of_turn[i]<0){
            // Pieces which can be released out of jail
            if(dice_roll == 6)
                // Take piece out of jail
                valid_moves.push_back(i);
        }
        // Pieces in the playing circle and home stretch
        else if(pos_start_of_turn[i]>=0 && pos_start_of_turn[i] != 99){
            valid_moves.push_back(i);
        }
    }

    return valid_moves;
}

void ludo_player::print_player_piece_positions()
{
    std::cout << "pos_start_of_turn: [ ";
    int i;
    for (i = 0; i < 3; i++)
    {
        std::cout << pos_start_of_turn[i] << ", ";
    }
    std::cout << pos_start_of_turn[i] << " ]"<< std::endl;
}

void ludo_player::print_dice_roll()
{
    std::cout << "dice_roll: " << dice_roll << std::endl;
}
