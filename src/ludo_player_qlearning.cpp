#include "ludo_player_qlearning.h"

ludo_player_Qlearning::ludo_player_Qlearning():ludo_player_expert()
{
    // Change player type from parent class
    this->player_type = "Qlearning";

    // Set initial positions
    for(int i = 0; i < 16; ++i)
    {
        this->previous_state.positions.push_back(-1);
    }
    this->previous_state.dice_roll = 0;
    this->previous_state.action = 0;
    this->previous_state.reward = 0;
    this->previous_state.Q = 0;
}

void ludo_player_Qlearning::setFANN(struct fann *ann_from_main, double *learning_rate_from_main)
{
    this->ann = ann_from_main;
    this->learning_rate = learning_rate_from_main;
}

void ludo_player_Qlearning::restart()
{
    // Set initial positions
    for(auto &i : this->previous_state.positions)
    {
        i = -1;
    }
    this->previous_state.dice_roll = 0;
    this->previous_state.action = 0;
    this->previous_state.reward = 0;
    this->previous_state.Q = 0;
}

int ludo_player_Qlearning::make_decision()
{

    std::vector<int> candidates = get_move_candidates\
                                    (pos_start_of_turn, dice_roll);
    // if there is no possible move for the player
    // return -1 as relative_piece in PLAYER relative position ARRAY
    if ( candidates.empty() )
        return -1;  // skip all the rest of the function

    fann_type ann_input[ANN_INPUTS];
    int chosen_move;
    float current_state_action_Q;

    if ( (rand()%100 < 30) && (*gamesTotal % 10000 > 1000) ) // don't use random moves when testing performance
    {
        chosen_move = pick_random_move(candidates);
        current_state_action_Q = fann_Q_estimate(pos_start_of_turn, dice_roll,\
                                chosen_move, ann_input);
    }
    else
    {
        // sort candidate moves according to reward of moves
        std::map<float, std::vector<int>, std::greater<float>>  move_priorities; //sorts key in decreasing order



        // Find the best candidate from available moves
        for (auto &c : candidates)
        {
            // Estimate Q value using FANN
            current_state_action_Q = fann_Q_estimate(pos_start_of_turn,\
                                          dice_roll,\
                                          c, ann_input);
            /*
            // debug print out
            std::cout << ann_input[0] << ", " << ann_input[1] << ", "
                                      << ann_input[2] << ", " << ann_input[3] << ", "
                                      << ann_input[4] << ", " <<ann_input[5] << ", "
                                      << ann_input[6] << ", " << std::endl;


            //convert_to_FANN_inputs(pos_start_of_turn, new_relative_positions, c, ann_input);
            //float *old_q = fann_run(ann, ann_input);

            // Order possible next moves according to their old Q-value
            */
            move_priorities[current_state_action_Q].push_back(c);
        }
        // pick randomly one move from moves with highest old Q_value
        chosen_move = pick_random_move(move_priorities.begin()->second);
        current_state_action_Q = move_priorities.begin()->first;
    }

    auto new_relative_positions = move_piece\
                                (pos_start_of_turn, chosen_move, dice_roll);
    // Get reward for new state
    //double actual_reward = reward(new_relative_positions, chosen_move);
    double actual_reward = reward(new_relative_positions);

    // skip learning in 1st 1000 games after every x*10 000 games
    // for testing learning statistics
    if (*gamesTotal % 10000 > 1000)
    {
        // Q LEARNING

        // --------------------------------------------------------
        float old_q = current_state_action_Q;
        // Estimate Q value of best successor of new state
        float max_Q_est = estimate_Max_Q(new_relative_positions);


        //Perform Q learning update
        float new_q = old_q + (*learning_rate) *\
                ( actual_reward + DISCOUNT_RATE * max_Q_est - old_q );

        fann_type desired_output = new_q;
        //ANN update
        fann_train(ann, ann_input, &desired_output);
        // --------------------------------------------------------


        // SARSA
/*
        // --------------------------------------------------------
        //Perform SARSA learning update
        float new_q = previous_state.Q + (*learning_rate) *\
                ( previous_state.reward + DISCOUNT_RATE * current_state_action_Q - previous_state.Q );
        fann_type desired_output = new_q;

        fann_type previous_ann_input[ANN_INPUTS];
        convert_to_FANN_inputs(previous_state.positions,\
                               previous_state.dice_roll,\
                               previous_state.action,\
                               previous_ann_input);

        //ANN update
        fann_train(ann, previous_ann_input, &desired_output);

        // Save current state for the update of Q table in the next iteration
        //previous_state.ann = ann_input;
        previous_state.reward = actual_reward;
        previous_state.Q = current_state_action_Q;
        previous_state.positions = pos_start_of_turn;
        previous_state.dice_roll = dice_roll;
        // --------------------------------------------------------
*/
    }


    return chosen_move;
}

float ludo_player_Qlearning::fann_Q_estimate(const std::vector<int>& state,\
                     int dice_roll_local,\
                     int action, fann_type *ann_input)
{
    convert_to_FANN_inputs(state, dice_roll_local, action, ann_input);
    float *q = fann_run(ann, ann_input);

    // Return Q value by dereferencing pointer
    return *q;
}

/*
 * conver to 18 FANN inputs. No scaling. 16 positions + 2 inputs representing action
 */
/*
void ludo_player_Qlearning::convert_to_FANN_inputs(const std::vector<int>& state,\
                                                   const std::vector<int>& new_state,\
                                                   int action,\
                                                   fann_type *ann_input)
{
     for (int i = 0; i < 16; ++i)
     {
        ann_input[i] = state[i];
     }
     ann_input[16] = state[action];
     ann_input[17] = new_state[action];
}
*/


void ludo_player_Qlearning::convert_to_FANN_inputs(const std::vector<int>& state,\
                                                   int dice_roll_local,\
                                                   int action,\
                                                   fann_type *ann_input)
{
    // Zero out init values
    for (int i = 0; i < ANN_INPUTS; ++i)
        ann_input[i] = 0;

    // Convert piece indexes to FANN inputs
    for (int player = 0; player < 4; ++player)
    {
        //std::cout << "Player: " << player << std::endl;
        for (int i = 0; i < 4; ++i)
        {
            //std::cout << "i: " << i << std::endl;
            if (state[4*player + i] == 99)
                ann_input[(player + 1)*59 - 1] += 0.25;
            else
            {
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
                ann_input[idx] += 0.25;
            }
        }
    }
    // add Two other nearal inputs to represent action
    ann_input[236] = state[action]/58.0;
    ann_input[237] = (state[action] + dice_roll_local)/58;

}


float ludo_player_Qlearning::estimate_Max_Q(const std::vector<int>& new_state)
{
    double max_q = -999999999;

    // iterate over all new possible actions
    for (int new_dice_roll = 1;\
         new_dice_roll <= 6; ++new_dice_roll)
    {
        // For each dice_roll find candidates
        std::vector<int> candidates = get_move_candidates\
                                        (new_state, new_dice_roll);
        // if there is no possible move for the player
        // return -1 as relative_piece in PLAYER relative position ARRAY
        if ( candidates.empty() )
        {
            // ToDo
            return -1;  // skip all the rest of the function
        }

        for (auto &c : candidates)
        {
            auto new_relative_positions = move_piece(new_state, c, new_dice_roll);

            fann_type ann_input[ANN_INPUTS];
            // Estimate Q value using FANN
            float new_q = fann_Q_estimate(new_state,\
                                          new_dice_roll,\
                                          c, ann_input);

            /*
            // get value of ANN for each next state
            convert_to_FANN_inputs(new_state, new_relative_positions, c, ann_input);
            float *new_q = fann_run(ann, ann_input);
            */

            // find maximal reward for next state
            if (new_q > max_q)
                max_q = new_q;
        }
    }

    return max_q;
}


double ludo_player_Qlearning::reward(std::vector<int>& new_state)
{
    double reward = 0;

    // Evaluate own player state
    bool win = 1;
    int pieces_in_goal = 0;
    int pieces_in_goal_stretch = 0;
    int pieces_in_jail = 0;


    //Evaluate player own pieces
    // -------------------------------------
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
            continue;
        }
        else if(new_state[i] > 50)
        {
            ++pieces_in_goal_stretch;
            win = 0;
        }
        else
        {
            // any of the pieces is not at the GOAL
            // -> no win
            win = 0;
        }

    }

    // Evaluate other players state
    double opponents_in_jail = 0;
    double opponents_in_goal = 0;
    double opponents_in_goal_stretch = 0;

    for (size_t i = 4; i < new_state.size(); ++i)
    {
        // Evaluate pieces in the jail
        if (new_state[i] == -1)
        {
            ++opponents_in_jail;
        }
        // Evaluate pieces at the Goal
        else if(new_state[i] == 99)
        {
            //reward for pieces already at goal
            ++opponents_in_goal;
        }
        else if(new_state[i] > 51)
        {
            ++opponents_in_goal_stretch;
        }
    }

    reward += pieces_in_goal + 0.5*pieces_in_goal_stretch - pieces_in_jail*5\
            - opponents_in_goal*2 - 0.5*opponents_in_goal_stretch + opponents_in_jail\
            + win*10;

    return reward;
}

