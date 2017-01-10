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
		instance->registerOperator(oDouble, "double");
		instance->registerOperator(oLong, "long");
		
		// Register default casts
		instance->registerCast(cInt, "int");
		instance->registerCast(cFloat, "float");
		instance->registerCast(cDouble, "double");
		instance->registerCast(cLong, "long");
		
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
	
	throw std::runtime_error("Type cannot be cast: " + var->type);
	
	return nullptr;
	
}

void* SOperatorRegistry::performOperation(SOPERATOR_ARGS) {
	
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
	if (!second->type.compare("int"))
		return instance()->standardArithmatic<int, int>(first, second, o);
	
	// Int to float
	if (!second->type.compare("float"))
		return instance()->standardArithmatic<int, float>(first, second, o);
	
	// Int to double
	if (!second->type.compare("double"))
		return instance()->standardArithmatic<int, double>(first, second, o);
	
	// Int to long
	if (!second->type.compare("long"))
		return instance()->standardArithmatic<int, long>(first, second, o);
	
	// Int to char
	if (!second->type.compare("char"))
		return instance()->standardArithmatic<int, char>(first, second, o);

	return nullptr;
	
}

void* SOperatorRegistry::oFloat(SOPERATOR_ARGS) {
	
	// Float to int
	if (!second->type.compare("int"))
		return instance()->standardArithmatic<float, int>(first, second, o);
	
	// Float to float
	if (!second->type.compare("float"))
		return instance()->standardArithmatic<float, float>(first, second, o);
	
	// Float to double
	if (!second->type.compare("double"))
		return instance()->standardArithmatic<float, double>(first, second, o);
	
	// Float to long
	if (!second->type.compare("long"))
		return instance()->standardArithmatic<float, long>(first, second, o);
	
	// Float to char
	if (!second->type.compare("char"))
		return instance()->standardArithmatic<float, char>(first, second, o);
	
	return nullptr;
	
}

void* SOperatorRegistry::oDouble(SOPERATOR_ARGS) {
	
	// Float to int
	if (!second->type.compare("int"))
		return instance()->standardArithmatic<double, int>(first, second, o);
	
	// Float to float
	if (!second->type.compare("float"))
		return instance()->standardArithmatic<double, float>(first, second, o);
	
	// Float to double
	if (!second->type.compare("double"))
		return instance()->standardArithmatic<double, double>(first, second, o);
	
	// Float to long
	if (!second->type.compare("long"))
		return instance()->standardArithmatic<double, long>(first, second, o);
	
	// Float to char
	if (!second->type.compare("char"))
		return instance()->standardArithmatic<double, char>(first, second, o);
	
	return nullptr;
	
}

void* SOperatorRegistry::oLong(SOPERATOR_ARGS) {
	
	// Float to int
	if (!second->type.compare("int"))
		return instance()->standardArithmatic<long, int>(first, second, o);
	
	// Float to float
	if (!second->type.compare("float"))
		return instance()->standardArithmatic<long, float>(first, second, o);
	
	// Float to double
	if (!second->type.compare("double"))
		return instance()->standardArithmatic<long, double>(first, second, o);
	
	// Float to long
	if (!second->type.compare("long"))
		return instance()->standardArithmatic<long, long>(first, second, o);
	
	// Float to char
	if (!second->type.compare("char"))
		return instance()->standardArithmatic<long, char>(first, second, o);
	
	return nullptr;
	
}

void* SOperatorRegistry::cInt(SCAST_ARGS) {
	
	if (!type.compare("int"))
		return instance()->standardCast<int, int>(var, type);
	
	if (!type.compare("float"))
		return instance()->standardCast<int, float>(var, type);
	
	if (!type.compare("double"))
		return instance()->standardCast<int, double>(var, type);
	
	if (!type.compare("long"))
		return instance()->standardCast<int, long>(var, type);
	
	if (!type.compare("char"))
		return instance()->standardCast<int, char>(var, type);
	
	if (!type.compare("bool"))
		return instance()->standardCast<int, bool>(var, type);
	
	return nullptr;
	
}

void* SOperatorRegistry::cFloat(SCAST_ARGS) {
	
	if (!type.compare("int"))
		return instance()->standardCast<float, int>(var, type);
	
	if (!type.compare("float"))
		return instance()->standardCast<float, float>(var, type);
	
	if (!type.compare("double"))
		return instance()->standardCast<float, double>(var, type);
	
	if (!type.compare("long"))
		return instance()->standardCast<float, long>(var, type);
	
	if (!type.compare("char"))
		return instance()->standardCast<float, char>(var, type);
	
	if (!type.compare("bool"))
		return instance()->standardCast<float, bool>(var, type);
	
	return nullptr;
	
}

void* SOperatorRegistry::cDouble(SCAST_ARGS) {
	
	if (!type.compare("int"))
		return instance()->standardCast<double, int>(var, type);
	
	if (!type.compare("float"))
		return instance()->standardCast<double, float>(var, type);
	
	if (!type.compare("double"))
		return instance()->standardCast<double, double>(var, type);
	
	if (!type.compare("long"))
		return instance()->standardCast<double, long>(var, type);
	
	if (!type.compare("char"))
		return instance()->standardCast<double, char>(var, type);
	
	if (!type.compare("bool"))
		return instance()->standardCast<double, bool>(var, type);
	
	return nullptr;
	
}

void* SOperatorRegistry::cLong(SCAST_ARGS) {
	
	if (!type.compare("int"))
		return instance()->standardCast<long, int>(var, type);
	
	if (!type.compare("float"))
		return instance()->standardCast<long, float>(var, type);
	
	if (!type.compare("double"))
		return instance()->standardCast<long, double>(var, type);
	
	if (!type.compare("long"))
		return instance()->standardCast<long, long>(var, type);
	
	if (!type.compare("char"))
		return instance()->standardCast<long, char>(var, type);
	
	if (!type.compare("bool"))
		return instance()->standardCast<long, bool>(var, type);
	
	return nullptr;
	
}
