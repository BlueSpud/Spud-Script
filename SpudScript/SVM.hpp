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
		bool bindFunction(void(*func)(params ...), std::string name) {
		
			// Get the function container
			SFunctionContainer* container = CreateSFunction(func);
			cpp_functions[name] = container;
			
			return true;
		
		}
	
		template<class T>
		T* getScriptValue(std::string name) {
		
			SVariable* var = resolveVarible(name);
			
			// Check if this is a valid cast
			if (STypeRegistry::instance()->cpp_class_names.count(var->type) && !strcmp(STypeRegistry::instance()->cpp_class_names[var->type].c_str(), typeid(T).name()))
				return (T*)var->value;
			
			throw std::runtime_error("Script variable could not be cast to c++ type");
			return nullptr;
		
		}
	
		#define EXPOSE_FUNC(vm, f) bool bound_##f = vm.bindFunction(f, #f);
	
    private:
	
		void* evaluateNode(SASTNode* node);
	
        SVariable declareVariable(std::string& type);
    
        std::map<std::string, SVariable> global_variables;
        std::map<std::string, SASTFunctionDefinition*> script_functions;
		std::map<std::string, SFunctionContainer*> cpp_functions;
    
        SBlock* current_block;
    
};

#endif /* SVM_hpp */
