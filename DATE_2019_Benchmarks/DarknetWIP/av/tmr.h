#include <sys/time.h>
#include <assert.h>

#include "../darknet/include/darknet.h"

#define NUM_CLASSES             3
#define TOPK                    2
#define NUM_NETS                3
#define MAX_BOUNDS              2
#define NUM_VALID               3

// ensemble struct
typedef struct{
    network *cnn[NUM_NETS];
    int indexes[NUM_NETS][TOPK];
    float classScores[NUM_NETS][NUM_CLASSES];
    int topClass;
    int confidence;
}tmr;

void tmr_init(tmr *nets, char *tmr_cfg[NUM_NETS], int trained);

void tmr_destroy(tmr *nets);

void destroy_network(network *net);

void predict_tmr(tmr *nets, int net, char *filename, int full);

void predict_tmr_all(tmr *nets, char * filename);
