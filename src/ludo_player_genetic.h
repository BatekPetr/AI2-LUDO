/**
 * @file ludo_player_genetic.h
 * @brief This file contains interface for a ludo_player_genetic
*/

#ifndef LUDO_PLAYER_GENETIC_H
#define LUDO_PLAYER_GENETIC_H

#include <QObject>
#include "ludo_player_random.h"

/**
 * @def NUMBER_OF_GENES
 * @brief Number of Genes in Genetic player's chromosome
 */
#define NUMBER_OF_GENES 7

/**
 * @class ludo_player_genetic
 * @brief The ludo_player_genetic class extends ludo_player_random and overrides make_decision() function.
 *
 * @details This is an AI player, which behaviour is trained and evolved during gameplay.
 */
class ludo_player_genetic : public ludo_player_random
{
    Q_OBJECT
private:
    int make_decision() override;    //polymorphed function

    double get_move_score(const std::vector<int> &move_type, const std::vector<double> &genes);

    int is_defensive(const std::vector<int> &state, int piece_to_move, int dice_roll);
    int is_aggressive(const std::vector<int> &state, int piece_to_move, int dice_roll);
    int is_fast(const std::vector<int> &state, int piece_to_move, int dice_roll);
    int is_release_from_jail(const std::vector<int> &state, int piece_to_move, int dice_roll);
    int is_move_in_goal_stretch(const std::vector<int> &state, int piece_to_move, int dice_roll);
    int is_move_to_goal(const std::vector<int> &state, int piece_to_move, int dice_roll);
    int is_self_kill(const std::vector<int> &state, int piece_to_move, int dice_roll);

public:
    /// \brief Vector of player genes (chromosome)
    std::vector<double> genes;

    ludo_player_genetic();
    ludo_player_genetic(std::vector<double> genes);

    /// \brief Getter of player genes (chromosome)
    std::vector<double> get_genes()
    {
        return this->genes;
    }

    /**
     * @brief Setter of player genes (chromosome)
     * @param genes: Vector of genes (chromosome) weights
     */
    void set_genes(std::vector<double> genes)
    {
        this->genes = genes;
    }

    /**
     * @brief Introduce player and print its chromosome
     * @details Must be overriden in order to print chromosome
     */
    void say_hi() override;

};

#endif // LUDO_PLAYER_GENETIC_H
