//
//  SScript.hpp
//  SpudScript
//
//  Created by Logan Pazol on 12/31/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SScript_hpp
#define SScript_hpp

#include <map>
#include <sstream>
#include "SAST.hpp"
#include "SFunctional.hpp"
#include "SOperator.hpp"

class SScript {
    
    public:
	
		void loadScript(std::string src);
	
        void execute();
	
		#define EXPOSE_FUNC(sc, f, r, s) bool bound_##sc##f = sc.bindFunction(f, #f, #r, #s);
		#define EXPOSE_FUNC_NO_ARGS(sc, f, r) bool bound_##sc##f = sc.bindFunction(f, #f, #r, "");
	
		template <class R, class... params>
		bool bindFunction(R(*func)(params...), std::string name, std::string return_type, std::string signature);
	
		template <class... params>
		void* callFunction(std::string name, params... p);
	
		template<class T>
		T* getScriptValue(std::string name);
	
		template<class T>
		void exposeVariable(void* value, std::string name);
	
    private:
	
		std::vector<SVMNode*> nodes;
	
		void* evaluateNode(SVMNode* node);
	
        SVariable declareVariable(size_t type);
		SVariable* resolveVariable(std::string name);
    
        std::map<std::string, SVariable> global_variables;
        std::map<std::string, SVMFunctionDefinition*> script_functions;
		std::map<std::string, SFunctionContainer*> cpp_functions;
	
		SVariable evaluateExpression(SVMExpression* expression);
		void evaluateBlock(SVMBlock* block);
		void* evaluateFunctionCall(SVMFunctionCall* call);
		void evaluateLoop(SVMLoop* loop);
		void* evaluateDeclaration(SVMDeclaration* declaration);
		void evaluateAssignment(SVMAssignment* assignment);
		void evaluateIf(SVMIfStatement* if_statement);
		void evaluateReturn(SVMReturn* return_node);
	
        SVMBlock* current_block;
    
};

template <class R, class... params>
bool SScript::bindFunction(R(*func)(params...), std::string name, std::string return_type, std::string signature) {
	
	// Get the function container
	SFunctionContainer* container = CreateSFunction(func, return_type, signature);
	cpp_functions[name] = container;
	
	return true;
	
}

template <class... params>
void* SScript::callFunction(std::string name, params... p){
	
	// Check if we have a function with the name
	if (script_functions.count(name)) {
		
		// Check we have the right number of args
		SVMFunctionDefinition* func = script_functions[name];

		if (sizeof...(params) != func->args.size()) {
			
			throw std::runtime_error("Wrong number of arguments passed to script function call from c++");
			return nullptr;
			
		}
		
		// Get the function and the args we passed in
		SVMBlock* block = script_functions[name]->block;
		std::vector<SVariable> args = parametersToVector(p...);
		
		for (int i = 0; i < sizeof...(params); i++) {
			
			// Check type matching TEMP, not done
			block->variables[func->args[i]->identifier] = declareVariable(func->args[i]->type);
			STypeRegistry::instance()->performCopy(block->variables[func->args[i]->identifier].value, args[i].value, func->args[i]->type);
			
		}
		
		// Call the function
		return evaluateNode(block);
		
	}
	
	return nullptr;
	
}

template<class T>
T* SScript::getScriptValue(std::string name) {
	
	SVariable* var = resolveVariable(name);
	
	// Check if this is a valid cVM
	if (var) {
		
		if (STypeRegistry::instance()->cpp_class_names.count(STypeRegistry::hashString(typeid(T).name())) &&
			(STypeRegistry::instance()->cpp_class_names[STypeRegistry::hashString(typeid(T).name())] == var->type))
		return (T*)var->value;
	
		throw std::runtime_error("Script variable was not of requested type");
		
	}
	
	throw std::runtime_error("Variable " + name + " was not declared in this scope");
	
	return nullptr;
	
}

template<class T>
void SScript::exposeVariable(void* value, std::string name) {
	
	// Get the type that we exposed
	size_t hash = STypeRegistry::instance()->hashString(typeid(T).name());
	if (STypeRegistry::instance()->cpp_class_names.count(hash)) {
		
		// Check that it wasnt defined globally already
		if (!global_variables.count(name)) {
		
			// Create a new variable and expose it in the global scope
			global_variables[name].type = STypeRegistry::instance()->cpp_class_names[hash];
			global_variables[name].value = value;
			
		} else throw std::runtime_error("Redefintion of " + name);
		
	} else throw std::runtime_error("Type was not exposed to script");
	
}


#endif /* SScript_hpp */
