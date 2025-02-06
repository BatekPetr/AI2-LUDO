/**
 * @file ludo_player_random.h
 * @brief This file contains interface for a ludo_player_random
*/

#ifndef LUDO_PLAYER_RANDOM_H
#define LUDO_PLAYER_RANDOM_H
#include <QObject>
#include <random>

#include "ludo_player.h"

/**
 * @class ludo_player_random
 * @brief The ludo_player_random class extends general ludo_player by pick_random_move function.
 */
class ludo_player_random : public ludo_player {
    Q_OBJECT
private:
    int make_decision() override; // polymorphed function
protected:
    std::random_device rd;
    std::mt19937 gen;

    /**
     * @brief Randomly pick a piece to play with from possible_moves
     * @param possible_moves: Vector of player's possible moves (pieces to play with)
     * @return int identification of randomly selected peace to play with
     */
    int pick_random_move(const std::vector<int>& possible_moves);
public:
    ludo_player_random();
};
#endif // LUDO_PLAYER_RANDOM_H
