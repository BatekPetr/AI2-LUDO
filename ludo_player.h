#ifndef LUDO_PLAYER_H
#define LUDO_PLAYER_H
#include <QObject>
#include <iostream>
#include <map>
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

    std::vector<int> filter_out_candidates\
                    (const std::vector<int>& candidates,\
                     const std::vector<int>& relative_player_positions);

    std::vector<int> move_piece(int relative_piece);
    void move_start(int relative_piece,\
                    std::vector<int>& new_relative_positions);
    void send_them_home(int relative_index,\
                                     std::vector<int>& new_relative_positions);
    int isStar(int index);
    std::vector<int> get_move_candidates();

    void print_player_piece_positions();
    void print_dice_roll();    
public:
    std::string player_type;
    ludo_player();
signals:
    void select_piece(int);
    void turn_complete(bool);
public slots:
    void start_turn(positions_and_dice relative);
    void post_game_analysis(std::vector<int> relative_pos);
};

#endif // LUDO_PLAYER_H
