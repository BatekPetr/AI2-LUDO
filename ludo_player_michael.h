#ifndef LUDO_PLAYER_MICHAEL_H
#define LUDO_PLAYER_MICHAEL_H
#include <QObject>
#include <iostream>
#include <string>
#include "positions_and_dice.h"
#include "ludo_player.h"

class ludo_player_michael : public ludo_player {
    Q_OBJECT
private:
    std::vector<int> pos_start_of_turn;
    std::vector<int> pos_end_of_turn;
    int dice_roll;
    int make_decision();
    double q_table[2][2][58][6]; // dist to enemy, dist to goal, dice
    int moves_to_enemy[4];
    int moves_to_friend[4];
    int brick_pos[4];
    double * value[4];



public:
    ludo_player_michael();
    void say_hi();
    void saveQtable(std::string);
    void loadQtable(std::string);
    int win_count;
    bool update_qtable;
    int game_count;
signals:
    void select_piece(int);
    void turn_complete(bool);
public slots:
    void start_turn(positions_and_dice relative);
    void post_game_analysis(std::vector<int> relative_pos);
};

#endif // LUDO_PLAYER_MICHAEL_H
