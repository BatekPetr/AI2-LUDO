#include "value_ann_small.h"

value_ANN_small::value_ANN_small(unsigned int num_layers, const unsigned int layers[])
{
    this->ann = fann_create_standard_array(num_layers, layers);

    fann_set_activation_function_hidden(this->ann, FANN_SIGMOID_SYMMETRIC );
    fann_set_activation_function_output(this->ann, FANN_LINEAR);
    fann_randomize_weights(this->ann, -1, 1);
}


void value_ANN_small::set_learning_rate(double learning_rate)
{
    this->learning_rate = learning_rate;
    fann_set_learning_rate(this->ann, learning_rate);
}

void value_ANN_small::set_momentum_rate(double momentum_rate)
{
    this->momentum_rate = momentum_rate;
    fann_set_learning_momentum(this->ann, momentum_rate);
}

float value_ANN_small::evaluate(fann_type *ann_input)
{
    float *V = fann_run(this->ann, ann_input);
    return *V;
}

void value_ANN_small::train(fann_type *ann_input, fann_type *desired_output)
{
    // Update weights in ANN
    fann_train(this->ann, ann_input, desired_output);
}
void value_ANN_small::save_to_file(std::string filename)
{
    fann_save(this->ann, filename.c_str());
}
