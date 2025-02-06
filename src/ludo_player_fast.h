/**
 * @file ludo_player_fast.h
 * @brief This file contains interface for a ludo_player_fast
*/

#ifndef LUDO_PLAYER_FAST_H
#define LUDO_PLAYER_FAST_H
#include <QObject>
#include <iostream>
#include "ludo_player.h"

/**
 * @class ludo_player_fast
 * @brief The ludo_player_fast class overrides virtual function make_decision of general ludo_player
 */
class ludo_player_fast : public ludo_player {
    Q_OBJECT
private:
    int make_decision() override; // polymorphed function
public:
    ludo_player_fast();
};

#endif // LUDO_PLAYER_FAST_H
