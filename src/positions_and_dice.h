/**
 * @file positions_and_dice.h
 * @brief This file contains definition of positions_and_dice struct
 */

#ifndef POSITIONS_AND_DICE_H
#define POSITIONS_AND_DICE_H
#include <vector>

/**
 * @brief Represents pieces positions in the gameplay and dice-roll
 */
struct positions_and_dice {
    /// Vector of all LUDO pieces positions on the game-plan
    std::vector<int> pos;
    /// Dice roll result
    int dice;
    positions_and_dice() {}
    positions_and_dice(int v) : dice(v) {}
};
#endif // POSITIONS_AND_DICE_H
