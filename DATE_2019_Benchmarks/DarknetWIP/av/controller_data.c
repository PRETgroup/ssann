#include "controller.h"

#define TOP_NET     0

void controller_init(int trained)
{    
    for(int i = 0; i < 20; i++)
    {
        if(i < BACK_NEURONS1) ctrl->back_in[i] = 0; 
        if(i < FRONT_NEURONS1) ctrl->front_in[i] = 0;
        if(i < FIN_NEURONS3) ctrl->decision[i] = 0;
        if(i < FIN_EXTERNAL) ctrl->fin_in[i] = 0;
    }
    
    int num_layers = 3;
    int layers[3] = {FIN_NEURONS1, FIN_NEURONS2, FIN_NEURONS3}; 
    int bias = 1;
    int activation[2] = {ACTIVATION1, ACTIVATION2};
    
    char weights[255];
    sprintf(weights, "backup/ANN10000/epoch%d_top%d.txt", trained, TOP_NET);

    printf("Initialising with weight file: %s\n", weights);

    if(trained >= 0)
        ann_init_fixed_file(fin, num_layers, layers, bias, activation, weights);
    else
        ann_init_fixed(fin, num_layers, layers, bias, activation);

    // Tick Esterel once to initialise it
    controller();
}

void controller_destroy()
{
    // free(front);
    // free(sides);
    // free(back);
    // free(fin);
    // free(ctrl);
}

void controller_run_tick()
{
    controller();
}

void run_fin()
{
    fix16_t inputs[FIN_NEURONS1]; 
    fix16_t outputs[FIN_NEURONS3];
    for(int i = 0; i < FIN_EXTERNAL; i++)
    {
        inputs[i] = fix16_from_float(ctrl->fin_in[i]);
        if(inputs[i] < 0)
            inputs[i] = 0;
    }
    for(int i = 0; i < FRONT_NEURONS1; i++)
    {
        inputs[i + FIN_EXTERNAL] = fix16_from_float(ctrl->front_in[i]);
        if(inputs[i] < 0)
            inputs[i] = 0;
    }
    for(int i = 0; i < BACK_NEURONS1; i++)
    {
        inputs[i + FIN_EXTERNAL + FRONT_NEURONS1] = fix16_from_float(ctrl->back_in[i]);
        if(inputs[i] < 0)
            inputs[i] = 0;
    }

    // printf("Controller inputs:\n");
    // for(int i = 0; i < FIN_NEURONS1; i++)
    //     printf(" %0.2f", fix16_to_float(inputs[i]));
    // printf("\n");

    ann_run_fixed(inputs, outputs, fin);

    // printf("Controller outputs:\n");
    // for(int i = 0; i < FIN_NEURONS3; i++)
    //     printf(" %0.2f", fix16_to_float(outputs[i]));
    // printf("\n");

    // printf("ANN outputs:\n");
    // for(int i = 0; i < FIN_NEURONS3; i++)
    // {
    //     printf(" %0.2f", fix16_to_float(outputs[i]));
    //     // ctrl->decision[i] = (fix16_to_float(outputs[i]) > 0.5 ? 1 : 0);
    // }
    // printf("\n");

    // only highest output becomes chosen action
    int max = 0;
    int pos = 0;
    int min_count = 0;
    for(int i = 0; i < FIN_NEURONS3; i++)
    {
        if(fix16_to_float(outputs[i]) < MIN_OUTPUT)
            min_count++;
        if(outputs[i] > max)
        {
            max = outputs[i];
            pos = i;
        }
    }
    for(int i = 0; i < FIN_NEURONS3; i++)
    {
        ctrl->decision[i] = 0;
    }
    if(min_count < FIN_NEURONS3)
        ctrl->decision[pos] = 1;

    // printf("Final ANN outputs:\n");
    // for(int i = 0; i < FIN_NEURONS3; i++)
    // {
    //     printf(" %d", ctrl->decision[i]);
    // }
    // printf("\n");

    // printf("\n");
}