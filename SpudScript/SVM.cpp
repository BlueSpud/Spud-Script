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

float SVM::evaluateNode(SASTNode* node) {

    switch (node->node_type) {
            
        case SASTTypeExpression: {
            
            // Get the numbers
            SASTExpression* expresison = (SASTExpression*)node;
            std::vector<double> numbers;
            
            for (int i = 0; i < expresison->tokens.size(); i++)
                if (expresison->tokens[i].type == STokenTypeNumber) {
                    
                    numbers.push_back(atof(expresison->tokens[i].string.c_str()));
                    
                } else if (expresison->tokens[i].type == STokenTypeIdentifier) {
                    
                    SVariable* variable = resolveVarible(expresison->tokens[i].string);
                    if (variable)
                        numbers.push_back(variable->value);
                    else {
                        
                        std::cout << expresison->tokens[i].string << " was not defined in this scope" << std::endl;
                        return 0;
                        
                    }
                    
                }
            
            // Go through again, evaluating operators
            int number = -1;
            
            for (int i = 0; i < expresison->tokens.size(); i++) {
                if (expresison->tokens[i].type == STokenTypeOperator && (!expresison->tokens[i].string.compare("*") || !expresison->tokens[i].string.compare("/") || !expresison->tokens[i].string.compare("%"))) {
                        
                    float new_number = 0.0;
                        
                    // Perform adition or multiplication
                    if (!expresison->tokens[i].string.compare("*"))
                        new_number = numbers[number] * numbers[number + 1];
                        
                    if (!expresison->tokens[i].string.compare("/"))
                        new_number = numbers[number] / numbers[number + 1];
                    
                    if (!expresison->tokens[i].string.compare("%"))
                        new_number = (int)numbers[number] % (int)numbers[number + 1];
                    
                    // Set and remove
                    numbers[number] = new_number;
                    std::vector<double>::iterator itterator = numbers.begin() + number + 1;
                    numbers.erase(itterator);
                    numbers.shrink_to_fit();
                        
                    // Incremenent i to skip the next number
                    i++;
                        
                        
                } else if (expresison->tokens[i].type == STokenTypeNumber || expresison->tokens[i].type == STokenTypeIdentifier)
                    number++;
            }
            
            double result = numbers[0];
            number = 1;
            
            // Do addition and subtraction
            for (int i = 0; i < expresison->tokens.size(); i++) {
                if (expresison->tokens[i].type == STokenTypeOperator && (!expresison->tokens[i].string.compare("+") || !expresison->tokens[i].string.compare("-"))) {
                    
                    if (!expresison->tokens[i].string.compare("+"))
                        result = result + numbers[number];
                    
                    if (!expresison->tokens[i].string.compare("-"))
                        result = result - numbers[number];
                    
                    number++;
                    
                }
                
            }
            
            return result;
            
        }  break;
            
        case SASTTypeDeclaration: {
            
            SASTDeclaration* declaration = (SASTDeclaration*)node;
            
            if (!current_block) {
                
                // Declare a variable in the global scope
                global_variables[declaration->identifier.string].value = 0.0;
                global_variables[declaration->identifier.string].type = declaration->type.string;
                
            } else {
                
                // Make sure that the variable is not already defined
                SVariable* variable = resolveVarible(declaration->identifier.string);
                
                if (!variable) {
                
                    // Declare a variable in a scope
                    current_block->variables[declaration->identifier.string].value = 0.0;
                    current_block->variables[declaration->identifier.string].type = declaration->type.string;
                    
                } else {
                    
                    std::cout << "Redefinition of " << declaration->identifier.string << std::endl;
                    return 0;
                    
                }
                
            }
            
        } break;
            
        case SASTTypeAssignment: {
            
            SASTAssignment* assignment = (SASTAssignment*)node;
            
            // Evaluate the expression
            float expression_result = evaluateNode(assignment->expression);
            
            // If we need to declare, declare
            if (assignment->declaration) {
                
                evaluateNode(assignment->declaration);
                SVariable* variable = resolveVarible(assignment->declaration->identifier.string);
                
                variable->value = expression_result;
                castVariable(variable->type, assignment->declaration->identifier.string);
                
            } else {
                
                SVariable* variable = resolveVarible(assignment->identifier.string);
                if (variable) {
                    
                    variable->value = expression_result;
                    castVariable(variable->type, assignment->identifier.string);
                    
                } else {
                    
                    // Variable was not declared
                    std::cout << assignment->identifier.string << " was not defined in this scope\n";
                    
                }
                
            }
            
        } break;
            
        case SASTTypeFunctionCall: {
            
            SASTFunctionCall* function = (SASTFunctionCall*)node;
            
            // Print out args
            for (int i = 0; i < function->expressions.size(); i++) {
                std::cout << function->expressions[i]->tokens[0].string << std::endl;
            }
            
            // Check if there is a script function
            if (script_functions.count(function->identifier.string))
                evaluateNode(script_functions[function->identifier.string]);
            else if (functions.count(function->identifier.string)) {
                
                // Call C++ function
                functions[function->identifier.string]();
                
            } else {
            
                // Neither a script function existed nor a C++ one did
                std::cout << "Unknown Function: " << function->identifier.string << std::endl;
                
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
            
            SFunctionDefinition* def = (SFunctionDefinition*)node;
            
            // Register the block that we have for the name of the function
            script_functions[def->identifier.string] = def->block;
            
        } break;
            
        default:
            
            break;
            
    }
    
    return 0;
    
}

SVariable* SVM::resolveVarible(std::string name) {
    
    // Go through the scopes and try to find the variable
    SBlock* search_block = current_block;
    
    while (search_block) {
        
        if (search_block->variables.count(name))
            return &search_block->variables[name];
        
        // Check the next block
        if (!strcmp(typeid(search_block->owner).name(), typeid(search_block).name()))
            search_block = (SBlock*)search_block->owner;
        else search_block = nullptr;
        
    }
    
    if (global_variables.count(name))
        return &global_variables[name];
    
    return nullptr;
    
}

void SVM::castVariable(std::string type, std:: string name) {
    
    SVariable* variable = resolveVarible(name);
    
    // See what type it is and cast if
    if (!type.compare("int"))
        variable->value = (int)variable->value;
    
}

