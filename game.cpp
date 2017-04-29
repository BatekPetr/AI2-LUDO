#include "game.h"
#define DEBUG 0


game::game(ludo_player &p1, ludo_player &p2, ludo_player &p3, ludo_player &p4):
    game_complete(false),
    turn_complete(true),
    game_delay(0),
    relative(),
    dice_result(1),
    rd(),
    gen(rd()),
    turnsNo(0),
    color(3),
    player_positions({-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}),
    winStats({0,0,0,0}),
    gamesTotal(0),
    learning_rate(1)
{
    // players is array of Smart pointer to specialized players
    // specialized players are subplasses of the base class <ludo_player>
    // vector <players> is holding pointer to each player
    // in order to put into one vector of base class different sublasses
    // it was needed to use array of Smart pointers of type:
    //                              <std::unique_ptr<ludo_player>>
    this->players.emplace_back(&p1);
    this->players.emplace_back(&p2);
    this->players.emplace_back(&p3);
    this->players.emplace_back(&p4);
    //std::cout << "Game INITIALIZED." << std::endl;
}

void game::setFANN(value_ANN_small *ann_from_main)
{
    this->ann = ann_from_main;
}

void game::set_Value_ANN(value_ANN_small *value_ann)
{
    this->value_ann = value_ann;
}

void game::reset(){
    game_complete = false;
    turn_complete = true;

    for(auto &i : player_positions){ //without & we're changing the copy made in auto rather than the player_position
        i = -1;
    }
    std::uniform_int_distribution<> start_player\
                                    (0, 4-1);
    color = start_player(gen);
    
    //std::cout << "Game RESET." << std::endl;
}

/*
 * converts <player_No> relative piece index to absolute/fixed piece index according to player_No
*/
int game::rel_to_fixed(int relative_piece_index, int player_No){
    return (relative_piece_index + player_No * 4) % 16;
}

/*
 * checks on which star the piece is standing 
 * returns no. of fields to the next star
*/
int game::isStar(int index){
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

int game::isOccupied(int index, int player_No){ //returns number of people of another color
    int number_of_people = 0;

    if(index != 99){
        for(size_t i = 0; i < player_positions.size(); ++i){
            if(i < static_cast<size_t>(player_No)*4 || i >= static_cast<size_t>(player_No)*4 + 4){        //Disregard own players
                if(player_positions[i] == index){
                    ++number_of_people;
                }
            }
        }
    }
    return number_of_people;
}

bool game::isGlobe(int index, int player_No){
    if(index < 52){     //check only the indexes on the board, not in the home streak
        if(index % 13 == 0 || (index - 8) % 13 == 0 || isOccupied(index, player_No) > 1){  //if more people of the same team stand on the same spot it counts as globe
            return true;
        }
    }
    return false;
}

void game::send_them_home(int index){
    for(size_t i = 0; i < player_positions.size(); ++i){
        if(i < static_cast<size_t>(color)*4 || i >= static_cast<size_t>(color)*4 + 4){        //this way we don't skip one player position
            if(player_positions[i] == index){
                player_positions[i] = -1;
            }
        }
    }
}

void game::move_start(int fixed_piece){
    if(dice_result == 6 && player_positions[fixed_piece] < 0){
        player_positions[fixed_piece] = color*13; //move me to start
        send_them_home(color*13); //send pieces home if they are on our start
    }
}

int game::next_turn(unsigned int delay = 0){

    turnsNo++;
    //Boolean variable indicating that someone has won the game
    if(game_complete){
        return 0;
    }
    
    // switch team turns by iterating over colors
    switch(color){
        case 0:
        case 1:
        case 2:
            ++color;
            break;
        case 3:
        default:
            color = 0;
            break;
    }
    // global_color = color;
    
    //Roll dice
    rollDice();
    relative.dice = getDiceRoll();
    
    //Get possitions relative to <color> team
    relative.pos = relativePosition();
    emit set_color(color);
    emit set_dice_result(dice_result);

    msleep(delay);

    switch(color){
        case 0:
            emit player1_start(relative);
            break;
        case 1:
            emit player2_start(relative);
            break;
        case 2:
            emit player3_start(relative);
            break;
        case 3:
            emit player4_start(relative);
        default:
            break;
    }


    // Using Smart point array to players instead of emmiting signals
    //players[color]->start_turn(relative);

    return 0;
}

/*
 * moves ONE piece, selected by player
 */
void game::movePiece(int relative_piece){
    // Check if there is a piece to move
    // -1 means, there is not any piece with possibility to move
    if (relative_piece == -1)
    {
        std::vector<int> new_relative = relativePosition();
        switch(color){
            case 0:
                emit player1_end(new_relative);
                break;
            case 1:
                emit player2_end(new_relative);
                break;
            case 2:
                emit player3_end(new_relative);
                break;
            case 3:
                emit player4_end(new_relative);
            default:
                break;
        }
        return;
    }

    // Find piece index in the absolute/fixed vector
    int fixed_piece = rel_to_fixed(relative_piece, color);     //index of the piece in player_positions
    int modifier = color * 13;
    // Get Absolute/fixed position of a particular piece
    int relative_pos = player_positions[fixed_piece];
    int target_pos = 0;
    
    // Convert absolute/fixed position to relative one
    // -------------------------------------------------
    if(player_positions[fixed_piece] == -1)
    { //if the selected piece is in the safe house, try to move it to start
        move_start(fixed_piece);
    }
    else
    {
        //convert to relative position
        if(relative_pos == 99)
        {
            // selected relative piece is already at the goal
            std::cout << "I tought this would be it ";
        }
        /* //Probably not needed in new indexing
        else if(relative_pos == 51){ //if people land on 51, they shouldn't be sent to goal stretch
            switch(color){
            case 0 : relative_pos = 51; break;
            case 1 : relative_pos = 38; break;
            case 2 : relative_pos = 25; break;
            case 3 : relative_pos = 12; break;
            }
        } 
        */

        // Is true when the piece is in a goal stretch. In this case, compute relative <color> index of piece, using mystery formula below.
        //This condition has to be before condition indexes larger than modifier but <= 50
        else if( relative_pos > 51)
        {
            // Formula to get relative piece position in the goal stretch from absolute/fixed positions for different <color>s
            relative_pos = relative_pos - color * 5;
        }
        else if(relative_pos < modifier)
        {
            relative_pos = relative_pos + 52 - modifier;
        }
        // For positions larger than modifier but smaller than goal stretch (<= 51)
        else {//if(relative >= modifier)
            relative_pos = relative_pos - modifier;
        }
        // Conversion before player turn to RELATIVE indexes is DONE
        // ---------------------------------------------------------
        
        
        if(DEBUG) std::cout << "color: " << color << " pos: " << relative_pos << " + " << dice_result << " = " << relative_pos + dice_result;

        // Update relative index by score from dice roll
        // Check for entering goal stretch
        if ( (relative_pos < 51) && (relative_pos + dice_result) > 50 )
        {
            // this needs to be done when entering goal stretch
            // goal stretch starts at 52 while main game circle
            // ends at 50 for relative indexing
            relative_pos += dice_result + 1;
        }
        else
        {
            //this is relative position of the selected token + the dice number
            relative_pos += dice_result;
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
        
        // target_pos is in absolute/fixed indexing
        
        // ########## special case checks ################################
        // This condition happens when player tries to move a piece in its goal stretch
        if(relative_pos > 57 && relative_pos < 73){ // go back
            //If the player moves over the goal, it should move backwards
            // target_pos is in absolute/fixed indexes,
            // hence adding color * 5
            target_pos = 57-(relative_pos-57) + color * 5; // + 1;
        }
        // Move piece to GOAL 
        else if(relative_pos == 57 || relative_pos >= 99)
        {
            target_pos = 99;
        }
        else if(relative_pos > 50) // goal stretch
        {
            //each field in goal stretch has its own number.
            //Starts with 52 for first color.
            //Each goal stretch has 5 fields -> multiplication by 5
            target_pos = relative_pos + color * 5;
        }
        else {
            // Get absolute/fixed position for piece outside from goal stretch
            int new_pos = relative_pos + modifier;
            if(new_pos < 52){
                target_pos = new_pos;
            } else { //wrap around
                target_pos = new_pos - 52;  //this is the global position wrap around at the green entry point
            }
        }
        // ########## END of special case checks ################################

        //check for game stuff
        if(isOccupied(target_pos, color)){
            if(isGlobe(target_pos, color)){
                target_pos = -1; //send me home -> when 2 pieces are on the globe, the new/arriving one,has to leave home
            } else {
                send_them_home(target_pos);
            }
        }
        if(DEBUG) std::cout << " => " << target_pos << std::endl;
        player_positions[fixed_piece] = target_pos;
    }
    std::vector<int> new_relative = relativePosition();
    switch(color){
        case 0:
            emit player1_end(new_relative);
            break;
        case 1:
            emit player2_end(new_relative);
            break;
        case 2:
            emit player3_end(new_relative);
            break;
        case 3:
            emit player4_end(new_relative);
        default:
            break;
    }
    emit update_graphics(player_positions);
}

/*
 * calculates relative positions of each figure for team in <color>
*/
std::vector<int> game::relativePosition(){
    //CAREFULL -> relative_positions are just copy of player positios INITIALLY
    // They are changed later
    
    std::vector<int> relative_positions;
    int modifier = color * 13;

    // Put actual <color> player pieces on the first 4 indexes of player relative possitions array
    // ------------------------------------------------------------------
    //from start id to end
    for(int i = color*4; i < static_cast<int>(player_positions.size()); ++i){
        relative_positions.push_back(player_positions[i]);
    }
    //from 0 to start id
    for(int i = 0; i < color*4; ++i){
        relative_positions.push_back(player_positions[i]);
    }
    // ------------------------------------------------------------------


    for(size_t i = 0; i < relative_positions.size(); ++i){
        // Positions at the Goal or Home
        if(relative_positions[i] == 99 || relative_positions[i] == -1)
        {
            relative_positions[i] = (relative_positions[i]);
        } 
        // Piece is in front of its initial position. Even though in relative index it is further than initial position
        // of the its Home(modifier) -> Absolute index is less than modifier(absolute init, position), because it crossed 
        // init. position of the 0th player -> low absolute position index
        else if(relative_positions[i] < modifier) {
            relative_positions[i] = (relative_positions[i]+52-modifier);
        } 
        // Is true when the piece is in a GOAL STRETCH. In this case, compute relative <color> index of piece, using formula below. Color*5 - 1 is there because each color goal stretch has different position numbers. 
        //This condition has to be before condition for indexes larger than modifier but <= 50
        else if(relative_positions[i] > 52) {
            //if((relative_positions[i] > 51 + (1 + color)*5))
            if((relative_positions[i] >= 52 + color*5))
                relative_positions[i] = (relative_positions[i]-color*5);
            else if ((relative_positions[i] < 52 + (color)*5))
                relative_positions[i] = (relative_positions[i]+(4-color)*5);
        } 
        else if(relative_positions[i] >= modifier) {
            relative_positions[i] = (relative_positions[i]-modifier);
        }
    }
    
    // Returns positions relative to given <color> as an rvalue (temporary variable = exist only until its next use)
    return std::move(relative_positions);
}

void game::turnComplete(bool win){
    game_complete = win;
    turn_complete = true;
    if(game_complete){
        //std::cout << "Game Finished." << std::endl;
        //std::cout << "player: " << color << " won" << std::endl;
        ++winStats[color];
        ++gamesTotal;

        emit declare_winner(color);
    }
}

void game::run() {
    if(DEBUG) std::cout << "color:     relative pos => fixed\n";

    for (int i = 0; i < players.size(); ++i)
    {
        players[i]->greet();
    }
#if MODE==0
    while(!game_complete)
    {
        if(turn_complete)
        {
            turn_complete = false;
            msleep(game_delay/4);
            next_turn(game_delay - game_delay/4);
        }
    }
   std::cout << "Game complete." << std::endl;
   std::cout << "Player " << color << " WON!" << std::endl;

#else
    std::ofstream ofs ("RunningStats.txt", std::ofstream::app);

    for(int i = 1; i < GAMES_NO + 1; ++i)
    {
        while(!game_complete)
        {
            if(turn_complete)
            {
                turn_complete = false;
                msleep(game_delay/4);
                next_turn(game_delay - game_delay/4);
            }

            //std::cout << "Game complete." << std::endl;
        }
        /*
        if (i % 1000 == 0 )
        {
            std::cout << "After game " << i << ": ";
            for(int j = 0; j < 3; j++)
                std::cout << winStats[j]/(float)i << ", ";
            std::cout << winStats[3]/(float)i << std::endl;

            ofs << "After game " << i << ": ";
            for(int j = 0; j < 3; j++)
                ofs << winStats[j]/(float)i << ", ";
            ofs << winStats[3]/(float)i << std::endl;                

        }
        */
        if (i % 10000 == 0 )
        {
            // zero out winning statistics
            for(int j = 0; j < 4; j++)
                winStats[j] = 0;

            if (i > 9999)
                value_ann->save_to_file("TD_Value_ANN_small.txt");
        }
        else if (i % 10000 == 1000)
        {
            std::cout << "After game " << i << ": ";
            for(int j = 0; j < 3; j++)
                std::cout << winStats[j]/1000.0 << ", ";
            std::cout << winStats[3]/1000.0 << std::endl;


        }
        else if (i % 5000 == 0)
        {
            if (i>1)
                learning_rate = learning_rate*0.95;
        }

        // Set final rewards for each LOOSING player
        for (int i = 0; i < 4; ++i)
        {
            if (i!=color)
                players[i]->after_game_reward(-100);
            else
                players[i]->after_game_reward(100);
        }

        reset();
    }
    for (int i = 0; i < 4; ++i)
    {
        players[i]->ofs_val_ann_error.close();
        //players[i].ofs_TD_error.close();
    }
    ofs.close();
#endif

    emit close();
    QThread::exit();
}
