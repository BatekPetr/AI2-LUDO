#include "ludo_player_defensive.h"
#include <map>

ludo_player_defensive::ludo_player_defensive():ludo_player_random()
{
}

int ludo_player_defensive::make_decision()
{
    std::vector<int> candidates = get_move_candidates();

    // if there is no possible move for the player
    // return -1 as relative_piece in PLAYER relative position ARRAY
    if ( candidates.empty() )
        return -1;  // skip all the rest of the function

    int no_possibleAttackers = 0;

    // sort candidate moves according to vulnerability by
    // other opponents pieces
    // more possible attackers, the higher priority
    std::map<int, std::vector<int>, std::greater<int>>  move_priorities; //sorts key in decreasing order
    for(size_t c = 0; c < candidates.size(); ++c)
    {

        no_possibleAttackers = vulnerableBy(pos_start_of_turn[candidates[c]],\
                                            pos_start_of_turn);
        // candidates[c] is relative_piece in PLAYER relative position ARRAY
        // no_possibleAttackers is No of oponents pieces
        // threatening candidate piece
        move_priorities[no_possibleAttackers].push_back(candidates[c]);
    }

    // moves with highest priority
    std::vector<int> highest_priority_moves = move_priorities.begin()->second;
    // randomly select move from the moves
    // with highest priority
    int selected_move = pick_random_move(highest_priority_moves);
    return selected_move;
}

