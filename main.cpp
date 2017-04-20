#include "dialog.h"
#include <QApplication>
#include "game.h"
#include <vector>
#include "ludo_player.h"
#include "ludo_player_random.h"
#include "ludo_player_fast.h"
#include "ludo_player_aggressive.h"
#include "ludo_player_defensive.h"
#include "ludo_player_expert.h"
#include "positions_and_dice.h"

#include <stdio.h>
//#include "fann.h" // FAST ANN library
//#include "doublefann.h"
#include "floatfann.h"


Q_DECLARE_METATYPE( positions_and_dice )

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    qRegisterMetaType<positions_and_dice>();

    //load existing ANN
    const char* ANN_file = "";
    //instanciate FANN
    const unsigned int num_input = 16;
    const unsigned int num_output = 1;
    const unsigned int num_layers = 2;
    const unsigned int num_neurons_hidden = 20;

    struct fann *ann = fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);

    if (ANN_file != "")
    {
        ann = fann_create_from_file("ANN_file.txt");
    }
    else
    {
        ann = fann_create_standard(num_layers, num_input,
            num_neurons_hidden, num_output);
    }

    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

    //instanciate the players here
    ludo_player_defensive p1;
    ludo_player p2;
    ludo_player p3;
    ludo_player_expert p4;

    game g;

    
    QObject::connect(&g,SIGNAL(close()),&a,SLOT(quit()));

    //set up for each player
    QObject::connect(&g, SIGNAL(player1_start(positions_and_dice)),&p1,SLOT(start_turn(positions_and_dice)));
    QObject::connect(&p1,SIGNAL(select_piece(int)),                &g, SLOT(movePiece(int)));
    QObject::connect(&g, SIGNAL(player1_end(std::vector<int>)),    &p1,SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&p1,SIGNAL(turn_complete(bool)),              &g, SLOT(turnComplete(bool)));

    QObject::connect(&g, SIGNAL(player2_start(positions_and_dice)),&p2,SLOT(start_turn(positions_and_dice)));
    QObject::connect(&p2,SIGNAL(select_piece(int)),                &g, SLOT(movePiece(int)));
    QObject::connect(&g, SIGNAL(player2_end(std::vector<int>)),    &p2,SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&p2,SIGNAL(turn_complete(bool)),              &g, SLOT(turnComplete(bool)));

    QObject::connect(&g, SIGNAL(player3_start(positions_and_dice)),&p3,SLOT(start_turn(positions_and_dice)));
    QObject::connect(&p3,SIGNAL(select_piece(int)),                &g, SLOT(movePiece(int)));
    QObject::connect(&g, SIGNAL(player3_end(std::vector<int>)),    &p3,SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&p3,SIGNAL(turn_complete(bool)),              &g, SLOT(turnComplete(bool)));

    QObject::connect(&g, SIGNAL(player4_start(positions_and_dice)),&p4,SLOT(start_turn(positions_and_dice)));
    QObject::connect(&p4,SIGNAL(select_piece(int)),                &g, SLOT(movePiece(int)));
    QObject::connect(&g, SIGNAL(player4_end(std::vector<int>)),    &p4,SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&p4,SIGNAL(turn_complete(bool)),              &g, SLOT(turnComplete(bool)));

#if MODE==0
    // Add a GUI <-- remove the '/' to uncomment block
    // ---------------------------------------------------
    g.setGameDelay(100); //if you want to see the game, set a delay

    Dialog w;
    QObject::connect(&g,SIGNAL(update_graphics(std::vector<int>)),&w,SLOT(update_graphics(std::vector<int>)));
    QObject::connect(&g,SIGNAL(set_color(int)),                   &w,SLOT(get_color(int)));
    QObject::connect(&g,SIGNAL(set_dice_result(int)),             &w,SLOT(get_dice_result(int)));
    QObject::connect(&g,SIGNAL(declare_winner(int)),              &w,SLOT(get_winner()));
    QObject::connect(&g,SIGNAL(close()),&a,SLOT(quit()));
    w.show();

    g.start();
    a.exec();
    // ---------------------------------------------------


#else
    std::ofstream ofs ("RunningStats.txt", std::ofstream::out);

    ofs << " ================== LUDO Game running statistics ================== " << std::endl;
    ofs << " ------------------------------------------------------------------ " << std::endl;
    ofs << " Players: " << p1.player_type << " , " << p2.player_type << " , "\
                               << p3.player_type << " , " << p4.player_type << " , " << std::endl;
    //ofs << " ------------------------------------------------------------------ " << std::endl;

    ofs.close();

    // start QApplication
    g.start();
    a.exec();


    std::cout << " ================== LUDO Game results ================== " << std::endl;
    std::cout << "Total games played: " << g.gamesTotal << std::endl;
    std::cout << "Victory stats: " << std::endl;
    std::cout << " ------------------------------------------------------- " << std::endl;
    std::cout << " Players | " << p1.player_type << " | " << p2.player_type << " | "\
                               << p3.player_type << " | " << p4.player_type << " | " << std::endl;
    std::cout << " ------------------------------------------------------- " << std::endl;
    std::cout << "Stats: ";
    for(int i = 0; i < 3; i++)
        std::cout << g.winStats[i] << ", ";
    std::cout << g.winStats[3] << " ]"<< std::endl;
#endif

    return 0;
}
