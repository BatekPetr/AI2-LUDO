/**
 * @file game.h
 * @brief This file defines interface for a LUDO game
 *
 * @todo Decide on and use only one naming convention
*/

#ifndef GAME_H
#define GAME_H

/**
 * @def MODE
 * @brief Switch for selection between a Single GUI game or multiple NO-GUI games
 * @param 0 - single game with GUI
 * @param 1 - multiple games, no GUI, with Statistics
 */
#define MODE 0

/**
 * @def GAMES_NO
 * @brief Number of Games to play in case of Multiple games (MODE == 1)
 */
#define GAMES_NO 10000

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

/**
 * @class game
 * @brief The game class defines LUDO game interface
 */
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

    /**
     * @brief Converts relative to <color> piece index [0-3]
     * to absolute/fixed piece index in player_positions vector [0-15]
    */
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
    /// Identification of current player [0-3]
    int color;
    /// LUDO pieces positions on the game-plan
    std::vector<int> player_positions;
    /// Number of wins of the players. Used in Multi-game scenario (MODE == 1)
    std::vector<int> winStats;
    /// Number of finished games
    int gamesTotal;
    /// Number of turns in a game
    int turnsNo;

    game();
    //game(ludo_player_genetic &p1, ludo_player_genetic &p2, ludo_player_genetic &p3, ludo_player_genetic &p4);
    //game(ludo_player &p1, ludo_player &p2, ludo_player &p3, ludo_player &p4);
    game(ludo_player *p1, ludo_player *p2, ludo_player *p3, ludo_player *p4);

    game(ludo_player_genetic *p1, ludo_player_genetic *p2,\
               ludo_player_genetic *p3, ludo_player_genetic *p4);

    /**
     * @brief Set test players for evaluation of player evolution/strength.
     * @param p1: pointer to object implementing ludo_player interface
     * @param p2: pointer to object implementing ludo_player interface
     * @param p3: pointer to object implementing ludo_player interface
     * @param p4: pointer to object implementing ludo_player interface
     */
    void set_test_players(ludo_player *p1, ludo_player *p2, ludo_player *p3, ludo_player *p4);

    /**
     * @brief Set active players for the next game.
     * @param p1: pointer to object implementing ludo_player interface
     * @param p2: pointer to object implementing ludo_player interface
     * @param p3: pointer to object implementing ludo_player interface
     * @param p4: pointer to object implementing ludo_player interface
     */
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
