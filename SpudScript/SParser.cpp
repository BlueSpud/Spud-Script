//
//  SParser.cpp
//  SpudScript
//
//  Created by Logan Pazol on 12/28/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SParser.hpp"

std::vector<SASTNode*> SAST::parseTokens(std::vector<SToken>& tokens) {

    std::vector<SASTNode*> nodes;
    
    // Store the block of code that we are working on
    SBlock* current_block = nullptr;
    
    // Go through every token
    for (int i = 0; i < tokens.size(); i++) {
        
        // If this is a semi colon, we remove every token behind
        if (tokens[i].type == STokenTypeSemi) {
            
            std::vector<SToken>::iterator start = tokens.begin();
            std::vector<SToken>::iterator end = tokens.begin() + i;
            
            tokens.erase(start, end);
            tokens.shrink_to_fit();
            i = 0;
            
        }
        
        // Check for block information
        if (tokens[i].type == STokenTypeOpenBrack) {
            
            // If we dont have a block, this is a root level block
            if (!current_block) {
            
                current_block = new SBlock();
                current_block->node_type = SASTTypeBlock;
            
            } else {
                
                SBlock* new_block  = new SBlock();
                new_block->node_type = SASTTypeBlock;
                new_block->owner = current_block;
                
                // This is a block within a block, add the last block, make this one owned
                current_block->nodes.push_back(new_block);
                current_block = new_block;
                
            }
                
        }
        
        if (tokens[i].type == STokenTypeCloseBrack) {

            // Need to be working on a block before we do anything
            if (!current_block) {
                
                std::cout << "Unexpected }\n";
                return nodes;
                
            }
        
            // Otherwise we finish the block we were working on
            if (!current_block->owner) {
                
                nodes.push_back(current_block);
                current_block = nullptr;
                
            } else {
                
                // Check if we need to restore the owning block
                if (!strcmp(typeid(current_block->owner).name(), typeid(SBlock).name()))
                    current_block = (SBlock*)current_block->owner;
                else current_block = nullptr;
                
            }
            
        }
        
        // If we are in a block, thats where everything should be done
        std::vector<SASTNode*>* node_place = &nodes;
        if (current_block)
            node_place = &current_block->nodes;
        
        // Parse an expression
        #define EXP_PARSE SASTExpression* expression_node = new SASTExpression(); expression_node->node_type = SASTTypeExpression; bool last_operator = true; while (i < tokens.size()) { if ((tokens[i].type == STokenTypeNumber || tokens[i].type == STokenTypeIdentifier) && last_operator) { expression_node->tokens.push_back(tokens[i]); i++; last_operator = false; } else if (tokens[i].type == STokenTypeOperator && tokens[i].string.compare("=") && !last_operator) { expression_node->tokens.push_back(tokens[i]); i++; last_operator = true; } else break; }
        
        // Parse a function call
        SASTFunctionCall* call_node = new SASTFunctionCall();
        call_node->node_type = SASTTypeFunctionCall;
        if (tokens[i].type == STokenTypeIdentifier) {
            
            call_node->identifier = tokens[i];
            
            // See if we have an open brace
            if (i + 1 < tokens.size() && tokens[i + 1].type == STokenTypeOpenParen) {
                
                i = i + 2;
                
                // Parse the arguments
                while (i < tokens.size()) {
                    
                    EXP_PARSE
                    
                    if (expression_node->tokens.size())
                        call_node->expressions.push_back(expression_node);
                    else break;
                    
                }
                
                // Definately was a function
                if (i < tokens.size() && tokens[i].type == STokenTypeCloseParen) {
                    
                    // Correctly parsed a function call
                    node_place->push_back(call_node);
                    
                    
                } else {
                    
                    std::cout << ") Expected\n";
                    return nodes;
                }
                
            }
            
        }
        
        // Parse an expression
        {
        
            EXP_PARSE
            
            if (expression_node->tokens.size()) {
                node_place->push_back(expression_node);
            
            }
            
        }
        
        #define DEC_PARSE SASTDeclaration* decl_node = new SASTDeclaration(); decl_node->node_type = SASTTypeDeclaration; if (tokens[i].type == STokenTypeType) { i++; if (i < tokens.size() && tokens[i].type == STokenTypeIdentifier) { decl_node->type = tokens[i - 1]; decl_node->identifier = tokens[i]; i++; } else { std::cout << "expected identifier\n"; return nodes; } }
        
        // Parse a declaration
        {
            DEC_PARSE
        
            if (decl_node->identifier.string.length())
                node_place->push_back(decl_node);
        
        }
        
        // Check for a function definition
        SFunctionDefinition* func_def = new SFunctionDefinition();
        func_def->node_type = SASTTypeFunctionDef;
        if (!current_block && tokens[i].type == STokenTypeKeyword && !tokens[i].string.compare("func")) {
            
            // Get the identifier of the function
            if (i + 1 < tokens.size() && tokens[i + 1].type == STokenTypeIdentifier) {
                
                func_def->identifier = tokens[i + 1];
                
                // Make sure we have parens
                if (i + 2 < tokens.size() && tokens[i + 2].type == STokenTypeOpenParen) {
                    
                    // Incriment i
                    i = i + 3;
                    
                    // Parse the arguments
                    while (i < tokens.size()) {
                        
                        DEC_PARSE
                        
                        if (decl_node->identifier.string.length())
                            func_def->args.push_back(decl_node);
                        else break;
                        
                    }
                    
                    // Definately was a function
                    if (i < tokens.size() && tokens[i].type == STokenTypeCloseParen) {
                        
                        // Increment i
                        i++;
                        
                        // Look for the open token
                        if (i < tokens.size() && tokens[i].type == STokenTypeOpenBrack) {
                            
                            // Create a block for the funciton definition
                            func_def->block = new SBlock();
                            func_def->block->node_type = SASTTypeBlock;
                            func_def->block->owner = func_def;
                            current_block = func_def->block;
                        
                        } else {
                            
                            // Misisng open bracket
                            std::cout << "{ expected\n";
                            return nodes;
                            
                        }
                        
                    } else {
                        
                        // Missing end of args
                        std::cout << ") Expected\n";
                        return nodes;
                        
                    }
                    
                } else {
                    
                    // Missing start of args
                    std::cout << "( Expected\n";
                    return nodes;
                    
                }
                
            } else {
                
                // Missing identifier
                std::cout << "Expected identifier\n";
                return nodes;
                
            }
            
        }
        
        // Check if we had a function definition
        if (func_def->identifier.string.length())
            nodes.push_back(func_def);
        
        // Parse an Assignment
        SASTAssignment* assign_node = new SASTAssignment();
        assign_node->node_type = SASTTypeAssignment;
        #define ASS_EXP_PARSE i++; EXP_PARSE if (expression_node->tokens.size()) assign_node->expression = expression_node; else { std::cout << "Expected expression\n"; return nodes; }
        
        if (!tokens[i].string.compare("=")) {
            
            // Parsed an equal sign
            // Two == is another operator, ignore that
            if (i + 1 < tokens.size() && tokens[i + 1].string.compare("=")) {
            
                // Node is declaration
                // Previous token is identifier
                if (node_place->size() != 0 && (*node_place)[node_place->size() - 1]->node_type == SASTTypeDeclaration) {
                    
                    assign_node->declaration = (SASTDeclaration*)(*node_place)[node_place->size() - 1];
                    std::vector<SASTNode*>::iterator deleter = node_place->begin() + node_place->size() - 1;
                    
                    // Delete the node
                    node_place->erase(deleter);
                    node_place->shrink_to_fit();
                    
                    ASS_EXP_PARSE
                    
                    
                } else if (tokens[i - 1].type == STokenTypeIdentifier) {
                    
                    // This probably means the last one was an expression, remove that
                    if (node_place->size() != 0) {
                        
                        std::vector<SASTNode*>::iterator deleter = node_place->begin() + node_place->size() - 1;
                        node_place->erase(deleter);
                        node_place->shrink_to_fit();
                        
                    }
                        
                    assign_node->identifier = tokens[i - 1];
                    ASS_EXP_PARSE
                    
                }
                
            }
            
        }
        
        // N is only icrememented if we didnt steal the decl node
        if (assign_node->identifier.string.length())
            node_place->push_back(assign_node);
        else if (assign_node->declaration) node_place->push_back(assign_node);
        
    }
    
    if (current_block)
        std::cout << "Missing }\n";
    
    return nodes;
    
}
