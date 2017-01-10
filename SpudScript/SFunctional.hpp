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
	
		virtual void operator()(const std::vector<void*>& variables) = 0;
	
};

template <class ...params>
class SFunction : public SFunctionContainer {
	
	public:
	
		SFunction(void(*_func)(params...)) { func = _func; }
	
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
SFunctionContainer* CreateSFunction(void(*func)(params ...)) { return (SFunctionContainer*)(new SFunction<params...>(func)); }

#endif /* SFunctional_h */
