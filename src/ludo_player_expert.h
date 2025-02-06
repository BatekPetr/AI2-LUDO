/**
 * @file ludo_player_expert.h
 * @brief This file contains interface for a ludo_player_expert
*/

#ifndef LUDO_PLAYER_EXPERT_H
#define LUDO_PLAYER_EXPERT_H
#include <QObject>

#include "ludo_player_random.h"

/**
 * @class ludo_player_expert
 * @brief The ludo_player_expert class extends ludo_player_random and overrides make_decision() function.
 *
 * @details The main difference of this player to the other players is that,
 * it has the most sophisticated strategy designed by a human. The class contains reward() function, which
 * was designed with advanced human strategy in place and fine-tuned.
 * It shall be considered as the most advanced non-AI player.
 */
class ludo_player_expert : public ludo_player_random
{
    Q_OBJECT
private:
    int make_decision() override; // polymorphed function
protected:
    virtual double reward(std::vector<int>& new_state, int piece_moved);
public:
    ludo_player_expert();
};

#endif // LUDO_PLAYER_EXPERT_H
