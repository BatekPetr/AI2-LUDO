#include "ludo_player_aggressive.h"

ludo_player_aggressive::ludo_player_aggressive():ludo_player_random()
{
    // Change player type from parent class
    this->player_type = "Aggressive";
}

ludo_player_aggressive::ludo_player_aggressive(struct fann *value_ann):ludo_player_random(value_ann)
{
    // Change player type from parent class
    this->player_type = "Aggresive";
}

int ludo_player_aggressive::make_decision()
{
    /*
     * chooses highest priority move from
     * moves available returned by get_move_candidates()
     */

    std::vector<int> candidates = get_move_candidates\
                                    (pos_start_of_turn, dice_roll);

    // if there is no possible move for the player
    // return -1 as relative_piece in PLAYER relative position ARRAY
    if ( candidates.empty() )
        return -1;  // skip all the rest of the function

    // check whether it is possible to kill other player piece
    for(size_t c = 0; c < candidates.size(); ++c)
    {
        // eliminate piece standing on players init. field
        //when dice_roll == 6
        if ( pos_start_of_turn[candidates[c]] == -1)
        {
            if(isOccupied(0, pos_start_of_turn) == 1)
            {
                return candidates[c];
            }
            // no piece is standing on player init. field
            continue; //not really needed, just to make it explicit
        }
        else
        {
            int new_relative_position = pos_start_of_turn[candidates[c]]\
                                        + dice_roll;

            if(isOccupied(new_relative_position, pos_start_of_turn) == 1)
            {
                // We want to eliminate self kill by moving onto occupied
                // globe possition
                if ( !isGlobe(new_relative_position))
                    return candidates[c];
            }
        }
    }

    // no possibility to kill other player piece
    // try to filter out candidates by elliminating
    // self killing new positions
    std::vector<int> candidates2 = filter_out_candidates\
                                  (candidates, pos_start_of_turn);

    // select randomly from filtered candidates2
    return pick_random_move(candidates2);

}


