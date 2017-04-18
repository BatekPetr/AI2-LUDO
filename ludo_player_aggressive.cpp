#include "ludo_player_aggressive.h"

ludo_player_aggressive::ludo_player_aggressive():ludo_player_random()
{
}

int ludo_player_aggressive::make_decision()
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

    if ( !candidates2.empty() )
    {
        // select randomly from filtered candidates2
        return pick_random_move(candidates2);
    }
    else
    {
        // select randomly from all candidates
        // no possibility to filter out available moves
        return pick_random_move(candidates);
    }

}

std::vector<int> ludo_player_aggressive::filter_out_candidates\
                (const std::vector<int>& candidates,\
                 const std::vector<int>& relative_player_positions)
{
    std::vector<int> filtered_candidates;

    for(size_t c = 0; c < candidates.size(); ++c)
    {
        int new_relative_position =\
                            relative_player_positions[candidates[c]]\
                            + dice_roll;
        // We want to eliminate self kill by moving onto occupied
        // globe possition
        if (isGlobe(new_relative_position) &&\
            isOccupied(new_relative_position, relative_player_positions) )
        {
            continue;
        }
        // the piece is in the goal stretch and
        // dice_roll is larger than remaining number of fields to goal
        else if ( dice_roll_tooMuch(new_relative_position) )
        {
            continue;
        }
        else
        {
            // Filter out first candidates which are in a goal stretch
            // and dice_roll is larger than number of remaining fields
            filtered_candidates.push_back(candidates[c]);
        }
    }
    return filtered_candidates;
}
