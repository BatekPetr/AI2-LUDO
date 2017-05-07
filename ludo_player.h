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
    int* gamesTotal;

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

    std::vector<int> move_piece(const std::vector<int>& state,\
                                int relative_piece, int dice_roll);
    void move_start(int relative_piece,\
                    const std::vector<int>& state,\
                    std::vector<int>& new_state);
    void send_them_home(int relative_index,\
                        const std::vector<int>& state,\
                        std::vector<int>& new_state);
    int isStar(int index);
    std::vector<int> get_move_candidates(const std::vector<int>& state,
                                         int dice_roll);

    void print_player_piece_positions();
    void print_dice_roll();    
public:
    std::string player_type;
    std::vector<double> genes;  //to make it possible to use unique_ptr for other players than just genetic


    ludo_player();
    virtual void say_hi();

    void set_gamesTotal(int* gamesTotal_from_game)
        {                   this->gamesTotal = gamesTotal_from_game; }
signals:
    void select_piece(int);
    void turn_complete(bool);
public slots:
    int start_turn(positions_and_dice relative);
    bool post_game_analysis(std::vector<int> relative_pos);
};

#endif // LUDO_PLAYER_H
