#include "ga.h"

#define WHOLE_WEIGHT

void ga_init(GA * ga)
{
    ga->last_epoch = 0;
    for(int i = 0; i < POP; i++)
    {
        int num_layers = 3;
        int layers[3] = {FIN_NEURONS1, FIN_NEURONS2, FIN_NEURONS3}; 
        int bias = 1;
        int activation[2] = {ACTIVATION1, ACTIVATION2};
        
        ann_init_fixed(ga->population[i], num_layers, layers, bias, activation);

        ga->scores[i] = 0;
        ga->positions[i] = i;
        ga->mutate = MUTATION;  // chance to mutate (/1000)
    }
}
void ga_init_file(GA * ga, char * folder)
{
    ga->last_epoch = 0;
    for(int i = 0; i < POP; i++)
    {
        int num_layers = 3;
        int layers[3] = {FIN_NEURONS1, FIN_NEURONS2, FIN_NEURONS3}; 
        int bias = 1;
        int activation[2] = {ACTIVATION1, ACTIVATION2};
        
        char filename[100];
        strcpy(filename, folder);
        char file[50];
        sprintf(file, "/top%d.net", i);
        strcat(filename, file);
        ann_init_fixed_file(ga->population[i], num_layers, layers, bias, activation, filename);

        ga->scores[i] = 0;
        ga->positions[i] = i;
    }
}
void ga_destroy(GA * ga)
{
    for(int i = 0; i < POP; i++)
    {
        if(ga->population[i]) free(ga->population[i]);
    }
    if(ga) free(ga);
}

void encode_ann(ANN_F * ann, int gene[GENE_BITS])
{
    int pos = 0;
    for(int i = 0; i < ann->num_layers - 1; i++)
    {
        int num_weights = ann->layers[i] * ann->layers[i + 1] + (ann->bias ? ann->layers[i + 1] : 0);
        for(int j = 0; j < num_weights; j++)
        {
            int gray_code[GRAY_BITS];
            gray_encode(ann->connection[i][j], ann->weights[i][j], gray_code);
            for(int k = 0; k < GRAY_BITS; k++)
            {
                // printf("Accessing position %d of %d\n", pos, GENE_BITS);
                gene[pos++] = gray_code[k];
            }
        }
    }
}
void gray_encode(int connection, fix16_t num, int gray_code[GRAY_BITS])
{
    // shift integer weight values to positive (after conversion, add a 1 to the front if negative)
    int bin_arr[MAX_BINARY];
    int_to_binary(abs(num), bin_arr);

    int pos = 0;
    if(WEIGHT_BIT)
        bin_arr[pos++] = connection;

    if(num < 0 && NEG_BIT)
        bin_arr[pos] = 1;
    else if(NEG_BIT)
        bin_arr[pos] = 0;

    // printf("Binary output: \n");
    // for(int i = 0; i < MAX_BINARY; i++)
    //     printf("%d ", bin_arr[i]);
    // printf("\n");

    for(int i = 0; i < GRAY_BITS; i++)
    {
        if(!i) gray_code[i] = bin_arr[i];
        else gray_code[i] = bin_arr[i - 1] != bin_arr[i] ? 1 : 0;
    }
}
void int_to_binary(int32_t num, int binary[MAX_BINARY])
{
	int pos = 0;
	// printf("Input is: %d, length is: %d\n", num, length);
	int k;
	for(k = 0; k < MAX_BINARY; k++) 
	{ 
		binary[k] = 0; 
	} 

	int i;
	for(i = 0; i < MAX_BINARY; i++)
	{
		if(num <= 0)
		{
			break;
		}
		binary[MAX_BINARY - 1 - pos] = num%2;
		num /= 2;
		pos += 1;
	}
}

void decode_gene(ANN_F * ann, int gene[GENE_BITS])
{
    int pos = 0;
    for(int i = 0; i < ann->num_layers - 1; i++)
    {
        int num_weights = ann->layers[i] * ann->layers[i + 1];
        for(int j = 0; j < num_weights; j++)
        {
            int gray_code[GRAY_BITS];
            for(int k = 0; k < GRAY_BITS; k++)
            {
                gray_code[k] = gene[pos++];
            }
            ann->weights[i][j] = gray_decode(&ann->connection[i][j], gray_code);
        }
    }
}
fix16_t gray_decode(int * connection, int gray_code[GRAY_BITS])
{
    int binary[MAX_BINARY];

    int i;
    for(i = 0; i < MAX_BINARY; i++)
    {
        if(!i) binary[i] = gray_code[i];
        else binary[i] = binary[i - 1] != gray_code[i] ? 1 : 0;
    }

    fix16_t num = 0;
    int count = 0;
    for(i = GRAY_BITS - 1; i > 0; i--)
    {
        num += binary[i] * (int)pow(2, count++);
    }

    count = 0;
    // printf("Binary output: \n");
    // for(int i = 0; i < MAX_BINARY; i++)
    //     printf("%d ", binary[i]);
    // printf("\n");

    if(WEIGHT_BIT)
        *connection = binary[count++];

    if(NEG_BIT)
        num *= binary[count] > 0 ? -1 : 1;
}

void fitness_arrange(GA * ga)
{
    // printf("\nScores before:\n");
    // for(int i = 0; i < POP; i++)
    //     printf("Score %d: %f\n", i, ga->scores[i]);
    // Arrange entire population by order of individual fitness
    for(int i = 0; i < POP; i++)
    {
        for(int j = i + 1; j < POP; j++)
        {
            if(ga->scores[j] > ga->scores[i])
            {
                float temp = ga->scores[j];
                ga->scores[j] = ga->scores[i]; 
                ga->scores[i] = temp;

                temp = ga->positions[j];
                ga->positions[j] = ga->positions[i]; 
                ga->positions[i] = temp;
            }
        }
    }

    if(!ga->last_epoch)
    {
        // Place X random non-fit individuals into top Y
        for(int i = 0; i < (int)(POP * NON_FIT); i++)
        {
            // Find random unfit individual
            int rem_pos = rand() % (int)(POP - POP * 0.4) + (int)(POP * 0.4);  
            int ins_pos = rand() % (int)(POP * 0.4);

            // printf("\nRemoving element from %d and inserting at %d\n", rem_pos, ins_pos);

            int temp_score = ga->scores[rem_pos];
            int temp_pos = ga->positions[rem_pos];
            
            // Shift right and insert
            for(int j = rem_pos; j > ins_pos; j--)
            {
                ga->scores[j] = ga->scores[j - 1];
                ga->positions[j] = ga->positions[j - 1];
            }  
            ga->scores[ins_pos] = temp_score;
            ga->positions[ins_pos] = temp_pos;
        }
    }   

    // printf("\nScores after:\n");
    // for(int i = 0; i < POP; i++)
    //     printf("Score %d: %f\n", i, ga->scores[i]);
}

void breed(GA * ga)
{
    for(int i = 0; i < (int)(POP * 0.2); i++)
    {
        // Parent ANN positions
        int pos1 = ga->positions[2 * i];
        int pos2 = ga->positions[2 * i + 1];
        // Children ANN positions
        int pos3 = ga->positions[(int)(POP * 0.4) + 3 * i];
        int pos4 = ga->positions[(int)(POP * 0.4) + 3 * i + 1];
        int pos5 = ga->positions[(int)(POP * 0.4) + 3 * i + 2];

        int fit_pos = (ga->scores[2 * i] > ga->scores[2 * i + 1] ? pos1 : pos2);
        int not_fit_pos = (ga->scores[2 * i] > ga->scores[2 * i + 1] ? pos2 : pos1);

        int tot_weights = 0;

        #ifdef WHOLE_WEIGHT
        for(int j = 0; j < ga->population[pos1]->num_layers - 1; j++)
        {
            int num_weights = ga->population[pos1]->layers[j] * ga->population[pos1]->layers[j + 1] + 
                            (ga->population[pos1]->bias ? ga->population[pos1]->layers[j + 1] : 0);
            tot_weights += num_weights;
            
            for(int k = 0; k < num_weights; k++)
            {
                fix16_t weight1 = ga->population[pos1]->weights[j][k];
                fix16_t weight2 = ga->population[pos2]->weights[j][k];
                // Child 1 -> random weights from each parent
                ga->population[pos3]->weights[j][k] = (rand()%2 ? weight1 : weight2);

                // Child 2 -> weighted average towards fitter parent
                float weight_non_fit = fix16_to_float(ga->population[not_fit_pos]->weights[j][k]);
                float weight_fit = fix16_to_float(ga->population[fit_pos]->weights[j][k]);

                if(weight_fit > weight_non_fit)
                {
                    float new_weight = weight_fit - (weight_fit - weight_non_fit) / 4.0;
                    ga->population[pos4]->weights[j][k] = fix16_from_float(new_weight);
                }
                else
                {
                    float new_weight = weight_fit + (weight_non_fit - weight_fit) / 4.0;
                    ga->population[pos4]->weights[j][k] = fix16_from_float(new_weight); 
                }

                // Child 3 -> weight is average of both parents
                ga->population[pos5]->weights[j][k] = fix16_div(fix16_add(weight1, weight2), fix16_from_int(2));
            }
        }

        // Mutate child 1
        if(rand() % (1000) < ga->mutate)
        {
            int rand_layer = rand() % (ga->population[pos1]->num_layers - 1);
            int rand_weight = rand() % (ga->population[pos1]->layers[rand_layer] * ga->population[pos1]->layers[rand_layer + 1] + 
                            (ga->population[pos1]->bias ? ga->population[pos1]->layers[rand_layer + 1] : 0));

            ga->population[pos3]->weights[rand_layer][rand_weight] = fix16_add(ga->population[pos3]->weights[rand_layer][rand_weight], 
                                                                                fix16_from_int(rand() % 131072 - 65536));
        }
        // Mutate child 2
        if(rand() % (1000) < ga->mutate)
        {
            int rand_layer = rand() % (ga->population[pos1]->num_layers - 1);
            int rand_weight = rand() % (ga->population[pos1]->layers[rand_layer] * ga->population[pos1]->layers[rand_layer + 1] + 
                            (ga->population[pos1]->bias ? ga->population[pos1]->layers[rand_layer + 1] : 0));

            ga->population[pos4]->weights[rand_layer][rand_weight] = fix16_add(ga->population[pos4]->weights[rand_layer][rand_weight], 
                                                                                fix16_from_int(rand() % 131072 - 65536));
        }
        // Mutate child 3
        if(rand() % (1000) < ga->mutate)
        {
            int rand_layer = rand() % (ga->population[pos1]->num_layers - 1);
            int rand_weight = rand() % (ga->population[pos1]->layers[rand_layer] * ga->population[pos1]->layers[rand_layer + 1] + 
                            (ga->population[pos1]->bias ? ga->population[pos1]->layers[rand_layer + 1] : 0));

            ga->population[pos5]->weights[rand_layer][rand_weight] = fix16_add(ga->population[pos5]->weights[rand_layer][rand_weight], 
                                                                                fix16_from_int(rand() % 131072 - 65536));
        }
        #else
        int parent1[GENE_BITS];
        int parent2[GENE_BITS];

        int child1[GENE_BITS];
        int child2[GENE_BITS]; 
        int child3[GENE_BITS];

        // Gray encode parent neural networks to binary arrays
        encode_ann(ga->population[pos1], parent1);
        encode_ann(ga->population[pos2], parent2);

        // Places of combination for each of the 3 children
        int rand1 = rand() % (int)(0.5 * GENE_BITS);
        int rand2 = rand() % (int)(0.5 * GENE_BITS) + (int)(0.5 * GENE_BITS);
        int rand3 = rand() % (int)(0.75 * GENE_BITS - 0.25 * GENE_BITS) + (int)(0.25 * GENE_BITS);

        // Places of mutation for each child
        int mutation1 = rand() % GENE_BITS;
        int mutation2 = rand() % GENE_BITS;
        int mutation3 = rand() % GENE_BITS;

        if(rand2 == rand1) rand2++;
        if(rand3 == rand2) rand3++;

        // Swap genetic material
        for(int j = 0; j < GENE_BITS; j++)
        {
            if(j < rand1) child1[j] = parent1[j];
            else child1[j] = parent2[j];

            if(j < rand2) child2[j] = parent1[j];
            else child2[j] = parent2[j];

            if(j < rand3) child3[j] = parent1[j];
            else child3[j] = parent2[j];
        }

        // Apply mutations
        child1[mutation1] = child1[mutation1] > 0 ? 0 : 1;
        child2[mutation2] = child2[mutation2] > 0 ? 0 : 1;
        child3[mutation3] = child3[mutation3] > 0 ? 0 : 1;

        // Decode children arrays into "empty" population slots after the fittest
        decode_gene(ga->population[pos3], child1);
        decode_gene(ga->population[pos4], child2);
        decode_gene(ga->population[pos5], child3);
        #endif
    }
}

void ga_run_once(GA * ga)
{
    fitness_function(ga);
    fitness_arrange(ga);
    if(!ga->last_epoch) breed(ga);
}