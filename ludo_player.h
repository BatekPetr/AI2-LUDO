#ifndef LUDO_PLAYER_H
#define LUDO_PLAYER_H
#include <QObject>
#include <iostream>
#include <map>
#include "positions_and_dice.h"
#include <fstream>
#include "value_ann_small.h"

#include "fann.h"

// ANN Parameters
#define ANN_INPUTS 238
#define VAL_ANN_INPUTS 14

//QL Parameters
#define LEARNING_RATE 1
#define DISCOUNT_RATE 0.95


class ludo_player : public QObject {
    Q_OBJECT
protected:
    std::vector<int> pos_start_of_turn;
    std::vector<int> pos_end_of_turn;

    value_ANN_small *value_ann;
    fann_type ann_input_START_of_turn[VAL_ANN_INPUTS];
    fann_type ann_input_END_of_turn[VAL_ANN_INPUTS];

    float estimate_V_of_START_state;
    float estimate_V_of_END_state;

    double *V_learning_rate;
    double discount_rate = 0.95;
    int dice_roll;
    int* gamesTotal;

    virtual int make_decision();    //function is used for polymorphysim by different players

    float fann_V_estimate(const std::vector<int>& state,\
                         fann_type *ann_input);

    void convert_to_FANN_inputs(const std::vector<int>& state,\
                                fann_type *ann_input);

    int isOccupied(int index,\
        const std::vector<int>& relative_player_positions);

    bool isGlobe(int index);
    bool isOwnGlobe(int index);

    bool isOwnBlockade(int index,\
         const std::vector<int>& relative_positions);
    bool isOpponentReleaseField(int index);

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

    float reward_Win_Loss(std::vector<int> state);

    void print_player_piece_positions();
    void print_dice_roll();    


public:       
    std::ofstream ofs_val_ann_error;
  //  std::ofstream ofs_TD_error;

    std::string player_type;

    //make class uncopyable, because of streams
    ludo_player(const ludo_player&);
    ludo_player& operator=(const ludo_player&);

    // Class contructors
    ludo_player();
    ludo_player(value_ANN_small *value_ann);
    ludo_player(value_ANN_small *value_ann, double *V_learning_rate);
    ludo_player(value_ANN_small *value_ann, std::string const &file);
    ludo_player(value_ANN_small *value_ann, double *V_learning_rate, std::string const &file);

    void open_ANN_oftream(std::ofstream &ofs);
    void open_TD_oftream(std::ofstream &ofs);

    void set_V_Learning_Rate(double* V_lr);
    virtual ~ludo_player() {}  // it's important to define virtual destructor as ludo_player is a base class

    virtual void greet();

    void set_gamesTotal(int* gamesTotal_from_game)
        {                   this->gamesTotal = gamesTotal_from_game; }
    void after_game_reward(double reward);
signals:
    void select_piece(int);
    void turn_complete(bool);
public slots:
    void start_turn(positions_and_dice relative);
    void post_game_analysis(std::vector<int> relative_pos);
};

#endif // LUDO_PLAYER_H
