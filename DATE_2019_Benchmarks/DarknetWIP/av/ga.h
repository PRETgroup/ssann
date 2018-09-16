#include "controller.h" 

#define NEG_BIT     1
#define WEIGHT_BIT  0
#define MAX_BITS    24
#define GRAY_BITS   (MAX_BITS + NEG_BIT + WEIGHT_BIT)
#define MAX_BINARY  GRAY_BITS

#define GA_WEIGHTS  (FIN_NEURONS1 * FIN_NEURONS2 + FIN_NEURONS2 + FIN_NEURONS2 * FIN_NEURONS3 + FIN_NEURONS3)
#define GENE_BITS   (GA_WEIGHTS * GRAY_BITS)

#define POP         100
#define NON_FIT     0.05
#define MUTATION    100

typedef struct{
    ANN_F * population[POP];

    float scores[POP];
    int positions[POP];
    int last_epoch;
    int mutate;
}GA;

void ga_init(GA * ga);
void ga_init_file(GA * ga, char * folder);
void ga_destroy(GA * ga);

void encode_ann(ANN_F * ann, int gene[GENE_BITS]);
void gray_encode(int connection, fix16_t num, int gray_code[GRAY_BITS]);
void int_to_binary(int32_t num, int binary[MAX_BINARY]);

void decode_gene(ANN_F * ann, int gene[GENE_BITS]);
fix16_t gray_decode(int * connection, int gray_code[GRAY_BITS]);

extern void fitness_function(GA * ga);
void fitness_arrange(GA * ga);
void breed(GA * ga);

void ga_run_once(GA * ga);