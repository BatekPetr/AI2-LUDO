#include "ludo_player.h"
#include <random>

ludo_player::ludo_player():
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    dice_roll(0)
{
}

int ludo_player::make_decision(){
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
