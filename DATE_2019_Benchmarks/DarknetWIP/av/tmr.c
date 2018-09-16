#include "tmr.h"

void tmr_init(tmr *nets, char *tmr_cfg[NUM_NETS], int trained)
{
    for(int i = 0; i < NUM_NETS; i++)
    {
        if(trained >= 0)
        {
            char cnn_weights[255];
            sprintf(cnn_weights, "backup/CNN_weights/tmr%d_%d.weights", trained, i + 1);
            nets->cnn[i] = load_network(tmr_cfg[i], cnn_weights, 0);
        }
        else
        {
            nets->cnn[i] = load_network(tmr_cfg[i], NULL, 0);
        }
    }
}

void tmr_destroy(tmr *nets)
{
    for(int i = 0; i < NUM_NETS; i++)
    {
        destroy_network(nets->cnn[i]);
    }
}
void destroy_network(network *net)
{
    int i;
    for(i = 0; i < net->n; ++i){
        if(net->layers[i].loss) free(net->layers[i].loss); // softmax doesn't unallocate loss
        free_layer(net->layers[i]);
    }
    free(net->layers);
    free(net->input);
    free(net->truth);
    free(net->seen);
    free(net->cost);
    free(net->t);
    free(net->workspace);
    free(net);
}

void predict_tmr(tmr *nets, int net, char *filename, int full)
{
    // if(full) printf("======= Starting prediction process =======\n\n");
    // else printf("Predicting...\n    ");
    // net = load_network(cfgfile, weightfile, 0);  // Don't load weights because we want it to not work
    set_batch_network(nets->cnn[net], 1);

    int i = 0;
    char **names = get_labels("../data/voc/labels.txt"); 
    clock_t time;
    int *indexes = calloc(TOPK, sizeof(int));

    image im = load_image_stb(filename, 3);
    time=clock();
    float *predictions = network_predict(nets->cnn[net], im.data);
    top_k(predictions, NUM_CLASSES, TOPK, indexes);


    // printf("(scores -> ");
    for(int i = 0; i < NUM_CLASSES; i++)
    {
        nets->classScores[net][i] = predictions[i];
        // printf("%0.2f ", nets->classScores[net][i]);
    }
    // printf("\n");
    for(int i = 0; i < TOPK; i++)
    {
        nets->indexes[net][i] = indexes[i];
    }

    free_ptrs((void**)names, NUM_CLASSES);
    free_image(im);
    free(indexes);
    // if(full) printf("\n======= Ending prediction process =======\n");
    // else printf("...done!\n");
}

void predict_tmr_all(tmr *nets, char * filename)
{ 
    // printf("Predicted classes: ");
    for(int i = 0; i < NUM_NETS; i++)
    {
        predict_tmr(nets, i, filename, 0);
        // printf("%d\t", nets->indexes[i][0]);
    }
    // printf("\n");


    int *indexest = calloc(TOPK, sizeof(int));
    int *indexes_avg = calloc(TOPK, sizeof(int));

    // GENERAL AVERAGING OF SCORES
    float pred[NUM_CLASSES];
    for(int i = 0; i < NUM_CLASSES; i++)
    {
        pred[i] = 0;
        for(int j = 0; j < NUM_NETS; j++)
        {
            pred[i] += nets->classScores[j][i];
        }
        // printf("Predicted average = (%f * %f * %f) / 3 = %f\n", pred1[j], pred2[j], pred3[j], pred[j]);
    }
    top_k(pred, NUM_CLASSES, TOPK, indexest);  // top k for added output

    // CUSTOM AVERAGING METHOD WITH GENERAL AVERAGE INCLUDED
    float class_scores_avg[NUM_CLASSES]; 
    for(int i = 0; i < NUM_NETS; i++)
    {
        class_scores_avg[nets->indexes[i][0]] += 1.01;
        class_scores_avg[nets->indexes[i][1]] += 0.5;
    }
    class_scores_avg[indexest[0]] += 1.01;
    class_scores_avg[indexest[1]] += 0.5;

    // printf("Class scores: %f, %f, %f, %f, %f, %f, %f\n", class_scores[0], class_scores[1], class_scores[2], 
    //         class_scores[3], class_scores[4], class_scores[5], class_scores[6]);

    float topk_avg[TOPK];
    for(int i = 0; i < TOPK; i++)
    {
        topk_avg[i] = 0;
    }
    for(int j = 0; j < NUM_CLASSES; j++)
    {
        int found = 0;
        for(int i = 0; i < TOPK; i++)
        {
            if(topk_avg[i] < class_scores_avg[j] && !found)
            {
                found = 1;

                for(int k = TOPK - 1; k > i; k--)
                {
                    topk_avg[k] = topk_avg[k - 1];
                    indexes_avg[k] = indexes_avg[k - 1];
                } 
                
                topk_avg[i] = class_scores_avg[j];
                indexes_avg[i] = j;
            }
        }
    }

    for(int i = 0; i < TOPK - 1; i++)
    {
        if(topk_avg[0] == topk_avg[i] && pred[indexes_avg[0]] < pred[indexes_avg[i]])
        {
            float temp = indexes_avg[0];
            indexes_avg[0] = indexes_avg[i];
            indexes_avg[i] = temp;
        }
    }
    // END CUSTOM AVERAGING

    nets->topClass = indexes_avg[0];

    if((float)pred[indexes_avg[1]] + (float)pred[indexes_avg[0]] != 0)
        nets->confidence = (float)pred[indexes_avg[0]] / ((float)pred[indexes_avg[1]] + (float)pred[indexes_avg[0]]) * 100.0;
    else nets->confidence = 0;

    // printf("Final class: %d (%d confidence)\n", nets->topClass, nets->confidence);

    free(indexes_avg);
    free(indexest);
}

