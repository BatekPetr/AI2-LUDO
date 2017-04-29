#include "ludo_player_defensive.h"
#include <map>

ludo_player_defensive::ludo_player_defensive():ludo_player_random()
{
    // Change player type from parent class
    this->player_type = "Defensive";
}

ludo_player_defensive::ludo_player_defensive(struct fann *value_ann):ludo_player_random(value_ann)
{
    // Change player type from parent class
    this->player_type = "Defensive";
}

ludo_player_defensive::ludo_player_defensive(struct fann *value_ann, std::string const &file)\
    :ludo_player_random(value_ann,file)
{
    // Change player type from parent class
    this->player_type = "Defensive";
}

int ludo_player_defensive::make_decision()
{
    std::vector<int> candidates = get_move_candidates\
                                    (pos_start_of_turn, dice_roll);

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


    if (move_priorities.begin()->first > 0)
    {
        // moves with highest priority
        std::vector<int> highest_priority_moves = move_priorities.begin()->second;
        // randomly select move from the moves
        // with highest priority
        int selected_move = pick_random_move(highest_priority_moves);
        return selected_move;
    }
    // no piece is vulnerable
    else
    {
        // try to filter out candidates by elliminating
        // self killing new positions
        std::vector<int> candidates2 = filter_out_candidates\
                                      (candidates, pos_start_of_turn);

        // select randomly from filtered candidates
        return pick_random_move(candidates2);
    }
}

