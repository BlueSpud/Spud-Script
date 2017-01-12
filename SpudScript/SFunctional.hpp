//
//  SFunctional.h
//  SpudScript
//
//  Created by Logan Pazol on 1/9/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SFunctional_h
#define SFunctional_h

#include "STypes.hpp"

class SFunctionContainer {
	
	public:
	
		std::string return_type;
		std::vector<std::string> signature;
	
		virtual void operator()(const std::vector<void*>& variables) = 0;
	
};


// Special thanks to n.m. on Stack Overflow for help with this class
template <class ...params>
class SFunction : public SFunctionContainer {
	
	public:
	
		SFunction(void(*_func)(params...), const std::string& _return_type, const std::string& _signature) {
			
			func = _func;
			return_type = _return_type;
			
			// Get the signature of the function as a vector of strings
			std::stringstream stream(_signature);
			std::string type;
			while (std::getline(stream, type, ' '))
				signature.push_back(type);
			
		}
	
		virtual void operator()(const std::vector<void*>& variables) {
		
			// Make sure we have correct # of parameters in the vector
			if (sizeof...(params) != variables.size())
				throw std::runtime_error("Wrong number of arguments");
			else call<std::tuple<params...>>(variables, std::make_index_sequence<sizeof...(params)>());
		
		}
	
	private:
	
		void(*func)(params...);
	
		template <class tuple, size_t... index>
		void call(const std::vector<void*>& variables, std::index_sequence<index...>) {
		
			// Call the function with the variables
			func(castCPP<typename std::tuple_element<index, tuple>::type>(variables[index])...);
		
		};
	
};

template <class... params>
SFunctionContainer* CreateSFunction(void(*func)(params ...), const std::string& _return_type, const std::string& _signature) {
	
	// Create a new function and return it as a container
	return (SFunctionContainer*)(new SFunction<params...>(func, _return_type, _signature));
	
}

template <class T>
void parameterPushBack(std::vector<SVariable>& args, T& param) {

	// Check if a C++ type was declared for this
	if (STypeRegistry::instance()->cpp_class_names.count(typeid(T).name())) {
	
		// Create a var and add it
		SVariable var;
		var.value = (void*)&param;
		var.type = STypeRegistry::instance()->cpp_class_names[typeid(T).name()];
		
		args.push_back(var);
		
	} else throw std::runtime_error("Trying to call function with a parameter of a type that was not exposed!");
	
}


template <class ...params, class T>
void parameterPushBack(std::vector<SVariable>& args, T& param, params&... p) {
	
	// Push back the top param
	parameterPushBack(args, param);
	
	// Recursively call for the next
	parameterPushBack(args, p...);
	
}

template <class ...params>
std::vector<SVariable> parametersToVector(params&... p) {
	
	// Create a vector and fill it
	std::vector<SVariable> args;
	parameterPushBack(args, p...);
	
	return args;
	
}

#endif /* SFunctional_h */
