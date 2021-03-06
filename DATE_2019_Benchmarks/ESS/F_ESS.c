
//This file should be called F_ESS.c
//This is autogenerated code. Edit by hand at your peril!
#include "F_ESS.h"

void ESS_init_all_vars(enforcervars_ESS_t* me, inputs_ESS_t* inputs, outputs_ESS_t* outputs) {
	//set any input vars with default values
	
	//set any output vars with default values
	outputs->charge = 0;
	outputs->watts = 0;
	

	
	me->_policy_safe_state = POLICY_STATE_ESS_safe_output;
	//input policy internal vars
	
	me->max_charge = 200;
	
	me->max_discharge = 150;
	
	me->max_capacity = (589*60);
	
	me->min_capacity = (155*60);
	
	
}

void ESS_run_via_enforcer(enforcervars_ESS_t* me, inputs_ESS_t* inputs, outputs_ESS_t* outputs) {
	//run the policies in reverse order for the inputs (last policies have highest priority)
	
	 ESS_run_input_enforcer_safe(me, inputs);
	

	ESS_run(inputs, outputs);

	//run policies in specified order for outputs
	ESS_run_output_enforcer_safe(me, inputs,outputs);
	
}


//input policies

//INPUT POLICY safe BEGIN
//This will run the input enforcer for ESS's policy safe
void ESS_run_input_enforcer_safe(enforcervars_ESS_t* me, inputs_ESS_t* inputs) {
	switch(me->_policy_safe_state) {
		case POLICY_STATE_ESS_safe_output:
			
			if(inputs->SoC < me->min_capacity) {
				//transition output -> violation on SoC < min_capacity
				//select a transition to solve the problem
				
				//Recovery instructions manually provided.
				
			} 
			if(inputs->SoC < me->min_capacity) {
				//transition output -> violation on SoC < min_capacity
				//select a transition to solve the problem
				
				//Recovery instructions manually provided.
				
			} 
			if(inputs->SoC > me->max_capacity) {
				//transition output -> violation on SoC > max_capacity
				//select a transition to solve the problem
				
				//Recovery instructions manually provided.
				
			} 
			if(inputs->SoC > me->max_capacity) {
				//transition output -> violation on SoC > max_capacity
				//select a transition to solve the problem
				
				//Recovery instructions manually provided.
				
			} 
			
			break;

		
	}
}

//INPUT POLICY safe END



//output policies

//OUTPUT POLICY safe BEGIN
//This will run the input enforcer for ESS's policy safe
void ESS_run_output_enforcer_safe(enforcervars_ESS_t* me, inputs_ESS_t* inputs, outputs_ESS_t* outputs) {
	//advance timers
	
	
	//run enforcer
	switch(me->_policy_safe_state) {
		case POLICY_STATE_ESS_safe_output:
			
			if(inputs->SoC < me->min_capacity) {
				//transition output -> violation on SoC < min_capacity
				//select a transition to solve the problem
				
				//Recovery instructions manually provided.
				outputs->charge = 1;
				
			} 
			if(inputs->SoC < me->min_capacity) {
				//transition output -> violation on SoC < min_capacity
				//select a transition to solve the problem
				
				//Recovery instructions manually provided.
				outputs->watts = me->min_capacity - inputs->SoC;
				
			} 
			if(inputs->SoC > me->max_capacity) {
				//transition output -> violation on SoC > max_capacity
				//select a transition to solve the problem
				
				//Recovery instructions manually provided.
				outputs->charge = 0;
				
			} 
			if(inputs->SoC > me->max_capacity) {
				//transition output -> violation on SoC > max_capacity
				//select a transition to solve the problem
				
				//Recovery instructions manually provided.
				outputs->watts = inputs->SoC - me->max_capacity;
				
			} 
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
			if(outputs->charge && inputs->SoC + outputs->watts > me->max_capacity) {
				//transition output -> violation on charge and SoC + watts > max_capacity
				//select a transition to solve the problem
				
				//Recovery instructions manually provided.
				outputs->watts = ( me->max_capacity - inputs->SoC );
				
			} 
			if( !outputs->charge && inputs->SoC - outputs->watts < me->min_capacity) {
				//transition output -> violation on not (charge) and SoC - watts < min_capacity
				//select a transition to solve the problem
				
				//Recovery instructions manually provided.
				outputs->watts = ( inputs->SoC - me->min_capacity );
				
			} 

			break;

		
	}

	//select transition to advance state
	switch(me->_policy_safe_state) {
		case POLICY_STATE_ESS_safe_output:
			
			if(inputs->SoC >= me->min_capacity && inputs->SoC <= me->max_capacity && outputs->watts > 0 && outputs->charge && outputs->watts <= me->max_charge && inputs->SoC + outputs->watts <= me->max_capacity) {
				//transition output -> output on SoC >= min_capacity and SoC <= max_capacity and watts > 0 and charge and watts <= max_charge and SoC + watts <= max_capacity
				me->_policy_safe_state = POLICY_STATE_ESS_safe_output;
				//set expressions
				
			} 
			if(inputs->SoC >= me->min_capacity && inputs->SoC <= me->max_capacity && outputs->watts > 0 && !outputs->charge && outputs->watts <= me->max_discharge && inputs->SoC - outputs->watts >= me->min_capacity) {
				//transition output -> output on SoC >= min_capacity and SoC <= max_capacity and watts > 0 and not (charge) and watts <= max_discharge and SoC - watts >= min_capacity
				me->_policy_safe_state = POLICY_STATE_ESS_safe_output;
				//set expressions
				
			} 
			
			break;

		
	}
}

//OUTPUT POLICY safe END



