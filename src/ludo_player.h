/**
 * @file ludo_player.h
 * @brief This file contains interface for a generall ludo_player
 *
 * @todo Decide on and use only one naming convention
*/

#ifndef LUDO_PLAYER_H
#define LUDO_PLAYER_H
#include <QObject>
#include <iostream>
#include <map>
#include "positions_and_dice.h"

/**
 * @class ludo_player
 * @brief The ludo_player class defines LUDO player interface
 */
class ludo_player : public QObject {
    Q_OBJECT
protected:
    std::vector<int> pos_start_of_turn;
    std::vector<int> pos_end_of_turn;
    int dice_roll;
    int* gamesTotal;

    /**
     * @brief make_decision A virtual function to be re-implemented by more specialized player
     * @return integer number of player's piece to play with
     * @note function is used for polymorphysim by different players
     */
    virtual int make_decision();

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
    /**
     * @brief A String name of player type
     *
     * @details E.g.: "aggresssive", "defensive", "expert", "fast", "genetic", etc.
     * @note Would be better to use Enum type
     */
    std::string player_type;

    /**
     * @brief A Vector of player feature genes
     *
     * @note to make it possible to use unique_ptr for other players than just genetic
     */
    std::vector<double> genes;

    /// Default Constructor
    ludo_player();

    /// @brief A Virtual function for identification of Polymorfed player.
    virtual void say_hi();

    /// @brief Setter for No. of gamesTotal variable
    void set_gamesTotal(int* gamesTotal_from_game) { this->gamesTotal = gamesTotal_from_game; }


/**
 * @defgroup LudoPLayerSignals Ludo_player class Qt Signals
 * @note The signals were used in the initial stage of the project,
 * when the game project was handed out with the assignment. During the work on the project,
 * signals and slots were replaced for better scalability.
 */
signals:
    /**
     * @ingroup LudoPLayerSignals
     * @brief select_piece signal emmited when a piece for play turn was chosen
     */
    void select_piece(int);

    /**
     * @ingroup LudoPLayerSignals
     * @brief turn_complete signal emmited when player's turn is finished
     */
    void turn_complete(bool);

/**
 *  @defgroup LudoPlayerSlots Ludo_player class Qt Slots
 */
public slots:
    /**
     * @ingroup LudoPlayerSlots
     * @brief start_turn slot for initiating a player turn
     * @param relative piece positions on the gameplan and dice roll
     * @return integer format of decision, what piece is going to play
     */
    int start_turn(positions_and_dice relative);

    /**
     * @ingroup LudoPlayerSlots
     * @brief post_game_analysis slot starts evaluation of a gamestate after player's turn
     * @param relative_pos player's peaces relative positions vector
     * @return bool, indicating, whether all player's peaces finished the game
     */
    bool post_game_analysis(std::vector<int> relative_pos);
};

#endif // LUDO_PLAYER_H
