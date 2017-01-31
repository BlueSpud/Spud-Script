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
	
		virtual SVariable operator()(const std::vector<void*>& variables) = 0;
	
};


// Special thanks to n.m. on Stack Overflow for help with this class
template <class R, class ...params>
class SFunction : public SFunctionContainer {
	
	public:
	
		SFunction(R(*_func)(params...), const std::string& _return_type, const std::string& _signature) {
			
			func = _func;
			return_type = _return_type;
			
			// Get the signature of the function as a vector of strings
			std::stringstream stream(_signature);
			std::string type;
			while (std::getline(stream, type, ' '))
				signature.push_back(type);
			
		}
	
		virtual SVariable operator()(const std::vector<void*>& variables) {
		
			// Call the function
			checkAndCall(variables);
			
			// Void return value
			return SVariable();
		
		}
	
		R checkAndCall(const std::vector<void*>& variables) {
		
			// Make sure we have correct # of parameters in the vector
			if (sizeof...(params) != variables.size())
				throw std::runtime_error("Wrong number of arguments");
			return call<std::tuple<params...>>(variables, std::make_index_sequence<sizeof...(params)>());
		
		}
	
	private:
	
		R(*func)(params...);
	
		template <class tuple, size_t... index>
		R call(const std::vector<void*>& variables, std::index_sequence<index...>) {
		
			// Call the function with the variables
			return func(castCPP<typename std::tuple_element<index, tuple>::type>(variables[index])...);
		
		};
	
};

template <class R, class ...params>
class SFunctionReturn : public SFunction<R, params...> {
	
	public:
	
		SFunctionReturn(R(*_func)(params...), const std::string& _return_type, const std::string& _signature) : SFunction<R, params...>(_func, _return_type, _signature) {}
	
		virtual SVariable operator()(const std::vector<void*>& variables) {
	
			// Get the value of the function
			R return_value = SFunction<R, params...>::checkAndCall(variables);
			SVariable var;

			// Copy the memory into a new block
			var.value = calloc(1, sizeof(R));
			memcpy(var.value, &return_value, sizeof(R));
			var.type = STypeRegistry::instance()->cpp_class_names[STypeRegistry::instance()->hashString(typeid(R).name())];
			
			return var;
		
		}
	
};

template <class R, class... params>
SFunctionContainer* CreateSFunction(R(*func)(params ...), const std::string& _return_type, const std::string& _signature) {
	
	// Create a new function with a return type
	return (SFunctionContainer*)(new SFunctionReturn<R, params...>(func, _return_type, _signature));
	
}

template <class... params>
SFunctionContainer* CreateSFunction(void(*func)(params ...), const std::string& _return_type, const std::string& _signature) {
	
	// Create a new function with a void return type type
	return (SFunctionContainer*)(new SFunction<void, params...>(func, _return_type, _signature));
	
}

template <class T>
void parameterPushBack(std::vector<SVariable>& args, T& param) {

	// Check if a C++ type was declared for this
	if (STypeRegistry::instance()->cpp_class_names.count(STypeRegistry::hashString(typeid(T).name()))) {
	
		// Create a var and add it
		SVariable var;
		var.value = (void*)&param;
		var.type = STypeRegistry::instance()->cpp_class_names[STypeRegistry::hashString(typeid(T).name())];
		
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
