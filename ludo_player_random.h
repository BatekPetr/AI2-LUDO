#ifndef LUDO_PLAYER_RANDOM_H
#define LUDO_PLAYER_RANDOM_H
#include <QObject>
#include <iostream>
#include "positions_and_dice.h"
#include "ludo_player.h"

/* Children of class ludo_player
 * make_decision() function is polymorphed/changed
 * for Random Player
 */
class ludo_player_random : public ludo_player {
    Q_OBJECT
private:
    int make_decision(); // polymorphed function
protected:
    std::random_device rd;
    std::mt19937 gen;
    int pick_random_move(const std::vector<int>& possible_moves);
public:
    ludo_player_random();
    ludo_player_random(struct fann *value_ann);
    ludo_player_random(struct fann *value_ann, std::string const &file);
};
#endif // LUDO_PLAYER_RANDOM_H
