//
//  SAST.cpp
//  SpudScript
//
//  Created by Logan Pazol on 12/28/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SAST.hpp"

std::vector<SASTNode*> SAST::parseTokens(std::vector<SToken>& tokens) {

    std::vector<SASTNode*> nodes;
    
    // Store the block of code that we are working on
    SBlock* current_block = nullptr;
    
    // Go through every token
    for (int i = 0; i < tokens.size(); i++) {
		
		// See if there was a semicolon, ends the line
        if (!parseSemi(tokens, i))
            return nodes;
		
		// Parse a }, ending a block
		if (!parseStartBlock(tokens, i, current_block))
			return nodes;
		
		// Parse a }, ending a block
        if (!parseEndBlock(tokens, i, nodes, current_block))
            return nodes;
        
        
        // If we are in a block, thats where everything should be done
        std::vector<SASTNode*>* node_place = &nodes;
        if (current_block)
            node_place = &current_block->nodes;
		
		
		// Parse a function call
		SASTFunctionCall* func_call = parseFunctionCall(tokens, i);
		if (func_call)
			node_place->push_back(func_call);
		
		
        // Parse an expression
		SASTExpression* expression_node = parseExpression(tokens, i);
		if (expression_node)
			node_place->push_back(expression_node);
		

        // Parse a declaration
		SASTDeclaration* decl_node = parseDeclaration(tokens, i);
		if (decl_node)
			node_place->push_back(decl_node);
		
        
        // Parse a function definition
		SASTFunctionDefinition* func_def = parseFunctionDef(tokens, i, current_block);
        if (func_def)
            nodes.push_back(func_def);
		
		
		// Parse an assignement
		if (!parseAssignment(tokens, i, node_place))
			return nodes;
        
    }
    
	if (current_block)
		throw std::runtime_error("Missing }");
	
    return nodes;
    
}

bool SAST::parseSemi(PARSE_ARGS) {

    // If this is a semi colon, we remove every token behind
    if (tokens[i].type == STokenTypeSemi) {
            
        std::vector<SToken>::iterator start = tokens.begin();
        std::vector<SToken>::iterator end = tokens.begin() + i;
            
        tokens.erase(start, end);
        tokens.shrink_to_fit();
        i = 0;
        
        return true;
            
    }

    return true;

}

bool SAST::parseStartBlock(PARSE_ARGS, SBlock*& current_block) {
	
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
	
	return true;
	
}

bool SAST::parseEndBlock(PARSE_ARGS, std::vector<SASTNode*>& nodes, SBlock*& current_block) {

    if (tokens[i].type == STokenTypeCloseBrack) {

        // Need to be working on a block before we do anything
        if (!current_block) {
                
			throw std::runtime_error("Unexpected }");
            return false;
                
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
    
    return true;

}

SASTExpression* SAST::parseExpression(PARSE_ARGS) {
	
	SASTExpression* expression_node = new SASTExpression();
	expression_node->node_type = SASTTypeExpression;
	bool last_operator = true;
	
	while (i < tokens.size()) {
		
		if ((tokens[i].type == STokenTypeNumber || tokens[i].type == STokenTypeIdentifier) && last_operator) {
			
			expression_node->tokens.push_back(tokens[i]);
			i++;
			last_operator = false;
			
		} else if (tokens[i].type == STokenTypeOperator && tokens[i].string.compare("=") && !last_operator) {
			
			expression_node->tokens.push_back(tokens[i]); i++; last_operator = true;
		
		} else break;
	
	}
	
	// Check if the parse was successful
	if (expression_node->tokens.size())
		return expression_node;
	else {
		
		delete expression_node;
		return nullptr;
		
	}

}

SASTDeclaration* SAST::parseDeclaration(PARSE_ARGS) {
	
	SASTDeclaration* decl_node = new SASTDeclaration();
	decl_node->node_type = SASTTypeDeclaration;
	if (tokens[i].type == STokenTypeType) {
		
		i++;
		if (i < tokens.size() && tokens[i].type == STokenTypeIdentifier) {
			
			decl_node->type = tokens[i - 1];
			decl_node->identifier = tokens[i];
			i++;
			
		} else {
			
			delete decl_node;
			throw std::runtime_error("Expected identifier");
		
		}
		
	}
	
	if (decl_node->identifier.string.length())
		return decl_node;
	else {
		
		delete decl_node;
		return nullptr;
		
	}
}

SASTFunctionCall* SAST::parseFunctionCall(PARSE_ARGS) {
	
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
				
				SASTExpression* expression_node = parseExpression(tokens, i);
				if (expression_node)
					call_node->expressions.push_back(expression_node);
				else break;
				
			}
			
			// Definately was a function
			if (i < tokens.size() && tokens[i].type == STokenTypeCloseParen) {
				
				// Correctly parsed a function call
				return  call_node;
				
				
			} else {
				
				delete call_node;
				throw std::runtime_error(") Expected");
			}
			
		}
		
	}
	
	// Parse was not successful
	delete call_node;
	return nullptr;
	
}

// Declaration to parse an expression specifically for an assignment
#define ASS_EXP_PARSE i++; SASTExpression* expression_node = parseExpression(tokens, i); if (expression_node) assign_node->expression = expression_node; else { throw std::runtime_error("Expected expression"); return false; }

bool SAST::parseAssignment(PARSE_ARGS, std::vector<SASTNode*>* node_place) {
	
	// Parse an Assignment
	SASTAssignment* assign_node = new SASTAssignment();
	assign_node->node_type = SASTTypeAssignment;
	
	if (!tokens[i].string.compare("=")) {
		
		// Parsed an equal sign
		// Two == is another operator, ignore that
		if (i + 1 < tokens.size() && i + 1 < tokens.size() && tokens[i + 1].string.compare("=")) {
			
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
	
	if (assign_node->identifier.string.length())
		node_place->push_back(assign_node);
	else if (assign_node->declaration) node_place->push_back(assign_node);
	
	return true;
	
}

SASTFunctionDefinition* SAST::parseFunctionDef(PARSE_ARGS, SBlock*& current_block) {
	
	// Check for a function definition
	SASTFunctionDefinition* func_def = new SASTFunctionDefinition();
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
					
					SASTDeclaration* decl_node = parseDeclaration(tokens, i);
					if (decl_node)
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
						delete func_def;
						throw std::runtime_error("{ expected");
						
					}
					
				} else {
					
					// Missing end of args
					delete func_def;
					throw std::runtime_error(") Expected");
					
				}
				
			} else {
				
				// Missing start of args
				delete func_def;
				throw std::runtime_error("( Expected");
				
			}
			
		} else {
			
			// Missing identifier
			delete func_def;
			throw std::runtime_error("Expected identifier");
			
		}
		
	}
	
	if (func_def->identifier.string.length())
		return func_def;
	else {
		
		delete func_def;
		return nullptr;
		
	}
	
	
}
