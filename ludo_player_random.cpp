#include "ludo_player_random.h"


ludo_player_random::ludo_player_random():ludo_player(),
    rd(),
    gen(rd())
{
    // Change player type from parent class
    this->player_type = "Random";
}

ludo_player_random::ludo_player_random(value_ANN_small *value_ann, std::string const &file):\
    ludo_player(value_ann, file),
    rd(),
    gen(rd())
{
    // Change player type from parent class
    this->player_type = "Random";
}

ludo_player_random::ludo_player_random(value_ANN_small *value_ann):ludo_player(value_ann),
    rd(),
    gen(rd())
{
    // Change player type from parent class
    this->player_type = "Random";
}

int ludo_player_random::make_decision(){

    std::vector<int> candidates = get_move_candidates\
                                    (pos_start_of_turn, dice_roll);

    // if there is no possible move for the player
    // return -1 as relative_piece in PLAYER relative position ARRAY
    if ( candidates.empty() )
        return -1;  // skip all the rest of the function
    else
        return pick_random_move(candidates);

}

int ludo_player_random::pick_random_move(const std::vector<int>& possible_moves)
{
    std::uniform_int_distribution<> piece\
                                    (0, possible_moves.size()-1);
    int select = piece(gen);
    return possible_moves[select];
}


