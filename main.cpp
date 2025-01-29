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
#include "ludo_player_genetic.h"


#include "positions_and_dice.h"

#include <stdio.h>


Q_DECLARE_METATYPE( positions_and_dice )

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    qRegisterMetaType<positions_and_dice>();
    qRegisterMetaType<std::vector<int>>();



    //instanciate the players here

    // Player after 500 evolution episodes
    //ludo_player_genetic p1({0.0972394, 0.272931, 0.236266, 0.399862, -0.83557, -0.345521, -1.24923});
    /*
    ludo_player_genetic p1({1,0,0,0,0,0,0});
    ludo_player_genetic p2({0,1,0,0,0,0,0});
    ludo_player_genetic p3({0,0,1,0,0,0,0});
    ludo_player_genetic p4({0,0,0,1,0,0,0});
    */
    ludo_player_genetic p1({0.744648, 0.117856, 0.487484, 1.40823, -0.632367, 1.03243, -2.21635});
    ludo_player_genetic p2({0.684924, 0.188297, 0.744413, 1.33665, -0.538997, 1.03243, -2.21635 });
    ludo_player_genetic p3({0.95887, 0.16744, 0.487484, 1.35318, -0.632367, 1.03676, -2.21635});
    ludo_player_genetic p4({0.684924, 0.117856, 0.487484, 1.25273, -0.584465, 1.03243, -2.21635});
    game g(&p1,&p2,&p3,&p4);


    ludo_player_random rd_p1;
    ludo_player rd_p2;
    ludo_player_expert rd_p3;
    ludo_player_expert rd_p4;
    g.set_test_players(&rd_p1, &rd_p2, &rd_p3, &rd_p4);


    QObject::connect(&g,SIGNAL(close()),&a,SLOT(quit()));

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
    std::cout << "Stats: [";
    for(int i = 0; i < 3; i++)
        std::cout << g.winStats[i] << ", ";
    std::cout << g.winStats[3] << " ]"<< std::endl;
#endif

    return 0;
}
