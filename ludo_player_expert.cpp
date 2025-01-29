#include "ludo_player_expert.h"

ludo_player_expert::ludo_player_expert():ludo_player(),
  rd(),
  gen(rd())
{
    // Change player type from parent class
    this->player_type = "expert";
}

int ludo_player_expert::make_decision()
{

    std::vector<int> candidates = get_move_candidates\
                                    (pos_start_of_turn, dice_roll);

    // if there is no possible move for the player
    // return -1 as relative_piece in PLAYER relative position ARRAY
    if ( candidates.empty() )
        return -1;  // skip all the rest of the function


    // sort candidate moves according to reward of moves
    std::map<double, std::vector<int>, std::greater<int>>  move_priorities; //sorts key in decreasing order

    for (auto &c : candidates)
    {
        auto new_relative_positions = move_piece(pos_start_of_turn, c, dice_roll);
        double actual_reward = reward(new_relative_positions, c);

        move_priorities[actual_reward].push_back(c);
    }

    return pick_random_move(move_priorities.begin()->second);

}

double ludo_player_expert::reward(std::vector<int>& new_state,\
                              int piece_moved)
{
    double reward = 0;

    // Evaluate own player state
    bool win = 1;
    int pieces_in_goal = 0;
    int pieces_in_jail = 0;
    int new_pieces_in_goal = 0;
    double new_piece_from_jail = 0;
    double pieces_save = 0;
    int total_vulnerability = 0;
    double progress_in_play = 0;
    int star_bonus = 0;
    double move_in_stretch = 0;

    for (int i = 0; i < 4; ++i)
    {
        // Evaluate pieces in the jail
        if (new_state[i] == -1)
        {
            ++pieces_in_jail;
            win = 0;
            continue;
        }
        // Evaluate pieces at the Goal
        else if(new_state[i] == 99)
        {
            //reward for pieces already at goal
            ++pieces_in_goal;

            if (i == piece_moved)
            {    //piece moved directly on goal from main circle
                if (pos_start_of_turn[i] > 52)
                {   // put more weight for skipping Goal stretch
                    new_pieces_in_goal += 3;
                }
                else
                {   //new piece in the goal
                    ++new_pieces_in_goal;
                }
            }
            continue;
        }
        else
        {
            // any of the pieces is not at the GOAL
            // -> no win
            win = 0;
        }

        // --------------------------------------------------
        // Evaluate pieces in save places
        if( isOwnGlobe(new_state[i]) )
        {
            pieces_save += 0.5;
        }
        else if( isOwnBlockade(new_state[i], new_state) )
        {
            pieces_save += 0.5;
        }
        else
        {
            // Evaluate vulnerability
            // Further the piece is, the more vulnerable
            total_vulnerability += ( 1 + new_state[i]/100.0) *\
                                    vulnerableBy(new_state[i],\
                                            new_state);
        }

        if (new_state[i] < 51)
            progress_in_play += new_state[i]/50.0;

        // --------------------------------------------------
        // Evaluate moved piece
        if (i == piece_moved)
        {
            if( (new_state[i] == 0) &&\
                     (pos_start_of_turn[i] == -1) )
            {
                //new piece released from jail
                new_piece_from_jail += 0.5;
            }

            //Check whether piece was moved by star
            else if (isStar(new_state[piece_moved]))
                star_bonus += 1;

            // Evaluate piece entering GOAL stretch
            else if( (new_state[i] > 51) )
            {    if( (pos_start_of_turn[i] < 51) )
                {
                    //new piece save in a goal stretch
                    pieces_save += 1;
                }
                else
                {
                    move_in_stretch += 1;
                }
            }


        }
    }

    // Evaluate other players state
    double damage_to_opponent = 0;
    int opponents_in_goal_stretch = 0;

    for (size_t i = 4; i < new_state.size(); ++i)
    {
        if ( (new_state[i] == -1) &&\
             (pos_start_of_turn[i] != -1) )
        {

            damage_to_opponent += 1;
        }
        // opponents in the goal_stretch
        else if (new_state[i] >= 52)
        {
            ++opponents_in_goal_stretch;
        }
    }

    reward += pieces_in_goal + new_pieces_in_goal\
            - pieces_in_jail + new_piece_from_jail\
            + pieces_save - total_vulnerability\
            + damage_to_opponent + star_bonus\
            + progress_in_play - move_in_stretch + win*10\
            - opponents_in_goal_stretch*3;

    return reward;

}

int ludo_player_expert::pick_random_move(const std::vector<int>& possible_moves)
{
    std::uniform_int_distribution<> piece\
                                    (0, possible_moves.size()-1);
    int select = piece(gen);
    return possible_moves[select];
}
