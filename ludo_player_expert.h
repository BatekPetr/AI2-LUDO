#ifndef LUDO_PLAYER_EXPERT_H
#define LUDO_PLAYER_EXPERT_H
#include <QObject>
#include <iostream>
#include "positions_and_dice.h"
#include "ludo_player.h"

/* Children of class ludo_player
 * make_decision() function is polymorphed/changed
 * for Random Player
 */
class ludo_player_expert : public ludo_player
{
    Q_OBJECT
private:
    std::random_device rd;
    std::mt19937 gen;
    int make_decision(); // polymorphed function
    double reward(std::vector<int>& new_state, int piece_moved);
    int pick_random_move(const std::vector<int>& possible_moves);
public:
    ludo_player_expert();
};

#endif // LUDO_PLAYER_EXPERT_H
