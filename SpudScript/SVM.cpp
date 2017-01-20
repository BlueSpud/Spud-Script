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
            
        case STypeExpression: {
            
            // Get the numbers
            SASTExpression* expression = (SASTExpression*)node;
			return evaluateExpression(expression).value;
            
        }  break;
			
		case STypeLoop: {
			
			// Get the numbers
			SASTLoop* loop = (SASTLoop*)node;
			evaluateLoop(loop);
			
		}  break;
			
		case STypeIfExpression: {
			
			SASTIfStatement* if_statement = (SASTIfStatement*)node;
			evaluateIf(if_statement);
			
		} break;
            
        case STypeDeclaration: {
            
            SASTDeclaration* declaration = (SASTDeclaration*)node;
			return evaluateDeclaration(declaration);
			
        } break;
            
        case STypeAssignment: {
            
            SASTAssignment* assignment = (SASTAssignment*)node;
			evaluateAssignment(assignment);
			
        } break;
            
        case STypeFunctionCall: {
            
            SASTFunctionCall* call = (SASTFunctionCall*)node;
			evaluateFuncitonCall(call);
			
        } break;
            
        case STypeBlock: {
            
            SBlock* block = new SBlock(*(SBlock*)node);
			evaluateBlock(block);
            
        } break;
            
        case STypeFunctionDef: {
			
			// Register the block that we have for the name of the function
            SASTFunctionDefinition* def = (SASTFunctionDefinition*)node;
            script_functions[def->identifier.string] = def;
            
        } break;
            
        default:
            
            break;
            
    }
    
    return 0;
    
}

SVariable SVM::declareVariable(size_t type) {

    SVariable to_declare;
	
    // If we have a type for this, declare it
	if (STypeRegistry::instance()->factories.count(type))
        to_declare.value = STypeRegistry::instance()->factories[type]->createObject();
    else throw std::runtime_error("Unknown type");
    
    to_declare.type = type;
    
    return to_declare;

}

SVariable* SVM::resolveVariable(std::string name) {
    
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
		if (search_block->owner && search_block->owner->node_type == STypeBlock && !search_block->func_override)
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
            if (new_member.type) {
            
                member->value = new_member.value;
                member->type = new_member.type;
                
            } else return nullptr;
        
        } else { return nullptr; }
    
    }
    
    // Check if we found a member or not
	// TODO, smart pointer
    if (member->type)
        return member;
    else {
    
        delete member;
        return var;
        
    }
    
}

SVariable SVM::evaluateExpression(SASTExpression* expression) {
	
	std::vector<SVariable> vars;
	
	for (int i = 0; i < expression->nodes.size(); i++) {
		
		switch (expression->nodes[i]->type) {
				
			case SExpressionNodeTypeLiteral: {
				
					SExpressionNodeLiteral* literal_node = (SExpressionNodeLiteral*)expression->nodes[i];
				
					vars.push_back(SVariable());
					SVariable& literal = vars.back();
				
					// Copy the data
					literal.type = literal_node->literal_type;
				
					literal.value = malloc(literal_node->size);
					STypeRegistry::instance()->performCopy(literal.value, literal_node->value, literal_node->literal_type);
    
				} break;
				
			case SExpressionNodeTypeVariable: {
				
					SExpressionNodeVariable* variable_node = (SExpressionNodeVariable*)expression->nodes[i];
					SVariable* resolved_var = resolveVariable(variable_node->var_name);
				
					if (resolved_var) {
					
						vars.push_back(SVariable());
						SVariable& var = vars.back();
						
						// Copy the data
						size_t size = STypeRegistry::instance()->getTypeSize(resolved_var->type);
						var.type = resolved_var->type;
						var.value = malloc(size);
						STypeRegistry::instance()->performCopy(var.value, resolved_var->value, resolved_var->type);
					
					} else {
						
						// Couln't find the variable
						throw std::runtime_error(variable_node->var_name + " was not defined in this scope");
						
					}
				
				
				} break;
				
			case SExpressionNodeTypeExpression: {
				
					// We evaluate the expression
					SExpressionNodeExpression* expression_node = (SExpressionNodeExpression*)expression->nodes[i];
					vars.push_back(evaluateExpression(expression_node->expression));
    
				} break;
				
			default:
				break;
				
		}
		
		
	}
	
	// Go through again, evaluating operators
	int var = -1;
	
	// Compare operators
	for (int i = 0; i < expression->nodes.size(); i++) {
		if (expression->nodes[i]->type == SExpressionNodeTypeOperator) {
			
			SExpressionNodeOperator* node = (SExpressionNodeOperator*)expression->nodes[i];
			
			if (!node->op.compare("==") || !node->op.compare("!=") || !node->op.compare("<") || !node->op.compare(">") || !node->op.compare(">=") || !node->op.compare("<=")) {
				
				void* new_value = SOperatorRegistry::instance()->performOperation(&vars[var], &vars[var + 1], node->op);
				
				// Set and remove
				vars[var].value = new_value;
				
				// Free the value and delete
				free(vars[var + 1].value);
				std::vector<SVariable>::iterator itterator = vars.begin() + var + 1;
				vars.erase(itterator);
				vars.shrink_to_fit();
				
				// Incremenent i to skip the next number
				i++;
				
			}
			
		} else var++;
	}
	
	// Reset var
	var = -1;
	
	// Multiplication and division
	for (int i = 0; i < expression->nodes.size(); i++) {
		if (expression->nodes[i]->type == SExpressionNodeTypeOperator) {
			
			SExpressionNodeOperator* node = (SExpressionNodeOperator*)expression->nodes[i];
			
			if (!node->op.compare("*") || !node->op.compare("/") || !node->op.compare("%")) {
			
				void* new_value = SOperatorRegistry::instance()->performOperation(&vars[var], &vars[var + 1], node->op);
			
				// Set and remove
				vars[var].value = new_value;
			
				// Free the value and delete
				free(vars[var + 1].value);
				std::vector<SVariable>::iterator itterator = vars.begin() + var + 1;
				vars.erase(itterator);
				vars.shrink_to_fit();
			
				// Incremenent i to skip the next number
				i++;
			
			}
	
		} else var++;
	}
	
	SVariable& result = vars[0];
	var = 1;
	
	// Do addition and subtraction
	for (int i = 0; i < expression->nodes.size(); i++) {
		if (expression->nodes[i]->type == SExpressionNodeTypeOperator) {
			
			SExpressionNodeOperator* node = (SExpressionNodeOperator*)expression->nodes[i];
			
			if (!node->op.compare("+") || !node->op.compare("-")) {
			
				void* new_value = SOperatorRegistry::instance()->performOperation(&result, &vars[var], node->op);
			
				// Free allocation
				free(vars[var].value);
				result.value = new_value;
				var++;
			
			}
		
		}
		
	}
	
	// Do a cast if we need to
	if (result.type != expression->destination_type && expression->destination_type) {
	
		void* casted = SOperatorRegistry::instance()->performCast(&result, expression->destination_type);
		free(result.value);
		result.value = casted;
		
		result.type = expression->destination_type;
		
	}
	
	return result;
	
}

void SVM::evaluateBlock(SBlock* block) {
	
	SBlock* last_block = current_block;
	current_block = block;
	current_block->owner = last_block;
	
	// Go through all the nodes in the block and evaluate them
	for (int i = 0; i < block->nodes.size(); i++)
		evaluateNode(block->nodes[i]);
	
	// Delete every variable in the block
	std::map<std::string, SVariable>::iterator i = block->variables.begin();
	while (i != block->variables.end()) {
		
		// Make sure to free the memory
		free(i->second.value);
		block->variables.erase(i);
		i = block->variables.begin();
		
	}
	
	current_block = last_block;
	
}

void* SVM::evaluateFuncitonCall(SASTFunctionCall* call) {
	
	// Check if there is a script function
	if (script_functions.count(call->identifier.string)) {
		
		// Clear the variables from the last function call
		SASTFunctionDefinition* def = script_functions[call->identifier.string];
		SBlock* block = script_functions[call->identifier.string]->block;
		block->func_override = true;
		
		// Define the new variables of the argument types
		if (call->expressions.size() == def->args.size()) {
			
			for (int i = 0; i < call->expressions.size(); i++) {
				
				// Check type matching TEMP, not done
				block->variables[def->args[i]->identifier.string] = declareVariable(STypeRegistry::hashString(def->args[i]->type.string));
				
				call->expressions[i]->destination_type = STypeRegistry::hashString(def->args[i]->type.string);
				void* expression_result = evaluateNode(call->expressions[i]);
				
				STypeRegistry::instance()->performCopy(block->variables[def->args[i]->identifier.string].value, expression_result, STypeRegistry::hashString(def->args[i]->type.string));
				free(expression_result);
				
			}
			
			// Call the function
			evaluateNode(block);
			
		} else {
			
			throw std::runtime_error("Invalid number of arguments, got " + std::to_string(call->expressions.size()) + " expected " + std::to_string(def->args.size()));
			return 0;
			
		}
		
	} else if (cpp_functions.count(call->identifier.string)) {
		
		// Call C++ function
		std::vector<void*> params;
		
		for (int i = 0; i < call->expressions.size(); i++) {
			
			call->expressions[i]->destination_type = STypeRegistry::hashString(cpp_functions[call->identifier.string]->signature[i]);
			void* expression_result = evaluateNode(call->expressions[i]);
			params.push_back(expression_result);
			
		}
		
		cpp_functions[call->identifier.string]->operator()(params);
		
		// Make sure that we free what we allocated
		for (int i = 0; i < call->expressions.size(); i++)
			free(params[i]);
		
	} else {
		
		// Neither a script function existed nor a C++ one did
		throw std::runtime_error("Unknown Function: " + call->identifier.string);
		return 0;
		
	}

	return nullptr;
}

void SVM::evaluateLoop(SASTLoop* loop) {
	
	if (loop->loop_type == SLoopTypeWhile) {
		
		// While loop
		while (true) {
			
			// Evaluate the condition
			bool* expression_result = (bool*)evaluateExpression(loop->expression).value;
			
			if (!*expression_result) {
				
				// Cleanup and leave
				free(expression_result);
				break;
				
			}
			
			// Perform the block
			evaluateNode(loop->block);
			free(expression_result);
			
		}
		
	}
	
	if (loop->loop_type == SLoopTypeFor) {
		
		SASTLoopFor* loop_f = (SASTLoopFor*)loop;
		
		// Do initial asignment
		evaluateNode(loop_f->initial_assign);
		
		// While loop
		while (true) {
			
			// Evaluate the condition
			bool* expression_result = (bool*)evaluateExpression(loop_f->expression).value;
			
			if (!*expression_result) {
				
				// Cleanup and leave
				free(expression_result);
				break;
				
			}
			
			// Perform the block
			evaluateNode(loop_f->block);
			
			// Perform increment
			evaluateNode(loop_f->increment);
			
			free(expression_result);
			
		}
		
	}
	
}

void* SVM::evaluateDeclaration(SASTDeclaration* declaration) {
	
	if (!current_block) {
		
		// Declare a variable in the global scope
		if (!global_variables.count(declaration->identifier.string)) {
			
			global_variables[declaration->identifier.string] = declareVariable(STypeRegistry::hashString(declaration->type.string));
			return &global_variables[declaration->identifier.string];
			
		} else {
			
			throw std::runtime_error("Redefinition of " + declaration->identifier.string);
			return nullptr;
			
		}
		
	} else {
		
		// Make sure that the variable is not already defined
		SVariable* variable = resolveVariable(declaration->identifier.string);
		if (!variable) {
			
			// Declare a variable in a scope
			current_block->variables[declaration->identifier.string] = declareVariable(STypeRegistry::hashString(declaration->type.string));
			return &current_block->variables[declaration->identifier.string];
			
		} else {
			
			throw std::runtime_error("Redefinition of " + declaration->identifier.string);
			return nullptr;
			
		}
		
	}
	
}

void SVM::evaluateAssignment(SASTAssignment* assignment) {
	
	// If we need to declare, declare
	if (assignment->declaration) {
		
		// Evaluate the expression
		assignment->expression->destination_type = STypeRegistry::hashString(assignment->declaration->type.string);
		void* expression_result = evaluateNode(assignment->expression);
		
		// COPY the result
		SVariable* variable = (SVariable*)evaluateNode(assignment->declaration);
		STypeRegistry::instance()->performCopy(variable->value, expression_result, variable->type);
		free(expression_result);
		
	} else {
		
		// Variable was already declared (hopefully) check if we have
		SVariable* variable = resolveVariable(assignment->identifier.string);
		if (variable) {
			
			assignment->expression->destination_type = variable->type;
			void* expression_result = evaluateNode(assignment->expression);
			
			// COPY the result
			STypeRegistry::instance()->performCopy(variable->value, expression_result, variable->type);
			free(expression_result);
			
		} else {
			
			// Variable was not declared
			throw std::runtime_error(assignment->identifier.string + " was not defined in this scope");
			
		}
		
	}
	
}

void SVM::evaluateIf(SASTIfStatement* if_statement) {
	
	// Evaluate the expression, always returns a bool
	bool* expression_result = (bool*)evaluateNode(if_statement->expression);
	
	// Check if the result was true, and if it wasnt execute the else (if it exists)
	if (*expression_result)
		evaluateNode(if_statement->block);
	else if (if_statement->else_node)
		evaluateNode(if_statement->else_node);
	
	// Clean up
	free(expression_result);
	
}
