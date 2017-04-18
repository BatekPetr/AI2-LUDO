#ifndef LUDO_PLAYER_DEFENSIVE_H
#define LUDO_PLAYER_DEFENSIVE_H
#include <QObject>
#include <iostream>
#include "ludo_player_random.h"

class ludo_player_defensive : public ludo_player_random
{
    Q_OBJECT
private:
    int make_decision(); // polymorphed function
public:
    ludo_player_defensive();
};

#endif // LUDO_PLAYER_DEFENSIVE_H
