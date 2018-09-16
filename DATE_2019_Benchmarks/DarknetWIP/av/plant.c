#include "plant.h"

// #define OUTPUT_PLANT
// #define DEBUG_PLANT

char * img_list = "../data/voc/test.list";
char * label_list = "../data/voc/labels.txt";
// char * cfgfiles[NUM_NETS] = {"data/tmr.cfg", "data/tmr.cfg", "data/tmr.cfg"};

char * objects[4] = {"Nothing", "Person", "Car", "Unknown"};
char * directions[4] = {"Forward", "Right", "Rear", "Left"};

int object_speeds[4] = {-1, 1, 2, 2};

void plant_init(plant * p, int reset, int cnns, int trained)
{
    ensemble_init(reset, cnns, trained);
    ensemble(); // get past first esterel tick
    // printf("Here1\n");
    for(int i = 0; i < CAMERAS; i++) 
    {
        // printf("Here2 %d\n", i);
        // if(!reset && cnns) tmr_init(p->cameras[i], cfgfiles);
        // p->cameras_conf[i] = 0;
        // p->cameras_out[i] = 0;
        p->lidar_in[i] = 0;

        if(i != 1) p->l->objects[i] = -1;
        else p->l->objects[i] = 0;

        p->l->direction[i] = 0;
        p->l->speed[i] = 0;
        if(i < 3) p->new_action[i] = 0;
        if(i < 3) p->action[i] = 0;
        // printf("Here3 %d\n", i);
    }
    // printf("Here4\n");
    p->speed = 1;
    p->prev_speed = 1;
}
void plant_destroy(plant * p, int destroy_cnns)
{
    ensemble_destroy(destroy_cnns);
    // if(destroy_cnns) 
    // {
    //     for(int i = 0; i < CAMERAS; i++)
    //     {
    //         tmr_destroy(p->cameras[i]);
    //         free(p->cameras[i]);
    //     }
    // }
    free(p->l);
    free(p);
}
void plant_destroy_no_cnns(plant * p)
{
    free(p->l);
    free(p);
}

void plant_get_inputs(plant * p)
{
    list *plist = get_paths(img_list);
    char **paths = (char **)list_to_array(plist);
    char **labels = get_labels(label_list);
    // char * paths[3] = {"", "", ""};
    // char * labels[3] = {"nothing", "person", "car"}; 
    for(int i = 0; i < CAMERAS; i++)
    {
        // only fetch "new" input if there is space for one and if it is not in the front zone
        if(((!(rand()%2) && p->l->objects[i] == 0) || p->l->objects[i] == -1) && i != 1)    
        {
            #ifdef DEBUG_PLANT
            printf("--> Generating new object at %d\n", i + 1);
            #endif
            int randnum = rand()%100;
            // printf("Inputs image: %s\n", paths[randnum]);
            strcpy(cameras->in[i], paths[randnum]);
            // printf("Inputs image after: %s\n", cameras->in[i]);

            p->lidar_in[i] = -1;
            char *path = paths[randnum];
            for(int j = 0; j < NUM_CLASSES; ++j){
                if(strstr(path, labels[j])){
                    p->lidar_in[i] = j;
                    break;
                }
            }
        }
        else  // fetch a "new" input of the same type as the last one
        {
            // printf("Using old object at %d\n", i);
            if(i == 1 && p->l->objects[i] == -1)
            {
                p->l->objects[i] = 0;
            }

            do
            {
                int randnum = rand()%100;
                // printf("Inputs image: %s\n", paths[randnum]);
                strcpy(cameras->in[i], paths[randnum]);

                p->lidar_in[i] = -1;
                char *path = paths[randnum];
                for(int j = 0; j < NUM_CLASSES; ++j){
                    if(strstr(path, labels[j])){
                        p->lidar_in[i] = j;
                        break;
                    }
                }
            }
            while(p->lidar_in[i] != p->l->objects[i]);  // keep fetching new inputs until the new object type matches the old
        }
    } 

    free_ptrs((void**)labels, NUM_CLASSES);
    free_ptrs((void**)paths, plist->size);
    free_list(plist);
}
void plant_get_inputs_no_cnns(plant * p)
{
    for(int i = 0; i < CAMERAS; i++)
    {
        if(((!(rand()%2) && p->l->objects[i] == 0) || p->l->objects[i] == -1) && i != 1)    
        {
            int randnum = rand() % 4;
            p->lidar_in[i] = (randnum > 2 ? 2 : randnum);
        }
        else
        {
            if(i == 1 && p->l->objects[i] == -1)
            {
                p->l->objects[i] = 0;
            }
            p->lidar_in[i] = p->l->objects[i];
        }
    }
}

void plant_run_inputs(plant * p, int run_cnns)
{
    // Run ensemble MNNs first
    if(run_cnns)
    {
        ensemble_I_start();
        ensemble();
    }

    for(int i = 0; i < CAMERAS; i++)
    {
        // Run camera ANNs and produce outputs
        // printf("Path: %s\n", p->cameras_in[i]);
        //printf("==== RUNNING CAMERA %d ====\n", i);
        // if(run_cnns)
        // {
        //     predict_tmr_all(p->cameras[i], p->cameras_in[i]);
            // p->cameras_out[i] = p->cameras[i]->topClass;
            // p->cameras_conf[i] = p->cameras[i]->confidence;
        // }

        // Run LiDAR reader and produce outputs
        // only generate new, random data if the object has changed (from undefined to something) or (nothing to something)
        if(p->l->objects[i] != p->lidar_in[i] && i != 1)  // object not directly in front  
        {
            p->l->objects[i] = p->lidar_in[i];
            if(p->l->objects[i] == 1 || p->l->objects[i] == 3)  // person or unknown
            {
                p->l->direction[i] = rand()%4;
                p->l->speed[i] = rand()%2;
            }
            else if(p->l->objects[i] == 2)  // car
            {
                p->l->direction[i] = 0;
                p->l->speed[i] = rand()%3; // create a car  at any speed
                if(p->l->speed[i] > p->speed && i == 3) p->l->speed[i] = p->speed;  // car from behind should not be driving faster
            }
            else if(p->l->objects[i] < 1)
            {
                p->l->speed[i] = -1;
                p->l->direction[i] = -1;
            }
            #ifdef DEBUG_PLANT
            printf("--> Object %d generated with speed %d and direction %d\n", i + 1, p->l->speed[i], p->l->direction[i]);
            #endif
        }  
        else
        {
            if((p->l->objects[i] == 1 || p->l->objects[i] == 3) && p->l->speed[i] != 2)  // person or unknown that is not in a "set" movement
            {
                if(!(rand()%2)) p->l->direction[i] += rand()%4;
                if(!(rand()%2)) p->l->speed[i] = rand()%2;
                p->l->direction[i] = (p->l->direction[i] + 4) % 4;
            }
            else if(p->l->objects[i] == 2)  // car
            {
                if(i == 3 && p->l->speed[i] > p->speed || i == 1 && p->l->speed[i] > p->l->speed[4])  // car behind is moving too fast
                    p->l->speed[i]--;
                else
                    p->l->speed[i] += rand()%3 - 1;

                if(p->l->speed[i] > 2) p->l->speed[i] = 2;
                if(p->l->speed[i] < 0) p->l->speed[i] = 0;
                p->l->direction[i] = 0;
            }
            else if(p->l->objects[i] < 1)
            {
                p->l->speed[i] = -1;
                p->l->direction[i] = -1;
            }
            #ifdef DEBUG_PLANT
            printf("--> Object %d updated has speed %d and direction %d\n", i + 1, p->l->speed[i], p->l->direction[i]);
            #endif
        }
    }
}

void plant_update_speed(plant * p)
{
    p->prev_speed = p->speed;

    // Update plant
    if(p->new_action[0])  // accelerate
    {
        p->speed += 1;
    }
    if(p->new_action[1] || p->new_action[2])  // slow brake 
    {
        p->speed -= 1;
    }
    // if(p->new_action[2])  // fast brake
    // {
    //     p->speed -= 1;
    // }

    if(p->speed > 2) p->speed = 2;
    if(p->speed < 0) p->speed = 0;
}

int plant_run_outputs(plant * p) 
{
    int accident = 0;

    #ifdef OUTPUT_PLANT
    plant_print(p);
    #endif

    // Update car first
    for(int i = 0; i < CAMERAS; i++)
    {
        if(p->l->objects[i] != 0)
        {
            if(p->l->objects[i] == 1 || p->l->objects[i] == 3)  // person or unknown
            { 
                if(i == 0 && p->l->direction[i] == 1 && p->l->speed[i] == 2)  // person moving towards vehicle from ahead left and will have contact
                {
                    if(p->speed || 
                        p->prev_speed > 1 || 
                        p->new_action[0] || 
                        !p->new_action[2] && p->prev_speed > 0 ||
                        !(p->new_action[1] || p->new_action[2]) && p->prev_speed == 0
                    )
                    {
                        // printf("Collision with a pedestrian ahead and to the left of the vehicle\n");
                        accident = 1;
                    }
                }

                // accident with pedestrian in front if vehicle was moving last tick and has not slammed on the brakes
                // accident if vehicle moving
                // accident if vehicle has tried to accelerate
                else if(i == 1 && (
                        p->speed || 
                        p->prev_speed > 1 ||  
                        p->new_action[0] || 
                        !p->new_action[2] && p->prev_speed > 0 ||
                        !(p->new_action[1] || p->new_action[2]) && p->prev_speed == 0
                    ))
                {
                    // printf("Collision with a pedestrian directly ahead of the vehicle\n");
                    accident = 2;
                }
                else if(i == 2 && p->l->direction[i] == 3 && p->l->speed[i] == 2)  // person moving towards vehicle from ahead right and will have contact
                {
                    if(p->speed || 
                        p->prev_speed > 1 || 
                        p->new_action[0] || 
                        !p->new_action[2] && p->prev_speed > 0 ||
                        !(p->new_action[1] || p->new_action[2]) && p->prev_speed == 0
                    )
                    {
                        // printf("Collision with a pedestrian ahead and to the right of the vehicle\n");
                        accident = 3;
                    }
                }
                else if(i == 3 && !p->speed && p->l->speed[i] && !p->l->direction[i])
                {
                    // printf("A pedestrian walked into the back of the vehicle\n");
                    accident = 4;
                }
                // no accident possible with object far ahead
            }
            else  // car
            {
                if(i == 1 && p->speed > p->l->speed[i])
                {
                    // printf("Collision with a vehicle directly ahead of the vehicle\n");
                    accident = 5;
                }

                // if vehicle has slammed on brakes while going at full speed, then accident
                else if(i == 3 && !p->speed && p->l->speed[i] == 2 && p->new_action[2])
                {
                    // printf("Collision with a vehicle directly behind the vehicle\n");
                    accident = 6;
                }
            }
        }
    }

    // Then update environment 
    #ifdef DEBUG_PLANT
    printf("--> Beginning updating of the environment\n");
    #endif
    for(int i = 0; i < CAMERAS; i++)
    {
        if(p->l->objects[i])
        {
            #ifdef DEBUG_PLANT
            printf("--> Updating object %d", i + 1);
            #endif
            if(p->l->objects[i] == 1 || (p->l->objects[i] == 4 && rand()%2))  // update person
            {
                #ifdef DEBUG_PLANT
                printf(" (%s)\n", objects[p->l->objects[i]]);
                #endif
                switch(i)
                {
                    case 0:
                        if(p->l->speed[i])
                        {
                            if(p->l->direction[i] != 1)  // walking away from road
                            {
                                make_nothing(p->l, i);
                            }
                            else if(p->l->direction[i] == 1)  // walking towards road
                            {
                                if(p->l->speed[i] == 2)  // already in road
                                    make_nothing(p->l, i);
                                else    // still off the road
                                    p->l->speed[i] = 2;  // make them be in the road
                            }
                        }
                        else
                        {
                            make_nothing(p->l, i);
                        }
                        break;
                    case 4:
                        if((p->l->direction[i] == 1 || p->l->direction[i] == 3) && p->l->speed[i])  // walking away from road
                        {
                            make_nothing(p->l, i);
                        }
                        else
                        {
                            if(!p->l->objects[1])  // open space in front of the car
                            {
                                #ifdef DEBUG_PLANT
                                printf("--> Object %d is being moved to position %d\n", i + 1, 2);
                                #endif
                                p->l->objects[1] = 1;
                                p->l->direction[1] = p->l->direction[i];
                                p->l->speed[1] = p->l->speed[i];
                                make_nothing(p->l, i);
                            }
                            else if(p->l->objects[1] == 1)
                            {
                                make_nothing(p->l, i);
                            }
                        }
                        break;
                    case 1:
                        if(p->l->speed[i])
                        {
                            if(p->l->direction[i] == 1 || p->l->direction[i] == 3)  // walking off the road
                            {
                                make_nothing(p->l, i);
                            }
                        }
                        break;
                    case 2:
                        if(p->l->speed[i])
                        {
                            if(p->l->direction[i] != 3)  // walking away from road
                            {
                                make_nothing(p->l, i);
                            }
                            else if(p->l->direction[i] == 3)  // walking towards road
                            {
                                if(p->l->speed[i] == 2)  // already in road
                                    make_nothing(p->l, i);
                                else    // still off the road
                                    p->l->speed[i] = 2;  // make them be in the road
                            }
                        }
                        else
                        {
                            make_nothing(p->l, i);
                        }
                        break;
                    case 3:
                        if(!(p->l->speed[i] > p->speed && p->l->direction[i] == 0))
                            make_nothing(p->l, i);
                        break;
                }
            }
            else if(p->l->objects[i] == 2 || p->l->objects[i] == 4)  // update car
            {
                #ifdef DEBUG_PLANT
                printf(" (%s)\n", objects[p->l->objects[i]]);
                #endif
                switch(i)
                {
                    case 0:
                        if(p->l->speed[i] != p->speed)
                        {
                            make_nothing(p->l, i);
                        }
                        break;
                    case 4:
                        if(p->l->speed[i] < p->speed)
                        {
                            if(!p->l->objects[1])  // if slower than this vehicle and space in front, make it take the space
                            {
                                #ifdef DEBUG_PLANT
                                printf("--> Object %d is being moved to position %d\n", i + 1, 2);
                                #endif
                                p->l->objects[1] = 2;
                                p->l->direction[1] = 0;
                                p->l->speed[1] = p->l->speed[i];
                                make_nothing(p->l, i);
                            }
                        }
                        else if(p->l->speed[i] > p->speed)
                        {
                            make_nothing(p->l, i);
                        }
                        break;
                    case 1:
                        if(p->l->speed[i] > p->speed)  // going faster than this car
                        {
                            if(!p->l->objects[4])  // nothing in front -> make this car in front
                            {
                                #ifdef DEBUG_PLANT
                                printf("--> Object %d is being moved to position %d\n", i + 1, 5);
                                #endif
                                p->l->objects[4] = 2;
                                p->l->direction[4] = 0;
                                p->l->speed[4] = p->l->speed[i];
                                make_nothing(p->l, i);
                            }
                            else
                            {
                                make_nothing(p->l, i);
                            }
                        }
                        else
                        {
                            make_nothing(p->l, i);  //  it changes lanes
                        }
                        break;
                    case 2:
                        if(p->l->speed[i] != p->speed)
                        {
                            make_nothing(p->l, i);
                        }
                        break;
                    case 3:
                        if(!p->l->speed[i] || p->l->speed[i] < p->speed) // not moving or going slower than this car
                        {
                            make_nothing(p->l, i);
                        }
                        break;
                }
            }
            else
            {
                #ifdef DEBUG_PLANT
                printf(" Nothing\n");
                #endif
            }
        }
    }

    // update action array
    for(int i = 0; i < 3; i++)
    {
        p->action[i] = p->new_action[i];
    }

    return accident;
}

void make_nothing(lidar * l, int i)
{
    #ifdef DEBUG_PLANT
    printf("--> Object %d is being removed from the environment\n", i + 1);
    #endif
    l->objects[i] = 0;
    l->speed[i] = -1;
    l->direction[i] = -1;
}

// Display the plant with inputs
void plant_print(plant * p)
{
    printf("\n====== DISPLAYING THE ENVIRONMENT ======\n\n");

    printf("Car is driving at speed %d, previously driving at speed %d\n", p->speed, p->prev_speed);
    printf("Car is undertaking the following action(s):\n");
    printf("ACC: %d, SB: %d, FB: %d\n\n", p->new_action[0], p->new_action[1], p->new_action[2]);
    // for(int i = 0; i < 8; i++)
    //     printf("LiDAR: %d\n", p->l->objects[i]);

    // Print front camera
    printf("\t\t\t\tObject5\n");

    printf("\t\t\t\tActual (LiDAR): %s\n", objects[p->l->objects[4]]);

    printf("\t\t\t\tCNNs: %s (%d confident)", objects[cameras->out[4]], cameras->conf[4]);
    if(cameras->out[4] == 2 && cameras->conf[4] < 10) printf("\t");
    printf("\n");

    printf("\t\t\t\tDirection: %s\t\t", p->l->direction[4] > -1 ? directions[p->l->direction[4]] : "Unknown");
    if(p->l->direction[4] > 1) printf("\t");
    printf("\n");

    printf("\t\t\t\tSpeed: %d \n", p->l->speed[4]);
    // End print front camera

    printf("\n\n");

    // Print top row
    printf("Object 1\t\t\tObject2\t\t\t\tObject3\n");

    printf("Actual (LiDAR): %s\t\t", objects[p->l->objects[0]]);
    printf("Actual (LiDAR): %s\t\t", objects[p->l->objects[1]]);
    printf("Actual (LiDAR): %s\t\t\n", objects[p->l->objects[2]]);

    printf("CNNs: %s (%d confident)", objects[cameras->out[0]], cameras->conf[0]);
    if(cameras->out[0] == 2 && cameras->conf[0] < 10) printf("\t");
    printf("\tCNNs: %s (%d confident)", objects[cameras->out[1]], cameras->conf[1]);
    if(cameras->out[1] == 2 && cameras->conf[1] < 10) printf("\t");
    printf("\tCNNs: %s (%d confident)", objects[cameras->out[2]], cameras->conf[2]);
    if(cameras->out[2] == 2 && cameras->conf[2] < 10) printf("\t");
    printf("\n");

    printf("Direction: %s\t\t", p->l->direction[0] > -1 ? directions[p->l->direction[0]] : "Unknown");
    if(p->l->direction[0] > 1) printf("\t");
    printf("Direction: %s\t\t", p->l->direction[1] > -1 ? directions[p->l->direction[1]] : "Unknown");
    if(p->l->direction[1] > 1) printf("\t");
    printf("Direction: %s\t\t", p->l->direction[2] > -1 ? directions[p->l->direction[2]] : "Unknown");
    if(p->l->direction[2] > 1) printf("\t");
    printf("\n");

    printf("Speed: %d \t\t\t", p->l->speed[0]);
    printf("Speed: %d \t\t\t", p->l->speed[1]);
    printf("Speed: %d \t\t\t\n", p->l->speed[2]);

    // End print top row

    printf("\n\n");

    // Print middle row
    printf("\t\t\t\tCAR\tCAR\tCAR\tCAR\n");
    printf("\t\t\t\tCAR\t\t\tCAR\t\n");
    printf("\t\t\t\tCAR\tSPEED:\t%d \tCAR\t\n", p->speed);
    printf("\t\t\t\tCAR\t\t\tCAR\t\n");
    printf("\t\t\t\tCAR\tCAR\tCAR\tCAR\n");
    // End print middle row

    printf("\n\n");

    // Print bottom row
    printf("\t\t\t\tObject4\n");

    printf("\t\t\t\tActual (LiDAR): %s\n", objects[p->l->objects[3]]);

    printf("\t\t\t\tCNNs: %s (%d confident)", objects[cameras->out[3]], cameras->conf[3]);
    if(cameras->out[3] == 2 && cameras->conf[3] < 10) printf("\t");
    printf("\n");

    printf("\t\t\t\tDirection: %s\t\t", p->l->direction[3] > -1 ? directions[p->l->direction[3]] : "Unknown");
    if(p->l->direction[3] > 1) printf("\t");
    printf("\n");

    printf("\t\t\t\tSpeed: %d \n", p->l->speed[3]);
    // End print bottom row

    printf("\n================= DONE =================\n\n");
}

