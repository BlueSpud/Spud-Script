//
//  SOperator.hpp
//  SpudScript
//
//  Created by Logan Pazol on 1/9/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SOperator_hpp
#define SOperator_hpp

#include "STypes.hpp"

#define SOPERATOR_ARGS SVariable* first, SVariable* second, const std::string& o
#define SOPERATOR(n) static void* n(SOPERATOR_ARGS);
typedef void*(*operator_func)(SVariable*, SVariable*, const std::string&);

#define SCAST_ARGS SVariable* var, const size_t type
#define SCAST(n) static void* n(SCAST_ARGS);
typedef void*(*cast_func)(SVariable*, const size_t);

class SOperatorRegistry {
	
	public:
	
		static SOperatorRegistry* instance();
	
		#define REGISTER_OPERATOR(f, t) bool operator_registered_##t = SOperatorRegistry::instance()->registerOperator(f, #t);
		bool registerOperator(operator_func func, const std::string type);
	
		#define REGISTER_CAST(f, t) bool cast_registered_##t = SOperatorRegistry::instance()->registerCast(f, #t);
		bool registerCast(cast_func func, const std::string type);
	
		void* performOperation(SOPERATOR_ARGS);
		void* performCast(SCAST_ARGS);
	
		template <class first_c, class second_c>
		void* standardArithmatic(SOPERATOR_ARGS);
	
		template <class first_c, class second_c>
		void* modulusArithmatic(SOPERATOR_ARGS);
	
		template <class from, class to>
		void* standardCast(SCAST_ARGS);
	
		template <class from>
		void* toString(SVariable* var);
	
	//private:
	
		std::map<size_t, operator_func> operator_funcs;
		std::map<size_t, cast_func> cast_funcs;
	
		SOPERATOR(oInt)
		SCAST(cInt)
	
		SOPERATOR(oFloat)
		SCAST(cFloat)
	
		SOPERATOR(oDouble)
		SCAST(cDouble)
	
		SOPERATOR(oLong)
		SCAST(cLong)
	
		SOPERATOR(oBool)
		SCAST(cBool)
	
		SOPERATOR(oString)
		SCAST(cString)
	
};

template <class first_c, class second_c>
void* SOperatorRegistry::standardArithmatic(SOPERATOR_ARGS) {

	first_c a = *(first_c*)first->value;
	second_c b = *(second_c*)second->value;
	
	if (!o.compare("+")) {
		
		first_c* out = (first_c*)calloc(1,sizeof(first_c));
		*out = (first_c)(a + b);
		return out;
		
	}
	
	if (!o.compare("-")) {
		
		first_c* out = (first_c*)calloc(1,sizeof(first_c));
		*out = (first_c)(a - b);
		return out;
		
	}
	
	if (!o.compare("/")) {
		
		first_c* out = (first_c*)calloc(1,sizeof(first_c));
		*out = (first_c)(a / b);
		return out;
		
	}
	
	if (!o.compare("*")) {
		
		first_c* out = (first_c*)calloc(1,sizeof(first_c));
		*out = (first_c)(a * b);
		
		return out;
		
	}
	
	if (!o.compare("==")) {
		
		first_c* out = (first_c*)calloc(1,sizeof(first_c));
		*out = a == b;
		
		return out;
		
	}
	
	if (!o.compare("!=")) {
		
		first_c* out = (first_c*)calloc(1,sizeof(first_c));
		*out = a != b;
		
		return out;
		
	}
	
	if (!o.compare("<")) {
		
		first_c* out = (first_c*)calloc(1,sizeof(first_c));
		*out = a < b;
		
		return out;
		
	}
	
	if (!o.compare(">")) {
		
		first_c* out = (first_c*)calloc(1,sizeof(first_c));
		*out = a > b;
		
		return out;
		
	}
	
	if (!o.compare("<=")) {
		
		first_c* out = (first_c*)calloc(1,sizeof(first_c));
		*out = a <= b;
		
		return out;
		
	}
	
	if (!o.compare(">=")) {
		
		first_c* out = (first_c*)calloc(1,sizeof(first_c));
		*out = a >= b;
		
		return out;
		
	}
	
	return nullptr;
	
}

template <class first_c, class second_c>
void* SOperatorRegistry::modulusArithmatic(SOPERATOR_ARGS) {
	
	// Check if one of the standard operators worked
	void* reg_result = standardArithmatic<first_c, second_c>(first, second, o);
	if (reg_result)
		return reg_result;
	
	// Perform modulus operation
	if (!o.compare("%")) {
		
		first_c a = *(first_c*)first->value;
		second_c b = *(second_c*)second->value;
		
		first_c* out = (first_c*)calloc(1,sizeof(first_c));
		*out = a % b;
		
		return out;
		
	}
	
	return nullptr;
	
}

template <class from, class to>
void* SOperatorRegistry::standardCast(SCAST_ARGS) {
	
	to* f = (to*)calloc(1,sizeof(to));
	*f = (to)*(from*)var->value;
	return f;
	
}

template <class from>
void* SOperatorRegistry::toString(SVariable* var) {
	
	std::string string = std::to_string(*(from*)var->value);
	
	// Create a new string buffer
	size_t size = sizeof(char) * (string.length() + 1);
	char* buffer = (char*)calloc(1,size);
	sprintf(buffer, "%s", string.c_str());
	
	char** ptr = (char**)calloc(1,sizeof(char**));
	ptr[0] = buffer;
	
	return ptr;

}

#endif /* SOperator_hpp */
