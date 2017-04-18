#include "ludo_player.h"
#include <random>

ludo_player::ludo_player():
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    dice_roll(0)
{
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

void ludo_player::start_turn(positions_and_dice relative){
    pos_start_of_turn = relative.pos;
    dice_roll = relative.dice;
    int decision = make_decision();
    emit select_piece(decision);
}

void ludo_player::post_game_analysis(std::vector<int> relative_pos){
    pos_end_of_turn = relative_pos;
    bool game_complete = true;
    for(int i = 0; i < 4; ++i){
        if(pos_end_of_turn[i] < 99){
            game_complete = false;
        }
    }
    emit turn_complete(game_complete);
}

std::vector<int> ludo_player::get_move_candidates()
{
    /*
     * function should return possible candidates
     * for the next player move
    */

    std::vector<int> valid_moves;

    for(int i = 0; i < 4; ++i){
        if(pos_start_of_turn[i]<0){
            // Pieces which can be released out of jail
            if(dice_roll == 6)
                // Take piece out of jail
                valid_moves.push_back(i);
        }
        // Pieces in the playing circle and home stretch
        else if(pos_start_of_turn[i]>=0 && pos_start_of_turn[i] != 99){
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

    if (new_relative_position > 56)
        return true;
    else
        return false;
}

int ludo_player::vulnerableBy(int index, const std::vector<int>& relative_positions)
{
    // check only indexes in the main circle
    if ( (index == -1) || (index > 50) )
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
