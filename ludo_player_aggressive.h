#ifndef LUDO_PLAYER_AGGRESSIVE_H
#define LUDO_PLAYER_AGGRESSIVE_H
#include <QObject>
#include <iostream>
#include "ludo_player_random.h"

class ludo_player_aggressive : public ludo_player_random {
    /*
     * Children of RANDOM player -> possibility to use inheritted
     * function <pick_random_move(...)> when there is no option to
     * eliminate another player
     */

    Q_OBJECT
private:
    int make_decision(); // polymorphed function

public:
    ludo_player_aggressive();
    ludo_player_aggressive(struct fann *value_ann);
};

#endif // LUDO_PLAYER_AGGRESSIVE_H
