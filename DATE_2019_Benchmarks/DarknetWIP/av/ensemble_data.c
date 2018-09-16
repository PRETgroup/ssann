#include "ensemble.h"

char * cfgfiles[NUM_NETS] = {"data/tmr.cfg", "data/tmr.cfg", "data/tmr.cfg"};

void ensemble_init(int reset, int cnns, int trained)
{
    if(!reset && cnns)
    {
        cameras = (ensembles *)malloc(sizeof(ensembles));
        if(cameras == 0)
        {
            printf("ERROR: Out of memory: cameras\n");
            return;
        }
        for(int i = 0; i < CAMERAS; i++)
        {
            cameras->cnns[i] = (tmr *)malloc(sizeof(tmr)); 
            if(cameras->cnns[i] == 0)
            {
                printf("ERROR: Out of memory: cameras->cnns[i]\n");
                return;
            }
            if(!reset && cnns) tmr_init(cameras->cnns[i], cfgfiles, trained);
            cameras->conf[i] = 0;
            cameras->out[i] = 0;
        }
    }
}

void run_cnn(int ensemble, int net)
{
    // printf("Inputs image #%d: %s\n", 0, cameras->in[0]);
    // printf("Inputs image #%d: %s\n", 1, cameras->in[1]);
    // printf("Inputs image #%d: %s\n", 2, cameras->in[2]);
    // printf("Inputs image #%d: %s\n", 3, cameras->in[3]);
    // printf("Inputs image #%d: %s\n", 4, cameras->in[4]);
    predict_tmr(cameras->cnns[ensemble], net, cameras->in[ensemble], 0);
}

void run_ensemble(int ensemble)
{
    int *indexest = calloc(TOPK, sizeof(int));
    int *indexes_avg = calloc(TOPK, sizeof(int));

    // GENERAL AVERAGING OF SCORES
    float pred[NUM_CLASSES];
    for(int i = 0; i < NUM_CLASSES; i++)
    {
        pred[i] = 0;
        for(int j = 0; j < NUM_NETS; j++)
        {
            pred[i] += cameras->cnns[ensemble]->classScores[j][i];
        }
        // printf("Predicted average = (%f * %f * %f) / 3 = %f\n", pred1[j], pred2[j], pred3[j], pred[j]);
    }
    top_k(pred, NUM_CLASSES, TOPK, indexest);  // top k for added output
    // printf("Class predictions: %f, %f, %f\n", cameras->cnns[ensemble]->classScores[0][0], cameras->cnns[ensemble]->classScores[0][1], cameras->cnns[ensemble]->classScores[0][2]);

    // CUSTOM AVERAGING METHOD WITH GENERAL AVERAGE INCLUDED
    float class_scores_avg[NUM_CLASSES]; 
    for(int i = 0; i < NUM_CLASSES; i++)
    {
        class_scores_avg[i] = 0;
    }
    for(int i = 0; i < NUM_NETS; i++)
    {
        class_scores_avg[cameras->cnns[ensemble]->indexes[i][0]] += 1.01;
        class_scores_avg[cameras->cnns[ensemble]->indexes[i][1]] += 0.5;
    }
    class_scores_avg[indexest[0]] += 1.01;
    class_scores_avg[indexest[1]] += 0.5;

    // printf("Class scores: %f, %f, %f\n", class_scores_avg[0], class_scores_avg[1], class_scores_avg[2]);

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

    // printf("Indexes: %d, %d, %d\n", indexes_avg[0], indexes_avg[1], indexes_avg[2]);

    cameras->cnns[ensemble]->topClass = indexes_avg[0];

    if((float)pred[indexes_avg[1]] + (float)pred[indexes_avg[0]] != 0)
        cameras->cnns[ensemble]->confidence = (float)pred[indexes_avg[0]] / ((float)pred[indexes_avg[1]] + (float)pred[indexes_avg[0]]) * 100.0;
    else cameras->cnns[ensemble]->confidence = 0;

    // printf("Final class: %d (%d confidence)\n", cameras->cnns[ensemble]->topClass, cameras->cnns[ensemble]->confidence);

    cameras->out[ensemble] = cameras->cnns[ensemble]->topClass;
    cameras->conf[ensemble] = cameras->cnns[ensemble]->confidence;

    free(indexes_avg);
    free(indexest);
}

void ensemble_destroy(int destroy_cnns)
{
    if(destroy_cnns) 
    {
        for(int i = 0; i < CAMERAS; i++)
        {
            tmr_destroy(cameras->cnns[i]);
            free(cameras->cnns[i]);
        }
    }
}