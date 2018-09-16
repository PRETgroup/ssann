
//This file should be called F_overcurrent.h
//This is autogenerated code. Edit by hand at your peril!

#include <stdint.h>
#include <stdbool.h>

//the dtimer_t type
typedef uint64_t dtimer_t;

//For each policy, we need an enum type for the state machine

enum overcurrent_policy_safe_states { 
	POLICY_STATE_overcurrent_safe_output 
};


//Inputs to the function overcurrent
typedef struct {
	int32_t SoC;
	
} inputs_overcurrent_t;

//Outputs from the function overcurrent
typedef struct {
	bool charge;
	int32_t watts;
	
} outputs_overcurrent_t;

//enforcer state and vars:
typedef struct {
	enum overcurrent_policy_safe_states _policy_safe_state;
	//internal vars
	int32_t max_charge;
	int32_t max_discharge;
	
	
} enforcervars_overcurrent_t;

//This function is provided in "F_overcurrent.c"
//It sets up the variable structures to their initial values
void overcurrent_init_all_vars(enforcervars_overcurrent_t* me, inputs_overcurrent_t* inputs, outputs_overcurrent_t* outputs);

//This function is provided in "F_overcurrent.c"
//It will run the synthesised enforcer and call the controller function
void overcurrent_run_via_enforcer(enforcervars_overcurrent_t* me, inputs_overcurrent_t* inputs, outputs_overcurrent_t* outputs);

//This function is provided from the user
//It is the controller function
extern void overcurrent_run(inputs_overcurrent_t* inputs, outputs_overcurrent_t* outputs);

//enforcer functions


//This function is provided in "F_overcurrent.c"
//It will run the input enforcer for overcurrent's policy safe
void overcurrent_run_input_enforcer_safe(enforcervars_overcurrent_t* me, inputs_overcurrent_t* inputs);

//This function is provided in "F_overcurrent.c"
//It will run the input enforcer for overcurrent's policy safe
void overcurrent_run_output_enforcer_safe(enforcervars_overcurrent_t* me, inputs_overcurrent_t* inputs, outputs_overcurrent_t* outputs);

