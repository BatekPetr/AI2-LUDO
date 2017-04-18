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
    std::vector<int> filter_out_candidates\
                    (const std::vector<int>& candidates,\
                     const std::vector<int>& relative_player_positions);
public:
    ludo_player_aggressive();
};

#endif // LUDO_PLAYER_AGGRESSIVE_H
