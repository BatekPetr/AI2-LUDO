#ifndef LUDO_PLAYER_FAST_H
#define LUDO_PLAYER_FAST_H
#include <QObject>
#include <iostream>
#include "ludo_player.h"

class ludo_player_fast : public ludo_player {
    Q_OBJECT
private:
    int make_decision(); // polymorphed function
public:
    ludo_player_fast();
    ludo_player_fast(value_ANN_small *value_ann);
    ludo_player_fast(value_ANN_small *value_ann, std::string const &file);
};

#endif // LUDO_PLAYER_FAST_H
