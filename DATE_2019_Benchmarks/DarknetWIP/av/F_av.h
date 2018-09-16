
//This file should be called F_av.h
//This is autogenerated code. Edit by hand at your peril!

#include <stdint.h>
#include <stdbool.h>

//the dtimer_t type
typedef uint64_t dtimer_t;

//For each policy, we need an enum type for the state machine

enum av_policy_brake_states { 
	POLICY_STATE_av_brake_cruise, 
	POLICY_STATE_av_brake_brake 
};


//Inputs to the function av
typedef struct {
	int32_t lobject0;
	int32_t lobject1;
	int32_t lobject2;
	int32_t lobject3;
	int32_t lobject4;
	int32_t cobject0;
	int32_t cobject1;
	int32_t cobject2;
	int32_t cobject3;
	int32_t cobject4;
	int32_t conf0;
	int32_t conf1;
	int32_t conf2;
	int32_t conf3;
	int32_t conf4;
	int32_t speed0;
	int32_t speed1;
	int32_t speed2;
	int32_t speed3;
	int32_t speed4;
	int32_t dir0;
	int32_t dir1;
	int32_t dir2;
	int32_t dir3;
	int32_t dir4;
	int32_t speed;
	int32_t prev_speed;
	
} inputs_av_t;

//Outputs from the function av
typedef struct {
	bool accelerate;
	bool slow_brake;
	bool fast_brake;
	bool cruise;
	
} outputs_av_t;

//enforcer state and vars:
typedef struct {
	enum av_policy_brake_states _policy_brake_state;
	//internal vars
	dtimer_t t;
	int32_t limit;
	
	
} enforcervars_av_t;

//This function is provided in "F_av.c"
//It sets up the variable structures to their initial values
void av_init_all_vars(enforcervars_av_t* me, inputs_av_t* inputs, outputs_av_t* outputs);

//This function is provided in "F_av.c"
//It will run the synthesised enforcer and call the controller function
void av_run_via_enforcer(enforcervars_av_t* me, inputs_av_t* inputs, outputs_av_t* outputs);

//This function is provided from the user
//It is the controller function
extern void av_run(inputs_av_t* inputs, outputs_av_t* outputs);

//enforcer functions


//This function is provided in "F_av.c"
//It will run the input enforcer for av's policy brake
void av_run_input_enforcer_brake(enforcervars_av_t* me, inputs_av_t* inputs);

//This function is provided in "F_av.c"
//It will run the input enforcer for av's policy brake
void av_run_output_enforcer_brake(enforcervars_av_t* me, inputs_av_t* inputs, outputs_av_t* outputs);
