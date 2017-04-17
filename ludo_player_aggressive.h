#ifndef LUDO_PLAYER_AGGRESSIVE_H
#define LUDO_PLAYER_AGGRESSIVE_H
#include <QObject>
#include <iostream>
#include "ludo_player.h"

class ludo_player_aggressive : public ludo_player {
    Q_OBJECT
private:
    std::random_device rd;
    std::mt19937 gen;
    int make_decision(); // polymorphed function
    bool isGlobe(int index);
    int isOccupied(int index);
public:
    ludo_player_aggressive();
};

#endif // LUDO_PLAYER_AGGRESSIVE_H
