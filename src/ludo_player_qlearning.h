#ifndef LUDO_PLAYER_QLEARNING_H
#define LUDO_PLAYER_QLEARNING_H
#include "ludo_player_expert.h"
#include "fann.h"

#include <stdlib.h>     /* srand, rand */

//ANN Parameters
#define ANN_INPUTS 238
#define ANN_LEARNING_RATE 0.5
//QL Parameters
#define LEARNING_RATE 1
#define DISCOUNT_RATE 0.95

class ludo_player_Qlearning : public ludo_player_expert
{
private:
    struct fann *ann;

    //definition of structure for ANN state
    struct state_ANN {
            struct fann *ann;
            int action;
            int dice_roll;
            double reward;
            float Q;
            std::vector<int> positions;
        };

    state_ANN previous_state;

    double *learning_rate;

    float fann_Q_estimate(const std::vector<int>& state,\
                         int dice_roll,\
                         int action, fann_type *ann_input);

    void convert_to_FANN_inputs(const std::vector<int>& state,\
                                int dice_roll,\
                                int action,\
                                fann_type *ann_input);
    //--------------
    int make_decision();
    float estimate_Max_Q(const std::vector<int>& new_state);
    double reward(std::vector<int>& new_state);
public:
    ludo_player_Qlearning();
    void setFANN(struct fann *ann, double *learning_rate);
    void restart();
};

#endif // LUDO_PLAYER_QLEARNING_H
