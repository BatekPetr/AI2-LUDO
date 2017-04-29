#ifndef VALUE_ANN_SMALL_H
#define VALUE_ANN_SMALL_H

//#include <iostream>
#include <string>
#include "fann.h"

class value_ANN_small
{
private:
    struct fann *ann;
public:
    double learning_rate;
    double momentum_rate;

    // 1st constructor
    value_ANN_small(unsigned int num_layers, const unsigned int layers[]);
    // setters
    void set_learning_rate(double learning_rate);
    void set_momentum_rate(double momentum_rate);

    // functions for working with ANN
    float evaluate(fann_type *ann_input);
    void train(fann_type *ann_input, fann_type *desired_output);

    void save_to_file(std::string filename);
};

#endif // VALUE_ANN_SMALL_H
