#ifndef GAME_H
#define GAME_H

#define MODE 1 // 0 - single game with GUI; 1 - multiple games, no GUI, with Statistics
#define GAMES_NO 50000000

#include <vector>
#include <random>
#include <iostream>
#include <QtCore>
#include <QThread>
#include "ludo_player.h"

#include <fstream>      // std::ofstream

#include "positions_and_dice.h"

#include "fann.h"


#include <memory> // Library for using smart pointers to subclasses of one superclass

/*
enum Color{
    green = 0,
    yellow = 1,
    blue = 2,
    red = 3
};
*/

// static int global_color = 5;

class game : public QThread
{
    Q_OBJECT

private:
    // players is array of Smart pointer to specialized players
    // specialized players are subplasses of the base class <ludo_player>
    // vector <players> is holding pointer to each player
    // in order to put into one vector of base class different sublasses
    // it was needed to use array of Smart pointers of type:
    //                              <std::unique_ptr<ludo_player>>

    std::vector<std::unique_ptr<ludo_player>> players;
    struct fann *ann;
    struct fann *value_ann;
    bool game_complete;
    bool turn_complete;
    unsigned int game_delay;
    positions_and_dice relative;
    int dice_result;
    std::random_device rd;
    std::mt19937 gen;
    std::vector<int> relativePosition();
    int isStar(int index);
    bool isGlobe(int index, int player_No);
    int isOccupied(int index, int player_No); //see if it is occupied and return the piece number
    int rel_to_fixed(int relative_piece_index, int player_No);
    void send_them_home(int index);
    void move_start(int fixed_piece);
    int next_turn(unsigned int delay);
    static void msleep(unsigned long msecs){
        if(msecs > 0){
            QThread::msleep(msecs);
        }
    }
public:
    double learning_rate;
    int turnsNo;
    int color;
    std::vector<int> player_positions;
    std::vector<int> winStats;
    int gamesTotal;

    void setFANN(struct fann *ann);
    void set_Value_ANN(struct fann *value_ann);
    void rollDice(){
        std::uniform_int_distribution<> dis(1, 6);
        dice_result = dis(gen);
    }
    int getDiceRoll() {return dice_result; }
    game(ludo_player &p1, ludo_player &p2, ludo_player &p3, ludo_player &p4);
    void setGameDelay(unsigned int mili_seconds){ game_delay = mili_seconds; }
    void reset();

signals:
    void player1_start(positions_and_dice);
    void player2_start(positions_and_dice);
    void player3_start(positions_and_dice);
    void player4_start(positions_and_dice);

    void player1_end(std::vector<int>);
    void player2_end(std::vector<int>);
    void player3_end(std::vector<int>);
    void player4_end(std::vector<int>);

    void update_graphics(std::vector<int>);
    void set_color(int);
    void set_dice_result(int);
    void declare_winner(int);
    void close();

public slots:
    void turnComplete(bool win);
    void movePiece(int relative_piece); //check game rules
protected:
    void run();
};

#endif // GAME_H
