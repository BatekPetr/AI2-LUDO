#ifndef LUDO_PLAYER_GENETIC_H
#define LUDO_PLAYER_GENETIC_H

#include <QObject>
#include "ludo_player_random.h"

#define NUMBER_OF_GENES 7

class ludo_player_genetic : public ludo_player_random
{
    Q_OBJECT
private:
    int make_decision();    //polymorphed function

    double get_move_score(const std::vector<int> &move_type, const std::vector<double> &genes);

    int is_defensive(const std::vector<int> &state, int piece_to_move, int dice_roll);
    int is_aggressive(const std::vector<int> &state, int piece_to_move, int dice_roll);
    int is_fast(const std::vector<int> &state, int piece_to_move, int dice_roll);
    int is_release_from_jail(const std::vector<int> &state, int piece_to_move, int dice_roll);
    int is_move_in_goal_stretch(const std::vector<int> &state, int piece_to_move, int dice_roll);
    int is_move_to_goal(const std::vector<int> &state, int piece_to_move, int dice_roll);
    int is_self_kill(const std::vector<int> &state, int piece_to_move, int dice_roll);

public:
    std::vector<double> genes;

    ludo_player_genetic();
    ludo_player_genetic(std::vector<double> genes);

    std::vector<double> get_genes()
    {
        return this->genes;
    }

    void set_genes(std::vector<double> genes)
    {
        this->genes = genes;
    }

    void say_hi();

};

#endif // LUDO_PLAYER_GENETIC_H
