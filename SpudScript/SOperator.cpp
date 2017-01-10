//
//  SOperator.cpp
//  SpudScript
//
//  Created by Logan Pazol on 1/9/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#include "SOperator.hpp"

SOperatorRegistry* SOperatorRegistry::instance() {
	
	static SOperatorRegistry* instance = NULL;
	
	if (instance == NULL) {
		
		instance = new SOperatorRegistry();
		
		// Register default operators
		instance->registerOperator(oInt, "int");
		instance->registerOperator(oFloat, "float");
		
		// Register default casts
		instance->registerCast(cInt, "int");
		instance->registerCast(cFloat, "float");
		
	}
	
	return instance;
	
}

bool SOperatorRegistry::registerOperator(operator_func func, std::string type) {
	
	// Save the operator
	operator_funcs[type] = func;
	return true;
	
}

bool SOperatorRegistry::registerCast(cast_func func, std::string type) {
	
	// Save the cast
	cast_funcs[type] = func;
	return true;
	
}

void* SOperatorRegistry::performCast(SCAST_ARGS) {
	
	// Check if we have a cast for this
	if (cast_funcs.count(var->type)) {
		
		// Return the casted value if success
		void* cast_result = cast_funcs[var->type](var, type);
		if (cast_result)
			return cast_result;
		else free(cast_result);
		
	}
	
	throw std::runtime_error("Type cannot be cast");
	
	return nullptr;
	
}

void* SOperatorRegistry::performOperation(SVariable* first, SVariable* second, const std::string& o) {
	
	// See if we have an operator
	if (operator_funcs.count(first->type)) {
		
		// Get the value
		void* out_value = operator_funcs[first->type](first, second, o);
		if (out_value)
			return out_value;
		else free(out_value);
	
	}
	
	throw std::runtime_error("Undefined operation\n");
	
	return nullptr;
	
}

void* SOperatorRegistry::oInt(SOPERATOR_ARGS) {
	
	// Int to int
	if (!second->type.compare("int")) {
		
		// Get the float and the int
		int a = *(int*)first->value;
		int b = *(int*)second->value;
		
		if (!o.compare("+")) {
			
			int* out = (int*)malloc(sizeof(int));
			*out = a + b;
			return out;
			
		}
		
		if (!o.compare("-")) {
			
			int* out = (int*)malloc(sizeof(int));
			*out = a - b;
			return out;
			
		}
		
		if (!o.compare("/")) {
			
			int* out = (int*)malloc(sizeof(int));
			*out = a / b;
			return out;
			
		}
		
		if (!o.compare("*")) {
			
			int* out = (int*)malloc(sizeof(int));
			*out = a * b;
			return out;
			
		}
		
	}
	
	// Int to float
	if (!second->type.compare("float")) {
		
		// Get the float and the int
		int a = *(int*)first->value;
		float b = *(float*)second->value;
		
		if (!o.compare("+")) {
			
			int* out = (int*)malloc(sizeof(int));
			*out = a + b;
			return out;
			
		}
		
		if (!o.compare("-")) {
			
			int* out = (int*)malloc(sizeof(int));
			*out = a - b;
			return out;
			
		}
		
		if (!o.compare("/")) {
			
			int* out = (int*)malloc(sizeof(int));
			*out = a / b;
			return out;
			
		}
		
		if (!o.compare("*")) {
			
			int* out = (int*)malloc(sizeof(int));
			*out = a * b;
			return out;
			
		}
		
	}

	return nullptr;
	
}

void* SOperatorRegistry::oFloat(SOPERATOR_ARGS) {
	
	// float to int
	if (!second->type.compare("int")) {
		
		// Get the float and the int
		float a = *(float*)first->value;
		int b = *(int*)second->value;
		
		if (!o.compare("+")) {
			
			float* out = (float*)malloc(sizeof(float));
			*out = a + b;
			return out;
			
		}
		
		if (!o.compare("-")) {
			
			float* out = (float*)malloc(sizeof(float));
			*out = a - b;
			return out;
			
		}
		
		if (!o.compare("/")) {
			
			float* out = (float*)malloc(sizeof(float));
			*out = a / b;
			return out;
			
		}
		
		if (!o.compare("*")) {
			
			float* out = (float*)malloc(sizeof(float));
			*out = a * b;
			return out;
			
		}
		
	}
	
	// float to float
	if (!second->type.compare("float")) {
		
		// Get the float and the int
		float a = *(float*)first->value;
		float b = *(float*)second->value;
		
		if (!o.compare("+")) {
			
			float* out = (float*)malloc(sizeof(float));
			*out = a + b;
			return out;
			
		}
		
		if (!o.compare("-")) {
			
			float* out = (float*)malloc(sizeof(float));
			*out = a - b;
			return out;
			
		}
		
		if (!o.compare("/")) {
			
			float* out = (float*)malloc(sizeof(float));
			*out = a / b;
			return out;
			
		}
		
		if (!o.compare("*")) {
			
			float* out = (float*)malloc(sizeof(float));
			*out = a * b;
			return out;
			
		}
		
	}
	
	return nullptr;
	
}

void* SOperatorRegistry::cInt(SCAST_ARGS) {
	
	if (!type.compare("float")) {
		
		float* f = (float*)malloc(sizeof(float));
		*f = (float)*(int*)var->value;
		return f;
		
	}
	
	return nullptr;
	
}

void* SOperatorRegistry::cFloat(SCAST_ARGS) {
	
	if (!type.compare("int")) {
		
		int* i = (int*)malloc(sizeof(int));
		*i = (int)*(float*)var->value;
		return i;
		
	}
	
	return nullptr;
	
}
