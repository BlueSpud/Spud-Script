//
//  SVM.cpp
//  SpudScript
//
//  Created by Logan Pazol on 12/31/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SVM.hpp"

void SVM::executeCode(std::vector<SVMNode*> nodes) {

    for (int i = 0; i < nodes.size(); i++)
        evaluateNode(nodes[i]);

}

void* SVM::evaluateNode(SVMNode* node) {

    switch (node->node_type) {
            
        case STypeExpression: {
            
            // Get the numbers
            SVMExpression* expression = (SVMExpression*)node;
			return evaluateExpression(expression).value;
            
        }  break;
			
		case STypeLoop: {
			
			// Get the numbers
			SVMLoop* loop = (SVMLoop*)node;
			evaluateLoop(loop);
			
		}  break;
			
		case STypeIfExpression: {
			
			SVMIfStatement* if_statement = (SVMIfStatement*)node;
			evaluateIf(if_statement);
			
		} break;
            
        case STypeDeclaration: {
            
            SVMDeclaration* declaration = (SVMDeclaration*)node;
			return evaluateDeclaration(declaration);
			
        } break;
            
        case STypeAssignment: {
            
            SVMAssignment* assignment = (SVMAssignment*)node;
			evaluateAssignment(assignment);
			
        } break;
            
        case STypeFunctionCall: {
            
            SVMFunctionCall* call = (SVMFunctionCall*)node;
			evaluateFuncitonCall(call);
			
        } break;
            
        case STypeBlock: {
            
            SVMBlock* block = new SVMBlock(*(SVMBlock*)node);
			evaluateBlock(block);
            
        } break;
            
        case STypeFunctionDef: {
			
			// Register the block that we have for the name of the function
            SVMFunctionDefinition* def = (SVMFunctionDefinition*)node;
            script_functions[def->identifier] = def;
            
        } break;
			
		case STypeReturn: {
			
			// Register the block that we have for the name of the function
			SVMReturn* return_node = (SVMReturn*)node;
			evaluateReturn(return_node);
			
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
    SVMBlock* search_block = current_block;
    
    std::stringstream stream(name);
    std::string var_name;
    std::getline(stream, var_name, '.');
    
    SVariable* var = nullptr;
    
    while (search_block) {
        
        if (search_block->variables.count(var_name))
            var = &search_block->variables[var_name];
        
        // Check the next block
		if (search_block->owner && search_block->owner->node_type == STypeBlock && !search_block->func_override)
            search_block = (SVMBlock*)search_block->owner;
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

SVariable SVM::evaluateExpression(SVMExpression* expression) {
	
	std::vector<SVariable> vars;
	
	for (int i = 0; i < expression->nodes.size(); i++) {
		
		switch (expression->nodes[i]->type) {
				
			case SExpressionNodeTypeLiteral: {
				
					SExpressionNodeLiteral* literal_node = (SExpressionNodeLiteral*)expression->nodes[i];
				
					vars.push_back(SVariable());
					SVariable& literal = vars.back();
				
					// Copy the data
					literal.type = literal_node->literal_type;
				
					literal.value = calloc(1,literal_node->size);
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
						var.value = calloc(1,size);
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
				
			case SExpressionNodeTypeFunction: {
				
				// Evaluate the function call and get the return value, it may be null
				SExpressionNodeFunction* func_node = (SExpressionNodeFunction*)expression->nodes[i];
				
				SVariable var;
				
				// Call the funtion and get the return value
				try {
					
					evaluateFuncitonCall(func_node->call);
					
				} catch (SVariable _var) { var = _var; }
				
				// Check that this expression was valid calling the function
				if (var.value != nullptr) {
					
					vars.push_back(var);
					
				} else if (expression->nodes.size() != 1) {
					
					// Something was done with the function, it wont work because there was no return value, not even 0
					throw std::runtime_error("Function call did not return a value, could not operate on void");
					
				} else return SVariable();
				
			}
				
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
	
		void* cast = SOperatorRegistry::instance()->performCast(&result, expression->destination_type);
		free(result.value);
		result.value = cast;
		
		result.type = expression->destination_type;
		
	}
	
	return result;
	
}

void SVM::evaluateBlock(SVMBlock* block) {
	
	SVMBlock* last_block = current_block;
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

void* SVM::evaluateFuncitonCall(SVMFunctionCall* call) {
	
	// Check if there is a script function
	if (script_functions.count(call->identifier)) {
		
		// Clear the variables from the lVM function call
		SVMFunctionDefinition* def = script_functions[call->identifier];
		SVMBlock* block = script_functions[call->identifier]->block;
		block->func_override = true;
		
		// Define the new variables of the argument types
		if (call->expressions.size() == def->args.size()) {
			
			for (int i = 0; i < call->expressions.size(); i++) {
				
				// Check type matching TEMP, not done
				block->variables[def->args[i]->identifier] = declareVariable(def->args[i]->type);
				
				call->expressions[i]->destination_type = def->args[i]->type;
				void* expression_result = evaluateNode(call->expressions[i]);
				
				STypeRegistry::instance()->performCopy(block->variables[def->args[i]->identifier].value, expression_result, def->args[i]->type);
				free(expression_result);
				
			}
			
			// Call the block, if there is a return node this will throw and something will have to catch an SVariable
			evaluateNode(block);
			
		} else {
			
			throw std::runtime_error("Invalid number of arguments, got " + std::to_string(call->expressions.size()) + " expected " + std::to_string(def->args.size()));
			return 0;
			
		}
		
	} else if (cpp_functions.count(call->identifier)) {
		
		// Call C++ function
		std::vector<void*> params;
		
		for (int i = 0; i < call->expressions.size(); i++) {
			
			call->expressions[i]->destination_type = STypeRegistry::hashString(cpp_functions[call->identifier]->signature[i]);
			void* expression_result = evaluateNode(call->expressions[i]);
			params.push_back(expression_result);
			
		}
		
		cpp_functions[call->identifier]->operator()(params);
		
		// Make sure that we free what we allocated
		for (int i = 0; i < call->expressions.size(); i++)
			free(params[i]);
		
	} else {
		
		// Neither a script function existed nor a C++ one did
		throw std::runtime_error("Unknown Function: " + call->identifier);
		return 0;
		
	}

	return nullptr;
}

void SVM::evaluateLoop(SVMLoop* loop) {
	
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
		
		SVMLoopFor* loop_f = (SVMLoopFor*)loop;
		
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

void* SVM::evaluateDeclaration(SVMDeclaration* declaration) {
	
	if (!current_block) {
		
		// Declare a variable in the global scope
		if (!global_variables.count(declaration->identifier)) {
			
			global_variables[declaration->identifier] = declareVariable(declaration->type);
			return &global_variables[declaration->identifier];
			
		} else {
			
			throw std::runtime_error("Redefinition of " + declaration->identifier);
			return nullptr;
			
		}
		
	} else {
		
		// Make sure that the variable is not already defined
		SVariable* variable = resolveVariable(declaration->identifier);
		if (!variable) {
			
			// Declare a variable in a scope
			current_block->variables[declaration->identifier] = declareVariable(declaration->type);
			return &current_block->variables[declaration->identifier];
			
		} else {
			
			throw std::runtime_error("Redefinition of " + declaration->identifier);
			return nullptr;
			
		}
		
	}
	
}

void SVM::evaluateAssignment(SVMAssignment* assignment) {
	
	// If we need to declare, declare
	if (assignment->declaration) {
		
		// Evaluate the expression
		assignment->expression->destination_type = assignment->declaration->type;
		void* expression_result = evaluateNode(assignment->expression);
		
		// Check that we got a value
		if (!expression_result)
			throw std::runtime_error("Expression returned null");
		
		// COPY the result
		SVariable* variable = (SVariable*)evaluateNode(assignment->declaration);
		STypeRegistry::instance()->performCopy(variable->value, expression_result, variable->type);
		free(expression_result);
		
	} else {
		
		// Variable was already declared (hopefully) check if we have
		SVariable* variable = resolveVariable(assignment->identifier);
		if (variable) {
			
			assignment->expression->destination_type = variable->type;
			void* expression_result = evaluateNode(assignment->expression);
			
			// Check that we got a value
			if (!expression_result)
				throw std::runtime_error("Expression returned null");
			
			// COPY the result
			STypeRegistry::instance()->performCopy(variable->value, expression_result, variable->type);
			free(expression_result);
			
		} else {
			
			// Variable was not declared
			throw std::runtime_error(assignment->identifier + " was not defined in this scope");
			
		}
		
	}
	
}

void SVM::evaluateIf(SVMIfStatement* if_statement) {
	
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

void SVM::evaluateReturn(SVMReturn* return_node) {
	
	// First figure out if this was a valid return node
	SVMBlock* search_block = current_block;
	
	// Cannot return in the global scope
	if (!search_block)
		throw std::runtime_error("Unexpected return");
		
	while (search_block) {
		
		if (search_block->func_override)
			break;
		else if (search_block->owner && search_block->owner->node_type == STypeBlock)
			search_block = (SVMBlock*)search_block->owner;
		else {
			
			// Reached the global scope, cant return from that
			throw std::runtime_error("Unexpected return");
			
		}
		
	}
	
	// We have verified that this is being called inside a function, so we can evaluate the expression and then throw it
	SVariable result;
	if (return_node->expression)
		result = evaluateExpression(return_node->expression);
	
	throw result;
	
}
