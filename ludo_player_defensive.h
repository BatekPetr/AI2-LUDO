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
    ludo_player_defensive(value_ANN_small *value_ann);
    ludo_player_defensive(value_ANN_small *value_ann, std::string const &file);
};

#endif // LUDO_PLAYER_DEFENSIVE_H
