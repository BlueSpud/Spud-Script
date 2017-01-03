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
                    
                    if (variables.count(expresison->tokens[i].string))
                        numbers.push_back(variables[expresison->tokens[i].string].value);
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
            variables[declaration->identifier.string].value = 0.0;
            variables[declaration->identifier.string].type = declaration->type.string;
            
        } break;
            
        case SASTTypeAssignment: {
            
            SASTAssignment* assignment = (SASTAssignment*)node;
            
            // Evaluate the expression
            float expression_result = evaluateNode(assignment->expression);
            
            // If we need to declare, declare
            if (assignment->declaration) {
                
                evaluateNode(assignment->declaration);
                variables[assignment->declaration->identifier.string].value = expression_result;
                castVariable(variables[assignment->declaration->identifier.string].type, assignment->declaration->identifier.string);
                
            } else {
                
                if (variables.count(assignment->identifier.string)) {
                    
                    variables[assignment->identifier.string].value = expression_result;
                    castVariable(variables[assignment->identifier.string].type, assignment->identifier.string);
                    
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
            
            // Go through all the nodes in the block and evaluate them
            for (int i = 0; i < block->nodes.size(); i++)
                evaluateNode(block->nodes[i]);
            
            
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

void SVM::castVariable(std::string type, std:: string variable) {
    
    // See what type it is and cast if
    if (!type.compare("int"))
        variables[variable].value = (int)variables[variable].value;
    
}
