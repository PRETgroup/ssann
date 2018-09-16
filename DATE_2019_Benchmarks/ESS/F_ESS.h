
//This file should be called F_ESS.h
//This is autogenerated code. Edit by hand at your peril!

#include <stdint.h>
#include <stdbool.h>

//the dtimer_t type
typedef uint64_t dtimer_t;

//For each policy, we need an enum type for the state machine

enum ESS_policy_safe_states { 
	POLICY_STATE_ESS_safe_output 
};


//Inputs to the function ESS
typedef struct {
	int32_t SoC;
	
} inputs_ESS_t;

//Outputs from the function ESS
typedef struct {
	bool charge;
	int32_t watts;
	
} outputs_ESS_t;

//enforcer state and vars:
typedef struct {
	enum ESS_policy_safe_states _policy_safe_state;
	//internal vars
	int32_t max_charge;
	int32_t max_discharge;
	int32_t max_capacity;
	int32_t min_capacity;
	
	
} enforcervars_ESS_t;

//This function is provided in "F_ESS.c"
//It sets up the variable structures to their initial values
void ESS_init_all_vars(enforcervars_ESS_t* me, inputs_ESS_t* inputs, outputs_ESS_t* outputs);

//This function is provided in "F_ESS.c"
//It will run the synthesised enforcer and call the controller function
void ESS_run_via_enforcer(enforcervars_ESS_t* me, inputs_ESS_t* inputs, outputs_ESS_t* outputs);

//This function is provided from the user
//It is the controller function
extern void ESS_run(inputs_ESS_t* inputs, outputs_ESS_t* outputs);

//enforcer functions


//This function is provided in "F_ESS.c"
//It will run the input enforcer for ESS's policy safe
void ESS_run_input_enforcer_safe(enforcervars_ESS_t* me, inputs_ESS_t* inputs);

//This function is provided in "F_ESS.c"
//It will run the input enforcer for ESS's policy safe
void ESS_run_output_enforcer_safe(enforcervars_ESS_t* me, inputs_ESS_t* inputs, outputs_ESS_t* outputs);
