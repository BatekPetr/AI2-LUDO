#ifndef LUDO_PLAYER_H
#define LUDO_PLAYER_H
#include <QObject>
#include <iostream>
#include "positions_and_dice.h"

class ludo_player : public QObject {
    Q_OBJECT
protected:
    std::vector<int> pos_start_of_turn;
    std::vector<int> pos_end_of_turn;
    int dice_roll;
    virtual int make_decision();    //function is used for polymorphysim by different players

    int isOccupied(int index,\
        const std::vector<int>& relative_player_positions);

    bool isGlobe(int index);
    bool isOwnGlobe(int index);

    bool isOwnBlockade(int index,\
         const std::vector<int>& relative_positions);

    bool dice_roll_tooMuch(int new_relative_position);

    int vulnerableBy(int index,\
                const std::vector<int>& relative_positions);
    void print_player_piece_positions();
    void print_dice_roll();
    std::vector<int> get_move_candidates();
public:
    ludo_player();
signals:
    void select_piece(int);
    void turn_complete(bool);
public slots:
    void start_turn(positions_and_dice relative);
    void post_game_analysis(std::vector<int> relative_pos);
};

#endif // LUDO_PLAYER_H
