//
//  SVM.cpp
//  SpudScript
//
//  Created by Logan Pazol on 12/31/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SVM.hpp"

void SVM::executeCode(std::vector<SASTNode*> nodes) {

    for (int i = 0; i < nodes.size(); i++)
        evaluateNode(nodes[i]);

}

void* SVM::evaluateNode(SASTNode* node) {

    switch (node->node_type) {
            
        case SASTTypeExpression: {
            
            // Get the numbers
            SASTExpression* expresison = (SASTExpression*)node;
            std::vector<SVariable> vars;
            
			for (int i = 0; i < expresison->tokens.size(); i++) {
				
                if (expresison->tokens[i].type == STokenTypeNumber) {
					
					// Create a numbers
					SVariable var;
					if (expresison->tokens[i].string.find('.') != std::string::npos) {
						
						var.value = malloc(sizeof(float));
						*(float*)var.value = atof(expresison->tokens[i].string.c_str());
						var.type = "float";
						
					} else {
						
						var.value = malloc(sizeof(int));
						*(int*)var.value = atoi(expresison->tokens[i].string.c_str());
						var.type = "int";
						
					}
					
					vars.push_back(var);
                    
                } else if (expresison->tokens[i].type == STokenTypeIdentifier) {
                    
                    SVariable* var = resolveVarible(expresison->tokens[i].string);
					if (var) {
					
						// Create a copy of the variable
						vars.push_back(SVariable());
						SVariable& new_var = vars.back();
						
						size_t size = STypeRegistry::instance()->getTypeSize(var->type);
						new_var.value = malloc(size);
						memcpy(new_var.value, var->value, size);
						
						new_var.type = var->type;

						
					} else {
                        
                        std::cout << expresison->tokens[i].string << " was not defined in this scope" << std::endl;
                        return 0;
                        
                    }
					
				}
				
			}
            
            // Go through again, evaluating operators
            int var = -1;
            
            for (int i = 0; i < expresison->tokens.size(); i++) {
                if (expresison->tokens[i].type == STokenTypeOperator && (!expresison->tokens[i].string.compare("*") || !expresison->tokens[i].string.compare("/") || !expresison->tokens[i].string.compare("%"))) {
                        
                    void* new_value = SOperatorRegistry::instance()->performOperation(&vars[var], &vars[var + 1], expresison->tokens[i].string);
					
                    // Set and remove
                    vars[var].value = new_value;
					
					// Free the value and delete
					free(vars[var + 1].value);
                    std::vector<SVariable>::iterator itterator = vars.begin() + var + 1;
                    vars.erase(itterator);
                    vars.shrink_to_fit();
                        
                    // Incremenent i to skip the next number
                    i++;
                        
                        
                } else if (expresison->tokens[i].type == STokenTypeNumber || expresison->tokens[i].type == STokenTypeIdentifier)
                    var++;
            }
            
			SVariable& result = vars[0];
            var = 1;
			
            // Do addition and subtraction
            for (int i = 0; i < expresison->tokens.size(); i++) {
                if (expresison->tokens[i].type == STokenTypeOperator && (!expresison->tokens[i].string.compare("+") || !expresison->tokens[i].string.compare("-"))) {
					
                    void* new_value = SOperatorRegistry::instance()->performOperation(&result, &vars[var], expresison->tokens[i].string);
					
					// Free allocation
					free(vars[var].value);
					result.value = new_value;
                    var++;
                    
                }
                
            }
			
			// Do a cast if we need to
			if (result.type.compare(expresison->destination_type))
				result.value = SOperatorRegistry::instance()->performCast(&result, expresison->destination_type);
			
			return result.value;
			
            
        }  break;
            
        case SASTTypeDeclaration: {
            
            SASTDeclaration* declaration = (SASTDeclaration*)node;
            
            if (!current_block) {
                
                // Declare a variable in the global scope
				if (!global_variables.count(declaration->identifier.string)) {
					
                    global_variables[declaration->identifier.string] = declareVariable(declaration->type.string);
					return &global_variables[declaration->identifier.string];
					
				} else {
					
                    std::cout << "Redefinition of " << declaration->identifier.string << std::endl;
                    return nullptr;
                
                }
                
            } else {
                
                // Make sure that the variable is not already defined
                SVariable* variable = resolveVarible(declaration->identifier.string);
                if (!variable) {
                
                    // Declare a variable in a scope
                    current_block->variables[declaration->identifier.string] = declareVariable(declaration->type.string);
					return &current_block->variables[declaration->identifier.string];
                    
                } else {
                    
                    std::cout << "Redefinition of " << declaration->identifier.string << std::endl;
                    return nullptr;
                    
                }
                
            }
            
        } break;
            
        case SASTTypeAssignment: {
            
            SASTAssignment* assignment = (SASTAssignment*)node;
            
            // If we need to declare, declare
            if (assignment->declaration) {
				
				// Evaluate the expression
				assignment->expression->destination_type = assignment->declaration->type.string;
				void* expression_result = evaluateNode(assignment->expression);
				
				// COPY the result
                SVariable* variable = (SVariable*)evaluateNode(assignment->declaration);
				memcpy(variable->value, expression_result, STypeRegistry::instance()->getTypeSize(variable->type));
				free(expression_result);
				
            } else {
                
                SVariable* variable = resolveVarible(assignment->identifier.string);
                if (variable) {
					
					assignment->expression->destination_type = variable->type;
					void* expression_result = evaluateNode(assignment->expression);
					
					// COPY the result
					memcpy(variable->value, expression_result, STypeRegistry::instance()->getTypeSize(variable->type));
					free(expression_result);
					
                } else {
                    
                    // Variable was not declared
                    std::cout << assignment->identifier.string << " was not defined in this scope\n";
                    
                }
                
            }
            
        } break;
            
        case SASTTypeFunctionCall: {
            
            SASTFunctionCall* function = (SASTFunctionCall*)node;
            
            // Check if there is a script function
            if (script_functions.count(function->identifier.string)) {
                
                // Clear the variables from the last function call
                SASTFunctionDefinition* def = script_functions[function->identifier.string];
                SBlock* block = script_functions[function->identifier.string]->block;
                
                block->variables.erase(block->variables.begin(), block->variables.end());
                
                // Define the new variables of the argument types
                if (function->expressions.size() == def->args.size()) {
                    
                    for (int i = 0; i < function->expressions.size(); i++) {
                        
                        // Check type matching TEMP, not done
						block->variables[def->args[i]->identifier.string] = declareVariable(def->args[i]->type.string);
						
						function->expressions[i]->destination_type = def->args[i]->type.string;
						void* expression_result = evaluateNode(function->expressions[i]);
						memcpy(block->variables[def->args[i]->identifier.string].value, expression_result, STypeRegistry::instance()->getTypeSize(def->args[i]->type.string));
						free(expression_result);
						
                    }
                    
                    // Call the function
                    evaluateNode(block);
                    
                    
                } else {
                    
                    std::cout << "Invalid number of arguments, got " << function->expressions.size() << " expected " << def->args.size() << std::endl;
                    return 0;
                    
                }
                
            } else if (cpp_functions.count(function->identifier.string)) {
                
                // Call C++ function
				std::vector<void*> params;
				
				for (int i = 0; i < function->expressions.size(); i++) {
					
					function->expressions[i]->destination_type = "float";
					void* expression_result = evaluateNode(function->expressions[i]);
					params.push_back(expression_result);
					
				}
				
                cpp_functions[function->identifier.string]->operator()(params);
                
            } else {
            
                // Neither a script function existed nor a C++ one did
                std::cout << "Unknown Function: " << function->identifier.string << std::endl;
                return 0;
                
            }
            
        } break;
            
        case SASTTypeBlock: {
            
            SBlock* block = (SBlock*)node;
            
            SBlock* last_block = current_block;
            current_block = block;
            
            // Go through all the nodes in the block and evaluate them
            for (int i = 0; i < block->nodes.size(); i++)
                evaluateNode(block->nodes[i]);
            
            current_block = last_block;
            
            
        } break;
            
        case SASTTypeFunctionDef: {
            
            SASTFunctionDefinition* def = (SASTFunctionDefinition*)node;
            
            // Register the block that we have for the name of the function
            script_functions[def->identifier.string] = def;
            
        } break;
            
        default:
            
            break;
            
    }
    
    return 0;
    
}

SVariable SVM::declareVariable(std::string& type) {

    SVariable to_declare;

    // If we have a type for this, declare it
    if (STypeRegistry::instance()->factories.count(type))
        to_declare.value = STypeRegistry::instance()->factories[type]->createObject();
    else throw std::runtime_error("Unknown type");
    
    to_declare.type = type;
    
    return to_declare;

}

SVariable* SVM::resolveVarible(std::string name) {
    
    // Go through the scopes and try to find the variable
    SBlock* search_block = current_block;
    
    std::stringstream stream(name);
    std::string var_name;
    std::getline(stream, var_name, '.');
    
    SVariable* var = nullptr;
    
    while (search_block) {
        
        if (search_block->variables.count(var_name))
            var = &search_block->variables[var_name];
        
        // Check the next block
        if (!strcmp(typeid(search_block->owner).name(), typeid(SBlock).name()))
            search_block = (SBlock*)search_block->owner;
        else search_block = nullptr;
        
    }
    
    if (global_variables.count(var_name) && !var)
        var = &global_variables[var_name];
    
    // If we didnt find it, we didnt find if
    if (!var)
        return nullptr;
    
    // Now we check if we have members
    std::string member_name;
    SVariable* member = new SVariable();
    member->type = var->type;
    member->value = var->value;
    
    while (std::getline(stream, member_name, '.')) {
    
        // See if this has a varaible that can be resolved
        if (STypeRegistry::instance()->variable_lookups.count(member->type)) {
            
            // Attempt to resolve the member
            SVariable new_member = STypeRegistry::instance()->getMemeber(member, member_name);
            if (new_member.type.length()) {
            
                member->value = new_member.value;
                member->type = new_member.type;
                
            } else return nullptr;
        
        } else { return nullptr; }
    
    }
    
    // Check if we found a member or not
	// TODO, smart pointer
    if (member->type.length())
        return member;
    else {
    
        delete member;
        return var;
        
    }
    
}

