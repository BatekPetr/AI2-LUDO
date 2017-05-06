#ifndef GAME_H
#define GAME_H

#define MODE 1 // 0 - single game with GUI; 1 - multiple games, no GUI, with Statistics
#define GAMES_NO 1000000

#include <vector>
#include <random>
#include <iostream>
#include <QtCore>
#include <QThread>

#include <fstream>      // std::ofstream
#include "ludo_player.h"
#include "ludo_player_genetic.h"
#include "positions_and_dice.h"

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
    //std::vector<std::unique_ptr<ludo_player_genetic>> players;
    //std::vector<std::unique_ptr<ludo_player>> players;
    std::vector<ludo_player*> players;

    std::vector<ludo_player_genetic*> genetic_players;
    std::vector<ludo_player*> test_players;

    std::vector<ludo_player*> substitudes_bench;

    bool game_complete;
    bool turn_complete;
    unsigned int game_delay;
    positions_and_dice relative;
    int dice_result;
    std::random_device rd;
    std::mt19937 gen;
    std::vector<int> relativePosition();
    int isStar(int index);
    bool isGlobe(int index);
    int isOccupied(int index); //see if it is occupied and return the piece number
    int rel_to_fixed(int relative_piece_index);
    void send_them_home(int index);
    void move_start(int fixed_piece);
    int next_turn(unsigned int delay);
    static void msleep(unsigned long msecs){
        if(msecs > 0){
            QThread::msleep(msecs);
        }
    }

    void find_2_best_players(int &best, int &second_best);
    void find_2_worst_players(int &worst, int &second_worst);
    void crossover();
    void mutation();

public:
    int turnsNo;
    int color;
    std::vector<int> player_positions;
    std::vector<int> winStats;
    int gamesTotal;

    game();
    //game(ludo_player_genetic &p1, ludo_player_genetic &p2, ludo_player_genetic &p3, ludo_player_genetic &p4);
    //game(ludo_player &p1, ludo_player &p2, ludo_player &p3, ludo_player &p4);
    game(ludo_player *p1, ludo_player *p2, ludo_player *p3, ludo_player *p4);

    game(ludo_player_genetic *p1, ludo_player_genetic *p2,\
               ludo_player_genetic *p3, ludo_player_genetic *p4);

    void set_test_players(ludo_player *p1, ludo_player *p2, ludo_player *p3, ludo_player *p4);
    void set_active_players(ludo_player *p1, ludo_player *p2, ludo_player *p3, ludo_player *p4);

    void introduce_players();

    void rollDice(){
        std::uniform_int_distribution<> dis(1, 6);
        dice_result = dis(gen);
    }
    int getDiceRoll() {return dice_result; }

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
