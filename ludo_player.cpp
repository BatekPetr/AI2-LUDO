#include "ludo_player.h"
#include <random>

ludo_player::ludo_player():
    player_type("General"),
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    dice_roll(0),
    V_learning_rate(NULL)
{
    this->value_ann = NULL;
}

ludo_player::ludo_player(value_ANN_small *value_ann):
    player_type("General"),
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    dice_roll(0)
{
    this->value_ann = value_ann;
}

ludo_player::ludo_player(value_ANN_small *value_ann, double *V_lr):
    player_type("General"),
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    dice_roll(0)
{
    this->value_ann = value_ann;
    set_V_Learning_Rate(V_lr);
}

ludo_player::ludo_player(value_ANN_small *value_ann, std::string const &file):\
    ofs_val_ann_error(file.c_str(),std::ofstream::app),
    player_type("General"),
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    dice_roll(0)
{
    this->value_ann = value_ann;
}

ludo_player::ludo_player(value_ANN_small *value_ann, double *V_lr, std::string const &file):\
    ofs_val_ann_error(file.c_str(),std::ofstream::app),
    player_type("General"),
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    dice_roll(0)
{
    this->value_ann = value_ann;
    set_V_Learning_Rate(V_lr);
}

void ludo_player::set_V_Learning_Rate(double* V_lr)
{
    this->V_learning_rate = V_lr;
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

float ludo_player::fann_V_estimate(const std::vector<int>& state,\
                                   fann_type *ann_input)
{
    convert_to_FANN_inputs(state, ann_input);
    float V = value_ann->evaluate(ann_input);

    // Return Q value by dereferencing pointer
    return V;
}

void ludo_player::convert_to_FANN_inputs(const std::vector<int>& state,\
                                         fann_type *ann_input)
{
    // Zero out init values
    for (int i = 0; i < VAL_ANN_INPUTS; ++i)
        ann_input[i] = 0;

    // Evaluate player own positions
    for (int i = 0; i < 4; ++i)
    {
        int predators = vulnerableBy(state[i], state);
        //Pieces on goal
        if ( state[i] == 99)
            ann_input[0] += 0.25;
        //Pieces in jail
        else if (state[i] == -1)
        {
            ann_input[1] += 0.25;
        }
        //Pieces in goal stretch
        else if (state[i] > 51)
        {
            ann_input[2] += 0.25;
        }
        // Pieces on save field
        else if (isOwnGlobe(state[i]) || isOwnBlockade(state[i], state))
        {
            ann_input[3] += 0.25;
        }
        // Vulnerable Pieces
        else if ( predators > 0 || isOpponentReleaseField(state[i]))
        {
            ann_input[4] += (predators/4.0 + 1)* 0.25;
        }
    }

    // Evaluate opponents possitions
    int piece_state;
    for (int player = 1; player < 4; ++player)
    {
        //std::cout << "Player: " << player << std::endl;
        for (int i = 0; i < 4; ++i)
        {
            piece_state = state[4*player + i];
            // Opponent pieces on goal
            if ( piece_state == 99)
                ann_input[3*player + 2] += 0.25;
            // Opponent pieces in jail
            else if (piece_state == -1)
            {
                ann_input[3*player + 3] += 0.25;
                /*
                int relative_position = state[4*player + i];
                if (relative_position > 56)
                    relative_position -= 5*player;
                int idx = (player*59) + relative_position + 1;
                /*
                int temp = (player*59) + (state[4*player + i] + 1);
                if (temp > 235)
                    temp -= 5*player; //opponents in their goal stretch
                    */
                //std::cout << "temp: " << temp << std::endl;
                /*
                ann_input[idx] += 0.25;
                */
            }
            // Opponent pieces in goal stretch
            else if (state[i] > 51)
            {
                ann_input[3*player + 4] += 0.25;
            }
        }
    }
}


void ludo_player::start_turn(positions_and_dice relative){
    pos_start_of_turn = relative.pos;
    dice_roll = relative.dice;

    double actual_reward = 0; // no rewards for intermediate states

    // Update ANN which estimates Value function
    // ------------------------------------------------------------
    // recompute inputs for ANN from new <pos_start_of_turn> inside
    // computing state Value estimate for <pos_start_of_turn>
    estimate_V_of_START_state = fann_V_estimate(pos_start_of_turn,\
                                  ann_input_START_of_turn);

    // Temporal Difference update for Value of previous END turn state
    float updated_V_of_END_state = estimate_V_of_END_state + ((*V_learning_rate) *\
        actual_reward + discount_rate*estimate_V_of_START_state - estimate_V_of_END_state);

    if (ofs_val_ann_error.is_open())
    {
        // Print squared error to the file
        ofs_val_ann_error << (updated_V_of_END_state - estimate_V_of_END_state) << "  ";
    }


    // Update weights in ANN
    value_ann->train(ann_input_END_of_turn, &updated_V_of_END_state);

    // Debugging output
    float temp = value_ann->evaluate(ann_input_END_of_turn);
    // ------------------------------------------------------------

    int decision = make_decision();
    //std::vector<int> new_relative_positions = move_piece(decision);
    emit select_piece(decision);
}

void ludo_player::post_game_analysis(std::vector<int> relative_pos){
    pos_end_of_turn = relative_pos;
    bool game_complete = true;
    double actual_reward = 0; // reward for winning the game

    // Check for winning the game
    for(int i = 0; i < 4; ++i){
        if(pos_end_of_turn[i] < 99){
            game_complete = false;

            break;
        }
    }

    /*
    if (game_complete)
        std::cout<<"complete"<<std::endl;
    */

    // Update ANN which estimates Value function
    // ------------------------------------------------------------
    float updated_V_of_START_state;
    // if player won, we don't want to estimate V of winning state
    // assign only reward wor WIN
    /*
    if (game_complete)
    {
        // Wait with assing reward !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        actual_reward = 100; // reward for winning the game

        // Temporal Difference update for Value of previous END turn state
        updated_V_of_START_state = estimate_V_of_START_state + ((*V_learning_rate) *\
            actual_reward - estimate_V_of_START_state);
    }
    else
    */
    {
        // recompute inputs for ANN from new <pos_end_of_turn> inside
        // computing state Value estimate for <pos_end_of_turn>
        estimate_V_of_END_state = fann_V_estimate(pos_end_of_turn,\
                                      ann_input_END_of_turn);

        // Temporal Difference update for Value of previous END turn state
        updated_V_of_START_state = estimate_V_of_START_state + ((*V_learning_rate) *\
            actual_reward + discount_rate*estimate_V_of_END_state - estimate_V_of_START_state);
    }

    if (ofs_val_ann_error.is_open())
    {
        // Print squared error to the file
        ofs_val_ann_error << (updated_V_of_START_state - estimate_V_of_START_state) << std::endl;
    }

    // Update weights in ANN
    value_ann->train(ann_input_START_of_turn, &updated_V_of_START_state);
    // ------------------------------------------------------------

    emit turn_complete(game_complete);
}
void ludo_player::after_game_reward(double reward)
{
    // Temporal Difference update for Value of previous END turn state
    float updated_V_of_END_state = estimate_V_of_END_state + ((*V_learning_rate) *\
        reward - estimate_V_of_END_state);

    // Update weights in ANN
    value_ann->train(ann_input_END_of_turn, &updated_V_of_END_state);
}

/*
float ludo_player::assign_final_rewards(std::vector<int> final_state)
{
    float game_reward = reward_Win_Loss(final_state);
    return game_reward;
}
*/

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

bool ludo_player::isOpponentReleaseField(int index){
    if(index < 51){  //check only the indexes on the board, not in the home streak
        if ( index != 0 && index % 13 == 0)
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

// TO DO
/*
void ludo_player::update_ANN_after_loss()
{
    float new_Q =
    fann_train(pos_end_of_turn
}
*/

float ludo_player::reward_Win_Loss(std::vector<int> state)
{
    bool win = true;
    // Check for WIN
    for (int i = 0; i < 4; ++i)
    {
        if (state[i] != 99)
            win = false;
    }

    if (win == true)
    {
        return 100;
    }

    // Check for Loss
    // Iterate over players
    for (int p = 1; p < 4; ++p)
    {
        bool loss = true;
        // Iterate over pieces of given player
        for (int i = 0; i < 4; ++i)
        {
            if (state[4*p + i] != 99)
            {
                loss = false;
                break;
            }
        }
        if (loss == true)
        {
            return -100;
        }
    }

    // NO Win either Loss
    return 0;
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

void ludo_player::greet()
{
    std::cout << "Hello I'm player " << player_type << std::endl;
}
