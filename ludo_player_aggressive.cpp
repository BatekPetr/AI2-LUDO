#include "ludo_player_aggressive.h"

ludo_player_aggressive::ludo_player_aggressive():ludo_player(),
    rd(),
    gen(rd())
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
            for (size_t i = 4; i < pos_start_of_turn.size(); ++i)
            {
                if ( 0 == pos_start_of_turn[i])
                    return candidates[c];
            }
            // no piece is standing on player init. field
            continue;
        }
        else
        {
            int new_relative_position = pos_start_of_turn[candidates[c]]\
                                        + dice_roll;

            for (size_t i = 4; i < pos_start_of_turn.size(); ++i)
            {
                if ( (-1 < pos_start_of_turn[i]) &&
                     (pos_start_of_turn[i] < 51) && // no killing in goal stretch
                     (pos_start_of_turn[i] == new_relative_position) )
                {
                    // We want to eliminate self kill by moving onto occupied
                    // globe possition
                    if (isGlobe(new_relative_position))
                        break;
                    else
                        return candidates[c];
                }
            }
        }
    }

    // no possibility to kill other player piece
    std::vector<int> candidates2;
    for(size_t c = 0; c < candidates.size(); ++c)
    {
        int new_relative_position = pos_start_of_turn[candidates[c]]\
                                    + dice_roll;
        // We want to eliminate self kill by moving onto occupied
        // globe possition
        if (isGlobe(new_relative_position))
        {
            bool occupied = false;
            for (size_t i = 4; i < pos_start_of_turn.size(); ++i)
            {
                // if new_relative_position is globe and is OCCUPIED by
                // other player -> do NOT consider this piece for the move
                if ( (-1 < pos_start_of_turn[i]) &&
                     //(pos_start_of_turn[i] < 99) && // probably not needed
                     (pos_start_of_turn[i] == new_relative_position) )
                {
                    occupied = true;
                    break;
                }
            }
            if (occupied)
                continue;
        }

        // the piece is in the goal stretch and
        // dice_roll is larger than remaining number of fields to goal
        if ( (pos_start_of_turn[candidates[c]] > 50) && \
             (pos_start_of_turn[candidates[c]] + dice_roll > 56) )
        {
            continue;
        }
        else
        {
            // Filter out first candidates which are in a goal stretch
            // and dice_roll is larger than number of remaining fields
            candidates2.push_back(candidates[c]);
        }
    }
    if ( !candidates2.empty() )
    {
        // select randomly from candidates2
        std::uniform_int_distribution<> piece(0, candidates2.size()-1);
        int select = piece(gen);
        return candidates2[select];
    }
    else
    {
        // select randomly from all candidates
        std::uniform_int_distribution<> piece(0, candidates.size()-1);
        int select = piece(gen);
        return candidates[select];
    }

}

bool ludo_player_aggressive::isGlobe(int index){
    if(index < 52){     //check only the indexes on the board, not in the home streak
        if(index % 13 == 0 || (index - 8) % 13 == 0 || isOccupied(index) > 1){  //if more people of the same team stand on the same spot it counts as globe
            return true;
        }
    }
    return false;
}

int ludo_player_aggressive::isOccupied(int index){ //returns number of people of another color
    int number_of_people = 0;

    if(index != 99){
        for(size_t i = 4; i < pos_start_of_turn.size(); ++i){ //Disregard own players
            if(pos_start_of_turn[i] == index)
            {
                ++number_of_people;
            }

        }
    }
    return number_of_people;
}

