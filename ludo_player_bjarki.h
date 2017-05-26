#ifndef ludo_player_BJARKI_H
#define ludo_player_BJARKI_H
#include <QObject>
#include <iostream>
#include "positions_and_dice.h"
#include "ludo_player.h"

class ludo_player_bjarki : public ludo_player {
    Q_OBJECT
private:
    std::vector<double> weights;
    bool isHome(int piece);
    bool isSafe(int piece);
    bool isBarricade(int piece);
    bool toHome(int piece);
    bool toSafe(int piece);
    bool toBarricade(int piece);
    bool toStar(int piece);
    bool toEnemy(int piece);
    bool toGoal(int piece);
    bool nearEnemyBefore(int piece);
    bool nearEnemyAfter(int piece);
    bool isGlobe(int index);
    int isStar(int index);
    int isOccupied(int index);
    std::vector<int> pos_start_of_turn;
    std::vector<int> pos_end_of_turn;
    int dice_roll;
    int make_decision();
    double bias(std::vector<bool> state);
public:
    ludo_player_bjarki();
    void say_hi();
    void setWeights(std::vector<double> w);
signals:
    void select_piece(int);
    void turn_complete(bool);
public slots:
    void start_turn(positions_and_dice relative);
    void post_game_analysis(std::vector<int> relative_pos);
};

#endif // ludo_player_bjarki_H
