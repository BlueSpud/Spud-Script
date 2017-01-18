//
//  SASTNodes.h
//  SpudScript
//
//  Created by Logan Pazol on 12/31/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SASTNodes_h
#define SASTNodes_h

#include "STypes.hpp"
#include "SLexer.hpp"

enum SASTNodeType {
    
    SASTTypeExpression,
    SASTTypeDeclaration,
    SASTTypeAssignment,
    SASTTypeFunctionCall,
    SASTTypeBlock,
    SASTTypeFunctionDef,
	SASTTypeIfExpression,
	SASTTypeLoop
    
};

struct SASTNode { SASTNodeType node_type; };

// Forward declaration of an expression
struct SASTExpression;

struct SASTDeclaration : public SASTNode { SToken type, identifier; };

struct SASTAssignment : public SASTNode {
    
    SASTDeclaration* declaration;
    SASTExpression* expression;
    SToken identifier;
    
};

struct SASTFunctionCall : public SASTNode {
    
    SToken identifier;
    std::vector<SASTExpression*> expressions;

};

struct SBlock : public SASTNode {
    
    std::vector<SASTNode*> nodes;
    SASTNode* owner = nullptr;
    
    std::map<std::string, SVariable> variables;
    
};

struct SASTFunctionDefinition : public SASTNode {
    
    SToken identifier;
    SBlock* block;
    
    std::vector<SASTDeclaration*> args;
    
};

struct SASTIfStatement : public SASTNode {
	
	SBlock* block;
	SBlock *parent_block;
	
	SASTExpression* expression;
	SASTNode* else_node;
	
	SASTIfStatement* parent_if;
	
};

enum SASTLoopType {
	
	SASTLoopTypeWhile,
	SASTLoopTypeFor
	
};

struct SASTLoop : public SASTNode {
	
	SBlock* block;
	SBlock* parent_block;
	
	SASTExpression* expression;
	SASTLoop* parent_loop;
	
	SASTLoopType loop_type;
	
};

struct SASTLoopFor : public SASTLoop {
	
	SASTAssignment* initial_assign;
	SASTAssignment* increment;
	
};

#endif /* SASTNodes_h */
