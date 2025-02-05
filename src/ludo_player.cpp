/**
 * @file ludo_player.cpp
 * @brief This file contains implementation of some basic functions for ludo_player
 *
 * @todo Decide on and use only one naming convention
*/

#include "ludo_player.h"
#include <random>

ludo_player::ludo_player():
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    dice_roll(0),
    player_type("General")
{
}

void ludo_player::say_hi()
{
    std::cout << this->player_type;
}


int ludo_player::make_decision()
{
    /*
     * make_decision function general for all players
     * if child class implements this function, polymorphed function
     * from subclass is used instead
     * -> make_decision() is VIRTUAL function
     *
     * specialized players should implement this function in their
     * respective subclasses
     */

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
        for(int i = 0; i < 4; ++i)
        {
            // Pieces in the game
            if(pos_start_of_turn[i]>=0 && pos_start_of_turn[i] != 99)
            {
                return i;
            }
        }
    }
    // No available move, all pieces locked in
    return -1;
}

int ludo_player::start_turn(positions_and_dice relative){
    pos_start_of_turn = relative.pos;
    dice_roll = relative.dice;
    int decision = make_decision();
    //std::vector<int> new_relative_positions = move_piece(decision);
    //emit select_piece(decision);

    return decision;
}

bool ludo_player::post_game_analysis(std::vector<int> relative_pos){
    pos_end_of_turn = relative_pos;
    bool game_complete = true;
    for(int i = 0; i < 4; ++i){
        if(pos_end_of_turn[i] < 99){
            game_complete = false;
        }
    }
    //emit turn_complete(game_complete);

    return game_complete;
}

/*
 * moves ONE piece, selected by player
 * works with player RELATIVE indexes
 */
std::vector<int> ludo_player::move_piece(const std::vector<int>& state,\
                                         int relative_piece, int action_dice_roll)
{
    // Check if there is a piece to move
    // -1 means, there is not any piece with possibility to move
    if (relative_piece == -1)
    {
        // Nothing can be done
        // game state after turn is the same as the one before
        return state;
    }

    // create new game state with relative possitions
    std::vector<int> new_relative_positions = state;

    // Get RELATIVE position of a particular piece
    int relative_pos = state[relative_piece];
    int target_pos = 0;

    if(relative_pos == -1)
    { //if the selected piece is in the safe house, try to move it to start
        move_start(relative_piece, state, new_relative_positions);
    }
    else
    {
        if(relative_pos == 99)
        {
            // selected relative piece is already at the goal
            std::cout << "I tought this would be it ";
        }

        // Update relative index by score from dice roll
        // Check for entering goal stretch
        if ( (relative_pos < 51) && (relative_pos + action_dice_roll) > 50 )
        {
            // this needs to be done when entering goal stretch
            // goal stretch starts at 52 while main game circle
            // ends at 50 for relative indexing
            relative_pos += action_dice_roll + 1;
        }
        else
        {
            //this is relative position of the selected token + the dice number
            relative_pos += action_dice_roll;
        }

        // Check whether piece is standing on the star (after dice update) and returns no. of fields to the next star when it is
        int jump = isStar(relative_pos); //return 0 | 6 | 7
        if(jump)
        {
            // Just in case, when piece stands on the star in front of its color goal stretch
            if(jump + relative_pos == 57){
                relative_pos = 57;
            }
            // Any other star just moves piece forward
            else {
                relative_pos += jump;
            }
        }

        // target_pos is in RELATIVE indexing

        // ########## special case checks ################################
        // This condition happens when player tries to move a piece in its goal stretch
        if(relative_pos > 57 && relative_pos < 73){ // go back
            //If the player moves over the goal, it should move backwards
            // target_pos is in absolute/fixed indexes,
            // hence adding color * 5
            target_pos = 57-(relative_pos-57);
        }
        // Move piece to GOAL
        else if(relative_pos == 57 || relative_pos >= 99)
        {
            target_pos = 99;
        }
        else // move piece by dice_result forward
        {
            target_pos = relative_pos;
        }
        // ########## END of special case checks ################################

        //check for game stuff
        if(isOccupied(target_pos, state)){
            if(isGlobe(target_pos)){
                target_pos = -1; //send me home -> when 2 pieces are on the globe, the new/arriving one,has to leave home
            } else {
                send_them_home(target_pos, state, new_relative_positions);
            }
        }
        new_relative_positions[relative_piece] = target_pos;
    }

    return new_relative_positions;
}

void ludo_player::move_start(int relative_piece,\
                             const std::vector<int>& state,\
                             std::vector<int>& new_state)
{
    if(dice_roll == 6 && state[relative_piece] < 0)
    {
        new_state[relative_piece] = 0; //move me to start
        send_them_home(0, state, new_state); //send pieces home if they are on our start
    }
}

void ludo_player::send_them_home(int relative_index,\
                                 const std::vector<int>& state,\
                                 std::vector<int>& new_state)
{
    for(size_t i = 4; i < state.size(); ++i)
    {
        if(state[i] == relative_index)
        {
            new_state[i] = -1;
        }
    }
}

/*
 * checks on which star the piece is standing
 * returns no. of fields to the next star
*/
int ludo_player::isStar(int index){
    if(index == 5  ||
       index == 18 ||
       index == 31 ||
       index == 44){
        return 6;
    } else if(index == 11 ||
              index == 24 ||
              index == 37 ||
              index == 50){
        return 7;
    }
    return 0;
}

std::vector<int> ludo_player::get_move_candidates(const std::vector<int>& state,\
                                                  int dice_roll)
{
    /*
     * function should return possible candidates
     * for the next player move
    */

    std::vector<int> valid_moves;

    for(int i = 0; i < 4; ++i){
        if(state[i]<0){
            // Pieces which can be released out of jail
            if(dice_roll == 6)
                // Take piece out of jail
                valid_moves.push_back(i);
        }
        // Pieces in the playing circle and home stretch
        else if(state[i]>=0 && state[i] != 99){
            valid_moves.push_back(i);
        }
    }

    return valid_moves;
}

int ludo_player::isOccupied(int index, const std::vector<int>& relative_player_positions)
{
    /*
     * returns number of people of another color
     * on the field with index in relative position
     */
    int number_of_people = 0;

    if( (index > -1) && (index < 51) ){   // check only indexes in the main game circle
        for(size_t i = 4; i < relative_player_positions.size(); ++i)
        {   // start from 4 to skip players own pieces
            if(relative_player_positions[i] == index){
                ++number_of_people;
            }

        }
    }
    return number_of_people;
}

bool ludo_player::isGlobe(int index){
    if(index < 51){  //check only the indexes on the board, not in the home streak
        if(index % 13 == 0 || (index - 8) % 13 == 0)
        {
            return true;
        }
    }
    return false;
}

bool ludo_player::isOwnGlobe(int index){
    if(index < 51){  //check only the indexes on the board, not in the home streak
        if ( index == 0 || (index - 8) % 13 == 0)
        {
            return true;
        }
    }
    return false;
}

bool ludo_player::isOwnBlockade(int index, const std::vector<int>& relative_positions)
{
    if ( (index > -1) && (index < 51) ) //check only main circle
    {
        int number_pieces = 0;
        for (int r = 0; r < 4; ++r)
        {
            if ( relative_positions[r] == index )
                ++number_pieces;
        }
        if (number_pieces > 1)
            return true;
    }

    return false;
}

bool ludo_player::dice_roll_tooMuch(int new_relative_position)
{
    /*
     * the piece is in the goal stretch and
     * dice_roll is larger than remaining number of fields to goal
     */

    if (new_relative_position > 57)
        return true;
    else
        return false;
}

int ludo_player::vulnerableBy(int index, const std::vector<int>& relative_positions)
{
    // check only indexes in the main circle
    if ( (index == -1) || (index > 51) )
         return 0;
    else if ( isOwnGlobe(index) ||\
              isOwnBlockade(index, relative_positions))
    {
        return 0;
    }
    else
    {
        std::pair<int, int> knocking_range;
        // if piece is not further than 6 fields before initial field
        // perform wrap around for negative indexes
        knocking_range.first = (index - 6);
        knocking_range.second = (index - 1);

        int predators = 0;
        // Check for possible attackers on the main circle
        for (int i = knocking_range.first;\
             i <= knocking_range.second; ++i)
        {
            if (i < 0)
            {
                // wrap around for negative i values
                if (isOccupied(i + 52, relative_positions) )
                {
                    ++predators;
                }
            }
            else
            {
                if (isOccupied(i, relative_positions) )
                {
                    ++predators;
                }
            }
        }
        // if piece stands on jail release field,
        // it is vulnerable by pieces still in jail
        if (index % 13 == 0)
        {
            // check if there are pieces of particular color
            // in its jail
            for (int i = 4 * index/13;\
                 i < (4 * index/13) + 4; i++)
            {
                if (relative_positions[i] == -1)
                {
                    ++predators;
                    break;
                }
            }
        }

        return predators;
    }

}

std::vector<int> ludo_player::filter_out_candidates\
                (const std::vector<int>& candidates,\
                 const std::vector<int>& relative_player_positions)
{
    std::vector<int> no_self_kill_filter;
    std::vector<int> filtered_candidates;

    for(size_t c = 0; c < candidates.size(); ++c)
    {
        int new_relative_position =\
                            relative_player_positions[candidates[c]]\
                            + dice_roll;
        // We want to eliminate self kill by moving onto occupied
        // globe possition
        if (!( isGlobe(new_relative_position) &&\
            isOccupied(new_relative_position, relative_player_positions) ) )
        {
            no_self_kill_filter.push_back(candidates[c]);
        }
    }

    // All possible moves results in self kill of piece
    if ( no_self_kill_filter.empty() )
    {
        return candidates;
    }
    else // There are some moves, where it is possible to avoid self kill
    {
        // Try to filter out more by elimminating moves, which results in
        // over taking the goal possition
        for(size_t c = 0; c < no_self_kill_filter.size(); ++c)
        {
            int new_relative_position =\
                                relative_player_positions[candidates[c]]\
                                + dice_roll;

            if ( !dice_roll_tooMuch(new_relative_position) )
            {
                filtered_candidates.push_back(candidates[c]);
            }
        }
        // all moves leads to overtaking goal
        if ( filtered_candidates.empty() )
        {
            return no_self_kill_filter;
        }
        else
        {
            return filtered_candidates;
        }

    }
        /*
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
    */
    //return filtered_candidates;
}

void ludo_player::print_player_piece_positions()
{
    std::cout << "pos_start_of_turn: [ ";
    int i;
    for (i = 0; i < 3; i++)
    {
        std::cout << pos_start_of_turn[i] << ", ";
    }
    std::cout << pos_start_of_turn[i] << " ]"<< std::endl;
}

void ludo_player::print_dice_roll()
{
    std::cout << "dice_roll: " << dice_roll << std::endl;
}
