#include "ludo_player_michael.h"
#include <random>
#include <iostream>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <string>
#include <fstream>
#include <cstdlib>
#include <time.h>

using namespace std;

ludo_player_michael::ludo_player_michael():
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    dice_roll(0)
{


    for (int i = 0; i < 2; i++)
    {
        for (int ii = 0; ii < 2; ii++)
        {

            for (int j = 0; j < 58; j++)
            {
                for (int k = 0; k < 6; k++)
                {
                    q_table[i][ii][j][k] = 0;
                }
            }
        }
    }
    // initialize pointers.
    for (int i = 0; i < 4; i++)
    {
        (value[i]) = &(q_table[0][0][0][0]);
    }
    // Initialize rewards for reaching goals
    //for (int i = 0; i < 7; i++)
    //{
    //    for (int j = 0; j < 6; j++)
    //    {
    //        q_table[i][57][j] = 1;
    //    }
    //}

    // initialize rewards for sending opponent back home.

    /*for (int j = 0; j < 58; j++)
    {
        for (int k = 0; k < 6; k++)
        {
            q_table[k+1][j][k] = 0;
        }
    }*/


    // set seed
    std::srand(time(NULL));
    win_count = 0;

    // initialize pointers

    update_qtable = false;

    for (int i = 0; i < 4; i++)
        value[i] = NULL;

}

void ludo_player_michael::say_hi()
{
    std::cout << "Michael";
}

int ludo_player_michael::make_decision(){


    //cout << "in decision making" << endl;
    int maxval = 3;


    // Check for invalid moves
    bool invalid_bricks[4];
    int n_invalid_bricks = 0;

    for (int i = 0; i < 4; i++)
    {
        moves_to_enemy[i] = 0;
        moves_to_friend[i] = 0;

        if (pos_start_of_turn[i] == 99 || (pos_start_of_turn[i] == -1 && dice_roll != 6))
        {
            invalid_bricks[i] = true;
            n_invalid_bricks++;
        }
        else
        {
            invalid_bricks[i] = false;
            maxval = i;
        }

    }


    // Inevitably, this giant forloop is need.
    for (int i = 0; i < 4; i++)
    {


        brick_pos[i] = pos_start_of_turn[i]+1;
        if (brick_pos[i] > 57)
        {
            brick_pos[i] = 57;
            break;
        }

        if (!invalid_bricks[i])
        {
            if(1) // made it collapsable
            {
                for (int j = 4; j < 16; j++)
                {
                    if (pos_start_of_turn[j]-pos_start_of_turn[i] == dice_roll)
                    {
                        moves_to_enemy[i] = 1;
                    }
                }


                for (int j = 0; j < 4; j++)
                {
                    if (i != j && pos_start_of_turn[j]-pos_start_of_turn[i] == dice_roll)
                    {
                        moves_to_friend[i] = 1;
                    }
                }
                //cout << "dist to enemy ok" << endl;




            }




            //cout << "checking: " << moves_to_enemy[i] << " " << brick_pos[i] << " " << dice_roll-1 << endl;



            value[i] = &(q_table[moves_to_enemy[i]][moves_to_friend[i]][brick_pos[i]][dice_roll-1]);

            //cout << "brick pos ok" << endl;

            // Check what brick gives the highest expected return
            //cout << "i: " << i << " maxval: " << maxval << endl;
            if (value[maxval] != NULL && value[i] != NULL)
            {
                if(*(value[i]) > *(value[maxval]))
                {
                    maxval = i;
                }
            }
            //cout << "last check" << endl;

        }
    }

    //cout << "for loop ok" << endl;

    // add some randomness randomness is a problem, due to the way this work!
    /*double random = std::rand()*(1-0)/(double)RAND_MAX;
    //std::cout << "random " << random << std::endl;
    if (random > 0.95 && n_invalid_bricks != 4)
    {
        // Choose a random, but valid brick!
        maxval = std::rand()/(RAND_MAX/4);
        while(invalid_bricks[maxval])
        {
            maxval = std::rand()/(RAND_MAX/4);
        }

        //maxval = 3;
        //std::cout << "choosing random:  " << maxval << std::endl;
    }//*/

    //cout << "random ok" << endl;

    double reward = 0;
    // Brick is selected. Now check for rewards:
    // Reward for leaving base:
    if (brick_pos[maxval] == 0 && dice_roll == 6)
    {
        reward += 5;
        //if (dist_to_enemy[maxval] == 1)
        //    reward += 1.5;
    }
    // Punishment for wasting turn:
    if (brick_pos[maxval] == 0 && dice_roll != 6)
    {
        reward -= 1;
        //if (dist_to_enemy[maxval] == 1)
        //    reward += 1.5;
    }

    // reward for going to a star:
    int star_pos[8] = {6,12,19,25,32,36,43,49};
    for (int i = 0; i < 8; i++)
    {
        if ((brick_pos[maxval]+dice_roll) == star_pos[i])
            reward += 5;
    }

    // reward for going to a globe
    //(only if they are not right outside opponent base)
    int globe_pos[5] = {1,9,22,35,48};
    for (int i = 0; i < 5; i++)
    {
        if ((brick_pos[maxval]+dice_roll) == globe_pos[i])
            reward += 5;
        //punish for moving from globe
        if (brick_pos[maxval] == globe_pos[i])
            reward -= 10;
    }


    if (moves_to_enemy[maxval])
        reward += 5;

    if (moves_to_friend[maxval])
        reward += 10;

    if ((brick_pos[maxval] + dice_roll) == 57)
        reward += 5;

    if ((brick_pos[maxval] + dice_roll) > 57)
        reward -= 10; // Set value to zero, because this move is just back and forth.

    //cout << "rewards calc ok" << endl;

    // max expected value form next step: (Q-learning)
    double max = 0;
    double sum = 0;

    for (int i = 0; i < 2; i++)
    {
        for (int ii = 0; ii < 2; ii++)
        {

            if (brick_pos[maxval] + dice_roll < 58)
            {
                sum = 0;
                for (int j = 0; j < 6; j++)
                {
                    if (q_table[i][ii][brick_pos[maxval] + dice_roll][j] > sum)
                    sum = q_table[i][ii][brick_pos[maxval] + dice_roll][j];
                }
                if (sum > max)
                {
                    max = sum;
                }

            }
        }
    }


    //double learning_rate;
    // update learning rate
    /*if(win_count < 100)
        learning_rate = 0.01;
    else
        learning_rate = 1/win_count;
*/
    // update value
    if ( maxval < 4 && maxval >= 0 && value[maxval] != NULL)
    {
        if (update_qtable)
        {
            *(value[maxval]) +=  0.1 * (reward + 0.5*max - (*(value[maxval])) );
        }


    }



    //cout << "done decision making" << maxval << endl;

    return maxval;



}



void ludo_player_michael::start_turn(positions_and_dice relative){

    //cout << "start_turn" << endl;
    pos_start_of_turn = relative.pos;
    dice_roll = relative.dice;

    int decision = make_decision();
    //cout << "decision ok" << endl;


    emit select_piece(decision);
    //QThread::usleep(1);

    //std::cout << "pos of brick 0: " << pos_end_of_turn[0] << std::endl;
    //double sum = 0;
}

void ludo_player_michael::post_game_analysis(std::vector<int> relative_pos){
    //cout << "analysis start " << endl;
    pos_end_of_turn = relative_pos;
    bool game_complete = true;
    for(int i = 0; i < 4; ++i){
        if(pos_end_of_turn[i] < 99){
            game_complete = false;
        }
    }
    if (game_complete)
    {
        //int nicearray[3];
        //nicearray[4] = 3;

        /*double sum = 0;
        for (int i = 0; i < 2; i++)
        {
            for (int ii = 0; ii < 2; ii++)
            {
                for (int j = 0; j < 58; j++)
                {
                    for (int k = 0; k < 6; k++)
                    {
                        sum += q_table[i][ii][j][k];
                    }
                }
            }
        }*/
        win_count++;
        //std::cout << "total sum of rewards: " << sum << std::endl;

    }

    emit turn_complete(game_complete);
    //QThread::usleep(1);
    //cout << "analysis end " << game_complete <<  endl;
}

void ludo_player_michael::saveQtable(string filename)
{
    ofstream myfile;
    myfile.open (filename);
    for (int i = 0; i < 2; i++)
    {
        for (int ii = 0; ii < 2; ii++)
        {
            for (int j = 0; j < 58; j++)
            {
                for (int k = 0; k < 6; k++)
                {
                    myfile << q_table[i][ii][j][k] << endl;
                }
            }
        }
    }

    myfile.close();
}
void ludo_player_michael::loadQtable(string filename)
{
    std::string locale = setlocale(LC_ALL, NULL); // save old locale
    setlocale(LC_ALL, "C"); // change locale
    string inputstring;
    ifstream myfile;
    myfile.open (filename);
    for (int i = 0; i < 2; i++)
    {
        for (int ii = 0; ii < 2; ii++)
        {
            for (int j = 0; j < 58; j++)
            {
                for (int k = 0; k < 6; k++)
                {
                    getline(myfile,inputstring);
                    q_table[i][ii][j][k] = strtod(inputstring.c_str(),NULL);

                }
            }
        }
    }

    myfile.close();
}
