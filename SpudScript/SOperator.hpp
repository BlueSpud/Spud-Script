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

#define SCAST_ARGS SVariable* var, const std::string& type
#define SCAST(n) static void* n(SCAST_ARGS);
typedef void*(*cast_func)(SVariable*, const std::string&);

class SOperatorRegistry {
	
	public:
	
		static SOperatorRegistry* instance();
	
		bool registerOperator(operator_func func, std::string type);
		bool registerCast(cast_func func, std::string type);
	
		void* performOperation(SOPERATOR_ARGS);
		void* performCast(SCAST_ARGS);
	
		template <class first_c, class second_c>
		void* standardArithmatic(SOPERATOR_ARGS);
	
		template <class from, class to>
		void* standardCast(SCAST_ARGS);
	
	private:
	
		std::map<std::string, operator_func> operator_funcs;
		std::map<std::string, cast_func> cast_funcs;
	
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
	
};

template <class first_c, class second_c>
void* SOperatorRegistry::standardArithmatic(SOPERATOR_ARGS) {

	first_c a = *(first_c*)first->value;
	second_c b = *(second_c*)second->value;
	
	if (!o.compare("+")) {
		
		first_c* out = (first_c*)malloc(sizeof(first_c));
		*out = a + (first_c)b;
		return out;
		
	}
	
	if (!o.compare("-")) {
		
		first_c* out = (first_c*)malloc(sizeof(first_c));
		*out = a - (first_c)b;
		return out;
		
	}
	
	if (!o.compare("/")) {
		
		first_c* out = (first_c*)malloc(sizeof(first_c));
		*out = a / (first_c)b;
		return out;
		
	}
	
	if (!o.compare("*")) {
		
		first_c* out = (first_c*)malloc(sizeof(first_c));
		*out = a * (first_c)b;
		
		return out;
		
	}
	
	return nullptr;
	
}

template <class from, class to>
void* SOperatorRegistry::standardCast(SCAST_ARGS) {
	
	to* f = (to*)malloc(sizeof(to));
	*f = (to)*(from*)var->value;
	return f;
	
}

#endif /* SOperator_hpp */
