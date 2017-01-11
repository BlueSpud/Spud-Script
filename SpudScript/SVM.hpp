//
//  SVM.hpp
//  SpudScript
//
//  Created by Logan Pazol on 12/31/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SVM_hpp
#define SVM_hpp

#include <map>
#include <sstream>
#include "SAST.hpp"
#include "SFunctional.hpp"
#include "SOperator.hpp"

class SVM {
    
    public:
    
        void executeCode(std::vector<SASTNode*> nodes);
    
        SVariable* resolveVarible(std::string name);
	
		template <class... params>
		bool bindFunction(void(*func)(params...), std::string name, std::string return_type, std::string signature);
	
		template <class... params>
		void* callFunction(std::string name, params... p);
	
		template<class T>
		T* getScriptValue(std::string name);
	
		#define EXPOSE_FUNC(vm, f, r, s) bool bound_##f = vm.bindFunction(f, #f, #r, #s);
	
    private:
	
		void* evaluateNode(SASTNode* node);
	
        SVariable declareVariable(std::string& type);
    
        std::map<std::string, SVariable> global_variables;
        std::map<std::string, SASTFunctionDefinition*> script_functions;
		std::map<std::string, SFunctionContainer*> cpp_functions;
	
		void* evaluateExpression(SASTExpression* expression);
		void* evaluateFuncitonCall(SASTFunctionCall* call);
	
        SBlock* current_block;
    
};

template <class... params>
bool SVM::bindFunction(void(*func)(params...), std::string name, std::string return_type, std::string signature) {
	
	// Get the function container
	SFunctionContainer* container = CreateSFunction(func, return_type, signature);
	cpp_functions[name] = container;
	
	return true;
	
}

template <class... params>
void* SVM::callFunction(std::string name, params... p){
	
	// Check if we have a function with the name
	if (script_functions.count(name)) {
		
		// Check we have the right number of args
		SASTFunctionDefinition* func = script_functions[name];

		if (sizeof...(params) != func->args.size()) {
			
			throw std::runtime_error("Wrong number of arguments passed to script function call from c++");
			return nullptr;
			
		}
		
		// Get the function and the args we passed in
		SBlock* block = script_functions[name]->block;
		std::vector<SVariable> args = parametersToVector(p...);
		
		for (int i = 0; i < sizeof...(params); i++) {
			
			// Check type matching TEMP, not done
			block->variables[func->args[i]->identifier.string] = declareVariable(func->args[i]->type.string);
			
			memcpy(block->variables[func->args[i]->identifier.string].value, args[i].value, STypeRegistry::instance()->getTypeSize(func->args[i]->type.string));
			
		}
		
		// Call the function
		evaluateNode(block);
		
	}
	
	return nullptr;
	
}

template<class T>
T* SVM::getScriptValue(std::string name) {
	
	SVariable* var = resolveVarible(name);
	
	// Check if this is a valid cast
	if (var) {
		
		if (STypeRegistry::instance()->cpp_class_names.count(typeid(T).name()) &&
			!STypeRegistry::instance()->cpp_class_names[typeid(T).name()].compare(var->type))
		return (T*)var->value;
	
		throw std::runtime_error("Script variable could not be cast to c++ type");
		
	}
	
	throw std::runtime_error("Variable " + name + " was not declared in this scope");
	
	return nullptr;
	
}


#endif /* SVM_hpp */
