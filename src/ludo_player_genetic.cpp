/**
 * @file ludo_player_genetic.cpp
 * @brief This file contains implementation of a ludo_player_genetic
*/

#include "ludo_player_genetic.h"

ludo_player_genetic::ludo_player_genetic():ludo_player_random()
{
    // Change player type from parent class
    this->player_type = "Genetic";
}

ludo_player_genetic::ludo_player_genetic(std::vector<double> genes):ludo_player_random()
{
    this->player_type = "Genetic";
    this->genes = genes;
}

void ludo_player_genetic::say_hi()
{
    std::cout << this->player_type;

    std::cout << ": [ ";
    for (int i = 0; i < genes.size() - 1; ++i)
    {
        std::cout << this->genes[i] << ", ";
    }
    std::cout << this->genes[genes.size() - 1] << " ]";
}

int ludo_player_genetic::make_decision()
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

    // sort candidate moves according to their priority
    // priority is given by genes
    std::map<double, std::vector<int>, std::greater<double>>  move_priorities; //sorts key in decreasing order
    for(size_t c = 0; c < candidates.size(); ++c)
    {
        std::vector<int> move_type;

        // Number of possile attackers for given piece
        move_type.push_back(is_defensive(pos_start_of_turn, candidates[c], dice_roll));
        move_type.push_back(is_aggressive(pos_start_of_turn, candidates[c], dice_roll));
        move_type.push_back(is_fast(pos_start_of_turn, candidates[c], dice_roll));
        move_type.push_back(is_release_from_jail(pos_start_of_turn, candidates[c], dice_roll));
        move_type.push_back(is_move_in_goal_stretch(pos_start_of_turn, candidates[c], dice_roll));
        move_type.push_back(is_move_to_goal(pos_start_of_turn, candidates[c], dice_roll));
        move_type.push_back(is_self_kill(pos_start_of_turn, candidates[c], dice_roll));

        double move_score = get_move_score(move_type, genes);


        // candidates[c] is relative_piece in PLAYER relative position ARRAY
        move_priorities[move_score].push_back(candidates[c]);
    }

    // moves with highest priority
    std::vector<int> highest_priority_moves = move_priorities.begin()->second;
    // randomly select move from the moves
    // with highest priority
    int selected_move = pick_random_move(highest_priority_moves);
    return selected_move;

}

double ludo_player_genetic::get_move_score(const std::vector<int> &move_type, const std::vector<double> &genes)
{
    double score = 0;
    for (int i = 0; i < genes.size(); ++i)
    {
        score += genes[i] * move_type[i];
    }
    return score;
}

int ludo_player_genetic::is_defensive(const std::vector<int> &state, int piece_to_move, int dice_roll)
{
    /*
     * Return difference in Number of possible attackers befor piece move and after it
     * Doesn't take into account SELF KILL moves
     */

    int no_possibleAttackers_before_move = vulnerableBy(state[piece_to_move], state);

    auto new_relative_positions = move_piece(state, piece_to_move, dice_roll);

    int no_possibleAttackers_after_move = vulnerableBy(new_relative_positions[piece_to_move],\
                                                       new_relative_positions);

    return no_possibleAttackers_before_move - no_possibleAttackers_after_move;
}


int ludo_player_genetic::is_aggressive(const std::vector<int> &state, int piece_to_move, int dice_roll)
{
    /*
     * Return number of opponents pieces which can be knocked to jail
     * More opponents pieces can be knocked out if they are staying on player init field
     */

    // eliminate piece standing on players init. field
    //when dice_roll == 6
    if ( state[piece_to_move] == -1)
    {
        // Return number of pieces standing on the player init field
        return isOccupied(0, state);
    }
    else
    {
        int new_relative_position = state[piece_to_move]\
                                    + dice_roll;

        // Doesn't take into account that opponents is on the SAVE field ->
        // results in self killing
        // Use this version with the gene for self killing movements
        return isOccupied(new_relative_position, state);

        /*
        // Do the check if the move is SELF KILL
        // Get No. of opponents on the new field
        int no_of_pieces_on_new_position = isOccupied(new_relative_position, state);
        if(no_of_pieces_on_new_position == 1)
        {
            // We want to eliminate self kill by moving onto occupied
            // globe possition
            if ( !isGlobe(new_relative_position) )
                return 1;
            // Opponent's piece is standing on the globe
            else
                return -1;
        }
        // There is an opponent's blockade
        else if (no_of_pieces_on_new_position > 1)
            return -1;
        // No piece is standing on new possition
        else
            return 0;
        */
    }
}

int ludo_player_genetic::is_fast(const std::vector<int> &state, int piece_to_move, int dice_roll)
{
    for (int i = 0; i < 4; ++i)
    {
        // Check if the move is with the piece, which is furthest in the gameplay
        if (i != piece_to_move)
            // it is NOT the furthest piece
            if (state[piece_to_move] < state[i])
                return 0;
    }
    // It is move with the FURTHEST piece in the game
    return 1;
}

int ludo_player_genetic::is_release_from_jail(const std::vector<int> &state, int piece_to_move, int dice_roll)
{
    if ( (state[piece_to_move] == -1) && dice_roll == 6)
        return 1;
    else
        return 0;
}

int ludo_player_genetic::is_move_in_goal_stretch(const std::vector<int> &state, int piece_to_move, int dice_roll)
{
    // Piece is in the goal stretch at the start of turn
    // But dice roll doesn't result into coming to goal
    if ( (state[piece_to_move] > 51) && (state[piece_to_move] + dice_roll != 57) )
         return 1;
    else
         return 0;
}

int ludo_player_genetic::is_move_to_goal(const std::vector<int> &state, int piece_to_move, int dice_roll)
{
    if (state[piece_to_move] + dice_roll == 57)
        return 1;
    else
        return 0;
}

int ludo_player_genetic::is_self_kill(const std::vector<int> &state, int piece_to_move, int dice_roll)
{
    // Releasing piece from jail never end in self kill
    if ( (state[piece_to_move] == -1) && (dice_roll == 6) )
        return 0;

    // Other move than release from jail
    int new_relative_position = state[piece_to_move] + dice_roll;

    // We want to eliminate self kill by moving onto occupied
    // globe possition and blockades
    if ( isOccupied(new_relative_position, state) > 1)
        return 1;
    else if ( (isOccupied(new_relative_position, state) == 1 ) &&\
              (isGlobe(new_relative_position) ) )
    {
        return 1;
    }
    else
        return 0;
}
