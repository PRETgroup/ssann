
//This file should be called F_overcurrent.c
//This is autogenerated code. Edit by hand at your peril!
#include "F_overcurrent.h"

void overcurrent_init_all_vars(enforcervars_overcurrent_t* me, inputs_overcurrent_t* inputs, outputs_overcurrent_t* outputs) {
	//set any input vars with default values
	
	//set any output vars with default values
	outputs->charge = 0;
	outputs->watts = 0;
	

	
	me->_policy_safe_state = POLICY_STATE_overcurrent_safe_output;
	//input policy internal vars
	
	me->max_charge = 200;
	
	me->max_discharge = 150;
	
	
}

void overcurrent_run_via_enforcer(enforcervars_overcurrent_t* me, inputs_overcurrent_t* inputs, outputs_overcurrent_t* outputs) {
	//run the policies in reverse order for the inputs (last policies have highest priority)
	
	 overcurrent_run_input_enforcer_safe(me, inputs);
	

	overcurrent_run(inputs, outputs);

	//run policies in specified order for outputs
	overcurrent_run_output_enforcer_safe(me, inputs,outputs);
	
}


//input policies

//INPUT POLICY safe BEGIN
//This will run the input enforcer for overcurrent's policy safe
void overcurrent_run_input_enforcer_safe(enforcervars_overcurrent_t* me, inputs_overcurrent_t* inputs) {
	switch(me->_policy_safe_state) {
		case POLICY_STATE_overcurrent_safe_output:
			
			
			break;

		
	}
}

//INPUT POLICY safe END



//output policies

//OUTPUT POLICY safe BEGIN
//This will run the input enforcer for overcurrent's policy safe
void overcurrent_run_output_enforcer_safe(enforcervars_overcurrent_t* me, inputs_overcurrent_t* inputs, outputs_overcurrent_t* outputs) {
	//advance timers
	
	
	//run enforcer
	switch(me->_policy_safe_state) {
		case POLICY_STATE_overcurrent_safe_output:
			
			if(outputs->watts < 0) {
				//transition output -> violation on watts < 0
				//select a transition to solve the problem
				
				//Recovery instructions manually provided.
				outputs->watts = 0;
				
			} 
			if(outputs->charge && outputs->watts > me->max_charge) {
				//transition output -> violation on charge and watts > max_charge
				//select a transition to solve the problem
				
				//Recovery instructions manually provided.
				outputs->watts = me->max_charge;
				
			} 
			if( !outputs->charge && outputs->watts > me->max_discharge) {
				//transition output -> violation on not (charge) and watts > max_discharge
				//select a transition to solve the problem
				
				//Recovery instructions manually provided.
				outputs->watts = me->max_discharge;
				
			} 

			break;

		
	}

	//select transition to advance state
	switch(me->_policy_safe_state) {
		case POLICY_STATE_overcurrent_safe_output:
			
			if(outputs->watts > 0 && outputs->charge && outputs->watts <= me->max_charge) {
				//transition output -> output on watts > 0 and charge and watts <= max_charge
				me->_policy_safe_state = POLICY_STATE_overcurrent_safe_output;
				//set expressions
				
			} 
			if(outputs->watts > 0 && !outputs->charge && outputs->watts <= me->max_discharge) {
				//transition output -> output on watts > 0 and not (charge) and watts <= max_discharge
				me->_policy_safe_state = POLICY_STATE_overcurrent_safe_output;
				//set expressions
				
			} 
			
			break;

		
	}
}

//OUTPUT POLICY safe END



