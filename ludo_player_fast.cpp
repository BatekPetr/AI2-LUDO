#include "ludo_player_fast.h"


ludo_player_fast::ludo_player_fast():ludo_player()
{
    // Change player type from parent class
    this->player_type = "Fast";
}

int ludo_player_fast::make_decision()
{
    /*
     * chooses highest priority move from
     * moves available returned by get_move_candidates()
     */

    std::vector<int> candidates = get_move_candidates();

    // if there is no possible move for the player
    // return -1 as relative_piece in PLAYER relative position ARRAY
    if ( candidates.empty() )
        return -1;  // skip all the rest of the function

    // sort candidate moves according to position of the pieces
    // further the piece, the higher priority
    std::map<int, int, std::greater<int>>  move_priorities; //sorts key in decreasing order
    for(size_t c = 0; c < candidates.size(); ++c)
    {
        // candidates[c] is relative_piece in PLAYER relative position ARRAY
        // pos_start_of_turn[it->second] is RELATIVE index of the piece on the BOARD
        move_priorities[pos_start_of_turn[candidates[c]]] = candidates[c];
    }

    // iterate over the priority map and return move with highest priority
    for (std::map<int,int, std::greater<int>>::iterator it=move_priorities.begin();\
         it!=move_priorities.end(); ++it)
    {
        int new_relative_position = pos_start_of_turn[it->second] + dice_roll;

        // We want to eliminate self kill by moving onto occupied
        // globe possition
        if (isGlobe(new_relative_position) &&\
            isOccupied(new_relative_position, pos_start_of_turn) )
        {
            continue;
        }
        // the furthest piece is in the goal stretch and
        // dice_roll is larger than remaining number of fields to goal
        else if (dice_roll_tooMuch(new_relative_position))
            continue;

        // return 1st highest priority piece which SATISFIES PREVIOUS condition
        // relative_piece in PLAYER relative position ARRAY
        return it->second;
    }

    // if there is no other option, play the piece with dice_roll over the number of remaining squares
    // relative_piece in PLAYER relative position ARRAY
    return move_priorities.begin()->second;

}
