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
    std::random_device rd;
    std::mt19937 gen;
    int make_decision(); // polymorphed function
public:
    ludo_player_random();
};
#endif // LUDO_PLAYER_RANDOM_H
