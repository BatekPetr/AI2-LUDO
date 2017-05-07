#include "game.h"
#define DEBUG 0

game::game():
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
    gamesTotal(0)
{
    // players is array of Smart pointer to specialized players
    // specialized players are subplasses of the base class <ludo_player>
    // vector <players> is holding pointer to each player
    // in order to put into one vector of base class different sublasses
    // it was needed to use array of Smart pointers of type:
    //                              <std::unique_ptr<ludo_player>>

    std::cout << "Game INITIALIZED." << std::endl;
    //introduce_players();
}

//game::game(ludo_player_genetic &p1, ludo_player_genetic &p2, ludo_player_genetic &p3, ludo_player_genetic &p4):
game::game(ludo_player *p1, ludo_player *p2, ludo_player *p3, ludo_player *p4):
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
    gamesTotal(0)
{
    // players is array of Smart pointer to specialized players
    // specialized players are subplasses of the base class <ludo_player>
    // vector <players> is holding pointer to each player
    // in order to put into one vector of base class different sublasses
    // it was needed to use array of Smart pointers of type:
    //                              <std::unique_ptr<ludo_player>>
    /*
    this->players.emplace_back(&p1);
    this->players.emplace_back(&p2);
    this->players.emplace_back(&p3);
    this->players.emplace_back(&p4);
    */

    this->players.push_back(p1);
    this->players.push_back(p2);
    this->players.push_back(p3);
    this->players.push_back(p4);

    std::cout << "Game INITIALIZED." << std::endl;
    introduce_players();
}

game::game(ludo_player_genetic *p1, ludo_player_genetic *p2,\
           ludo_player_genetic *p3, ludo_player_genetic *p4):
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
    gamesTotal(0)
{
    this->genetic_players.push_back(p1);
    this->genetic_players.push_back(p2);
    this->genetic_players.push_back(p3);
    this->genetic_players.push_back(p4);

    //Set active players
    for (int i = 0; i < genetic_players.size(); ++i)
        this->players.push_back(genetic_players[i]);

    std::cout << "Game INITIALIZED." << std::endl;
    introduce_players();
}

void game::set_test_players(ludo_player *p1, ludo_player *p2, ludo_player *p3, ludo_player *p4)
{
    this->test_players.push_back(p1);
    this->test_players.push_back(p2);
    this->test_players.push_back(p3);
    this->test_players.push_back(p4);
}

void game::set_active_players(ludo_player *p1, ludo_player *p2, ludo_player *p3, ludo_player *p4)
{
    this->players[0] = p1;
    this->players[1] = p2;
    this->players[2] = p3;
    this->players[3] = p4;
}


void game::introduce_players()
{
    std::cout << "Players in the game are: " << std::endl;
    std::cout << "Green:  "; players[0]->say_hi(); std::cout << std::endl;
    std::cout << "Yellow: "; players[1]->say_hi(); std::cout << std::endl;
    std::cout << "Blue:   "; players[2]->say_hi(); std::cout << std::endl;
    std::cout << "Red:    "; players[3]->say_hi(); std::cout << std::endl;
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
 * converts <color> relative piece index to absolute/fixed piece index according to color
*/
int game::rel_to_fixed(int relative_piece_index){
    return relative_piece_index + color * 4;
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

int game::isOccupied(int index){ //returns number of people of another color
    int number_of_people = 0;

    if(index != 99){
        for(size_t i = 0; i < player_positions.size(); ++i){
            if(i < static_cast<size_t>(color)*4 || i >= static_cast<size_t>(color)*4 + 4){        //Disregard own players
                if(player_positions[i] == index){
                    ++number_of_people;
                }
            }
        }
    }
    return number_of_people;
}

bool game::isGlobe(int index){
    if(index < 52){     //check only the indexes on the board, not in the home streak
        if(index % 13 == 0 || (index - 8) % 13 == 0 || isOccupied(index) > 1){  //if more people of the same team stand on the same spot it counts as globe
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
    /*
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
    */
    int selected_piece = players[color]->start_turn(relative);
    movePiece(selected_piece);

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
        /*
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
        */
        bool game_complete = players[color]->post_game_analysis(new_relative);
        turnComplete(game_complete);

        return;
    }

    // Find piece index in the absolute/fixed vector
    int fixed_piece = rel_to_fixed(relative_piece);     //index of the piece in player_positions
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
            //std::cout << "I tought this would be it ";
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
        if(isOccupied(target_pos)){
            if(isGlobe(target_pos)){
                target_pos = -1; //send me home -> when 2 pieces are on the globe, the new/arriving one,has to leave home
            } else {
                send_them_home(target_pos);
            }
        }
        if(DEBUG) std::cout << " => " << target_pos << std::endl;
        player_positions[fixed_piece] = target_pos;
    }
    std::vector<int> new_relative = relativePosition();
    /*
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
    */
    bool game_complete = players[color]->post_game_analysis(new_relative);
    turnComplete(game_complete);

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
    std::ofstream ofs ("running_stats.txt", std::ofstream::out);
    std::ofstream ofs_chromozome("chromozome_development.txt", std::ofstream::out);

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

        if (i % 1000 == 0)
        {
            // every (2i)*1000 (even 1000s) of games evaluate best player performance
            // against random opponents
            // and afterwards change random players to last genetic ones for further evolution
            if ((i / 1000)%2 == 0)
            {
                // save winning statistics
                for(int j = 0; j < 4; j++)
                {
                    ofs << winStats[j] << " ";
                }
                ofs << std::endl;

                players.clear();
                // Put genetic players back into play
                for (int j = 0; j < 4; ++j)
                {
                    players.push_back(genetic_players[j]);
                }

                // clear substitudes_bench
                //substitudes_bench.clear();

                //players[1] = test_players[1];


            }
            // every (2i+1)*1000 evaluate game play of 4 genetic players and create new offsprings
            // And change 3 worst players to random ones for the next 1000 test games
            else // ((i / 1000)%2 != 0)
            {
                // generate new offsprings
                // this does not affect testing against random opponents, since the best player
                // from previous 1000 games is left intact and selected for testing based on
                // winning statistics
                crossover();
                mutation();

                int best, second_best;
                // Find best player for testing it against random opponents in the next set of 1000 games
                find_2_best_players(best, second_best);

                // Change genetic players for testing ones, keep only best genetic player
                // Save pointers to all genetic players to be able to recover them for
                // further evolution
                ludo_player_genetic* best_player = dynamic_cast<ludo_player_genetic*>(players[best]);

                // Save values for genes in chromozome
                for (int j = 1; j < best_player->genes.size(); ++j)
                {
                    ofs_chromozome << best_player->genes[j] << "    ";
                }
                ofs_chromozome << std::endl;

                // Set test players active
                players.clear();
                players.push_back(best_player);
                for (int j = 1; j < 4; ++j)
                {
                    players.push_back(test_players[j]);
                }


            }

            std::cout << "After game " << i << ": ";
            for(int j = 0; j < 3; j++)
                std::cout << winStats[j]/1000.0 << ", ";
            std::cout << winStats[3]/1000.0 << std::endl;

            // zero out winning statistics
            for(int j = 0; j < 4; j++)
            {
                winStats[j] = 0;
            }

            // introduce player for the next set of 1000 games
            introduce_players();
        }
        reset();
    }

    ofs.close();
    ofs_chromozome.close();

#endif

    emit close();
    QThread::exit();
}

void game::find_2_best_players(int &best, int &second_best)
{
    int best_score = INT_MIN;
    int second_best_score = INT_MIN;
    best = 0;
    second_best = 0;

    for (int i = 0; i < players.size(); ++i)
    {
        if(winStats[i] > best_score)
        {
            second_best = best;
            second_best_score = best_score;
            best = i;
            best_score = winStats[i];
        }
        else if (winStats[i] > second_best_score)
        {
            second_best = i;
            second_best_score = winStats[i];
        }
    }
}

void game::find_2_worst_players(int &worst, int &second_worst)
{
    int worst_score = INT_MAX;
    int second_worst_score = INT_MAX;
    worst = 0;
    second_worst = 0;

    for (int i = 0; i < players.size(); ++i)
    {
        if(winStats[i] < worst_score)
        {
            second_worst = worst;
            second_worst_score = worst_score;
            worst = i;
            worst_score = winStats[i];
        }
        else if (winStats[i] < second_worst_score)
        {
            second_worst = i;
            second_worst_score = winStats[i];
        }
    }
}

void game::crossover()
{
    int best, second_best;
    find_2_best_players(best, second_best);
    std::cout << "2 best players are: " << best << ", " << second_best << std::endl;

    //std::vector<double> best_genes = players[best]->get_genes();
    //std::vector<double> second_best_genes = players[best]->get_genes();

    int worst, second_worst;
    find_2_worst_players(worst, second_worst);
    std::cout << "2 worst players are: " << worst << ", " << second_worst << std::endl;

    std::uniform_real_distribution<double> uniform(0.0, 1.0);

    //DEBUG
    //std::cout << "Genes size: " << genetic_players[0]->genes.size() << std::endl;

    for (size_t i = 0; i < genetic_players[0]->genes.size(); ++i)
    {
        double crossover_eps = uniform(gen);
        if (crossover_eps < 0.1)
        {
            genetic_players[worst]->genes[i] = genetic_players[best]->genes[i];
            genetic_players[second_worst]->genes[i] = genetic_players[second_best]->genes[i];
        }
        else
        {
            genetic_players[second_worst]->genes[i] = genetic_players[best]->genes[i];
            genetic_players[worst]->genes[i] = genetic_players[second_best]->genes[i];
        }
    }

    /*
    for (size_t i = 0; i < players[0].genes.size(); ++i)
    {
        double crossover_eps = uniform(gen);
        if (crossover_eps < 0.3)
        {
            players[worst].genes[i] = players[best].genes[i];
            players[second_worst].genes[i] = players[second_best].genes[i];
        }
        else
        {
            players[second_worst].genes[i] = players[best].genes[i];
            players[worst].genes[i] = players[second_best].genes[i];
        }
    }
    */

}

void game::mutation()
{
    int worst, second_worst;
    find_2_worst_players(worst, second_worst);

    std::uniform_real_distribution<double> uniform(0.0, 1.0);
    std::normal_distribution<double> normal(0.0, 0.001);

    for (size_t i = 0; i < genetic_players[0]->genes.size(); ++i)
    {
        double mutation_eps1 = uniform(gen);
        if (mutation_eps1 < 0.1)
        {
            double mutation = normal(gen);
            genetic_players[worst]->genes[i] += mutation;
        }

        double mutation_eps2 = uniform(gen);
        if (mutation_eps2 < 0.1)
        {
            double mutation = normal(gen);
            genetic_players[second_worst]->genes[i] += mutation;
        }
    }

    /*
    for (size_t i = 0; i < players[0].genes.size(); ++i)
    {
        double mutation_eps1 = uniform(gen);
        if (mutation_eps1 < 0.5)
        {
            double mutation = normal(gen);
            players[worst].genes[i] += mutation;
        }

        double mutation_eps2 = uniform(gen);
        if (mutation_eps2 < 0.5)
        {
            double mutation = normal(gen);
            players[second_worst].genes[i] += mutation;
        }
    }
    */
}
