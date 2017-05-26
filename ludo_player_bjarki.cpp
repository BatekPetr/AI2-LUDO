#include "ludo_player_bjarki.h"
#include <stdio.h>

using namespace std;

ludo_player_bjarki::ludo_player_bjarki():
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    dice_roll(0)
{
}

void ludo_player_bjarki::say_hi()
{
    std::cout << "Bjarki";
}

// TODO: Check how isGlobe() treats home square (fix toHome and isHome)
// TODO: Fix future checks from home square

// TODO: Fix all future checks to account for star jumps

int ludo_player_bjarki::make_decision(){
    std::vector<int> valid_moves;
    if(dice_roll == 6){
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i]<0){
                valid_moves.push_back(i);
            }
        }
    }
    for(int i = 0; i < 4; ++i){
        if(pos_start_of_turn[i]>=0 && pos_start_of_turn[i] != 99){
            valid_moves.push_back(i);
        }
    }
    if(valid_moves.size()==0){
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i] != 99){
                valid_moves.push_back(i);
            }
        }
    }

    vector<double> selection;
    //uint64_t state;
    for (int piece = 0; piece < 4; ++piece) {
        std::vector<bool> state;
        state.push_back( this->isHome(piece) );
        state.push_back( this->isSafe(piece) );
        state.push_back( this->isBarricade(piece) );
        state.push_back( this->toHome(piece) );
        state.push_back( this->toSafe(piece) );
        state.push_back( this->toBarricade(piece) );
        state.push_back( this->toStar(piece) );
        state.push_back( this->toEnemy(piece) );
        state.push_back( this->toGoal(piece) );
        state.push_back( this->nearEnemyBefore(piece) );
        state.push_back( this->nearEnemyAfter(piece) );

        selection.push_back(this->bias(state));
        /**
        cout << "State " << piece << ": ";
        for (int i = 0; i < 11; ++i) {
            cout << state[i] << " ";
        }
        cout << endl;//*/
        //cout << this->isHome(piece) << endl;
        //cout << this->isSafe(piece) << endl;
        //cout << this->isBarricade(piece) << endl;
        //cout << this->toHome(piece) << endl;
        //cout << this->toSafe(piece) << endl;
        //cout << this->toBarricade(piece) << endl;
        //cout << this->toStar(piece) << endl;
        //cout << this->toEnemy(piece) << endl;
        //cout << this->toGoal(piece) << endl;
        //cout << this->nearEnemyBefore(piece) << " " << this->nearEnemyAfter(piece) << endl;
    }
    sort(valid_moves.begin(), valid_moves.end());
    vector<double> valid_selection = {-99,-99,-99,-99};
    for (size_t i = 0; i < selection.size(); ++i) {
        if(find(valid_moves.begin(), valid_moves.end(), (int)i) != valid_moves.end()) {
            valid_selection[i] = selection[i];
        }
    }
    int select = distance(valid_selection.begin(), max_element(valid_selection.begin(), valid_selection.end()));

    //population pop;
    //pop.printFirst();
    
    //cout << valid_moves.size() << endl;
    /**
    cout << "Valid moves: ";
    for (size_t i = 0; i < valid_moves.size(); ++i) {
        cout << valid_moves[i] << " ";
    }
    cout << endl;

    cout << "Selection bias: ";
    for (size_t i = 0; i < selection.size(); ++i) {
        cout << selection[i] << " ";
    }
    cout << endl;

    cout << "Valid selection: ";
    for (size_t i = 0; i < valid_selection.size(); ++i) {
        cout << valid_selection[i] << " ";
    }
    cout << endl;
    cout << "Selected " << select << endl;
    cout << endl;//*/

    //cout << "selected " << valid_moves[select] << "with index " << select << endl;

    return select;
}

bool ludo_player_bjarki::isHome(int piece) {
	if ( pos_start_of_turn[piece] < 0 ) {
  	return true;
  }
  return false;
}


// Skip checking for home position as it leads to another safe pos
bool ludo_player_bjarki::isSafe(int piece) {
	if ( this->isGlobe(pos_start_of_turn[piece]) 
                ||	ludo_player_bjarki::isBarricade(piece)
				||	pos_start_of_turn[piece] > 51) {
		return true;
	}
	return false;
}

bool ludo_player_bjarki::isBarricade(int piece) {
	if ( pos_start_of_turn[piece] == 99 || pos_start_of_turn[piece] < 0) return false;
	for (int i = 0; i < 4; ++i) {
		if (piece == i) continue;
		if ( pos_start_of_turn[piece] == pos_start_of_turn[i] )
			return true;
	}
	return false;
}

bool ludo_player_bjarki::toHome(int piece) {
	int target = pos_start_of_turn[piece] + dice_roll;
	if ( this->isOccupied(target) > 1
				||	( this->isOccupied(target)
							&&	this->isGlobe(target) ) ) {
		return true;
	}
	return false;
}

bool ludo_player_bjarki::toSafe(int piece) {
	if ( this->isGlobe( pos_start_of_turn[piece] + dice_roll )
                ||	ludo_player_bjarki::toBarricade(piece)
				||	pos_start_of_turn[piece] + dice_roll > 51) {
		return true;
	}
	return false;
}

bool ludo_player_bjarki::toBarricade(int piece) {
    if ( pos_start_of_turn[piece] == 99 || pos_start_of_turn[piece] < 0) return false;
    for (int i = 0; i < 4; ++i) {
		if (piece == i) continue;
		if ( pos_start_of_turn[piece] + dice_roll == pos_start_of_turn[i] )
			return true;
	}
	return false;
}

bool ludo_player_bjarki::toStar(int piece) {
    if ( pos_start_of_turn[piece] == 99 || pos_start_of_turn[piece] < 0) return false;
	if ( this->isStar( pos_start_of_turn[piece] + dice_roll )) {
		return true;
	}
	return false;
}

bool ludo_player_bjarki::toEnemy(int piece) {
    if ( pos_start_of_turn[piece] == 99 || pos_start_of_turn[piece] < 0) return false;
	if ( this->isOccupied( pos_start_of_turn[piece] + dice_roll ) == 1 ) {
		return true;
	}
	return false;
}

bool ludo_player_bjarki::toGoal(int piece) {
    if ( pos_start_of_turn[piece] == 99 || pos_start_of_turn[piece] < 0) return false;
	if ( pos_start_of_turn[piece] + dice_roll == 56) {
		return true;
	}
	return false;
}

//TODO: ignore enemies in -1
bool ludo_player_bjarki::nearEnemyBefore(int piece) {
	for (int i = 1; i <= 6; ++i) {
        if ( pos_start_of_turn[piece] - i < 0) break;
		if ( this->isOccupied( pos_start_of_turn[piece] - i ) ) {
			return true;
		}
	}
	return false;
}

bool ludo_player_bjarki::nearEnemyAfter(int piece) {
	for (int i = 1; i <= 6; ++i) {
		if ( this->isOccupied( pos_start_of_turn[piece] + dice_roll - i ) ) {
			return true;
		}
	}
	return false;
}

int ludo_player_bjarki::isStar(int index){
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

int ludo_player_bjarki::isOccupied(int index) { //	Returns number of people of another color
  int number_of_people = 0;
	if( index != 99 && index >= 0) {
		for ( size_t i = 4; i < pos_start_of_turn.size(); ++i) {
			if ( pos_start_of_turn[i] == index ) {
				++number_of_people;
			}
		}
	}
	return number_of_people;
}

bool ludo_player_bjarki::isGlobe(int index){
    if(index < 52){     //check only the indexes on the board, not in the home streak
        if(index % 13 == 0 || (index - 8) % 13 == 0 || isOccupied(index) > 1){  //if more people of the same team stand on the same spot it counts as globe
            return true;
        }
    }
    return false;
}

void ludo_player_bjarki::setWeights(std::vector<double> w) {
    this->weights = w;
    return;
}

double ludo_player_bjarki::bias(vector<bool> state) {
    double b = 0;
    for (size_t i = 0; i < state.size(); ++i) {
        b += state[i] ? this->weights[i] : 0;
    }
    return b;
}

void ludo_player_bjarki::start_turn(positions_and_dice relative){
    pos_start_of_turn = relative.pos;
    dice_roll = relative.dice;
    //Print pos_start_of_turn vector *
    //for (std::vector<int>::const_iterator i = pos_start_of_turn.begin(); i != pos_start_of_turn.end(); ++i)
    //	std::cout << *i << ' ';
    //cout << endl << dice_roll << endl;
    int decision = make_decision();
    emit select_piece(decision);
}

void ludo_player_bjarki::post_game_analysis(std::vector<int> relative_pos){
    pos_end_of_turn = relative_pos;
    bool game_complete = true;
    for(int i = 0; i < 4; ++i){
        if(pos_end_of_turn[i] < 99){
            game_complete = false;
        }
    }
    emit turn_complete(game_complete);
}

