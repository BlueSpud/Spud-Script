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

#include "SVMNodes.h"

struct SASTNode {
	
	SNodeType node_type;

	virtual SVMNode* compile() { return nullptr; };
	
};

// Forward declaration of an expression node
struct SASTExpression : public SASTNode {
	
	std::vector<SExpressionNode*> nodes;
	size_t destination_type = STypeRegistry::hashString("int");
	
	virtual SVMNode* compile() {
		
		SVMExpression* expression = new SVMExpression();
		expression->node_type = node_type;
		expression->destination_type = destination_type;
		expression->nodes = nodes;
		
		return expression;
		
	}
	
};

struct SASTDeclaration : public SASTNode {
	
	SToken type, identifier;

	virtual SVMNode* compile() {
		
		SVMDeclaration* declaration = new SVMDeclaration();
		declaration->node_type = node_type;
		declaration->type = STypeRegistry::instance()->hashString(type.string);
		declaration->identifier = identifier.string;
		
		return declaration;
		
	}
	
};

struct SASTAssignment : public SASTNode {
    
    SASTDeclaration* declaration;
    SASTExpression* expression;
    SToken identifier;
	
	virtual SVMNode* compile() {
	
		SVMAssignment* assignment = new SVMAssignment();
		assignment->node_type = node_type;
		assignment->declaration = (SVMDeclaration*)declaration->compile();
		assignment->expression = (SVMExpression*)expression->compile();
		
		return assignment;
		
	}
	
};

struct SASTFunctionCall : public SASTNode {
    
    SToken identifier;
    std::vector<SASTExpression*> expressions;
	
	virtual SVMNode* compile() {
		
		SVMFunctionCall* call = new SVMFunctionCall();
		call->node_type = node_type;
		
		// Compile all of th expressions
		for (int i = 0; i < expressions.size(); i++)
			call->expressions.push_back((SVMExpression*)expressions[i]->compile());
		
		call->identifier = identifier.string;
		
		return call;
		
	}
};

struct SBlock : public SASTNode {
    
    std::vector<SASTNode*> nodes;
    SASTNode* owner = nullptr;
    
    std::map<std::string, SVariable> variables;
	
	bool func_override = false;
	
	virtual SVMNode* compile() {
	
		SVMBlock* block = new SVMBlock();
		block->node_type = node_type;
		
		for (int i = 0; i < nodes.size(); i++)
			block->nodes.push_back(nodes[i]->compile());
		
		block->func_override = func_override;
		
		return block;
		
	}
	
};

struct SASTFunctionDefinition : public SASTNode {
    
    SToken identifier;
    SBlock* block;
    
    std::vector<SASTDeclaration*> args;
	
	virtual SVMNode* compile() {
	
		SVMFunctionDefinition* def = new SVMFunctionDefinition();
		def->node_type = node_type;
		
		for (int i = 0; i < args.size(); i++)
			def->args.push_back((SVMDeclaration*)args[i]->compile());
		
		def->block = (SVMBlock*)block->compile();
		def->identifier = identifier.string;
		
		return def;
	
	}
    
};

struct SASTIfStatement : public SASTNode {
	
	SBlock* block;
	SBlock *parent_block;
	
	SASTExpression* expression;
	SASTNode* else_node;
	
	SASTIfStatement* parent_if;
	
	virtual SVMNode* compile() {
		
		SVMIfStatement* if_statement = new SVMIfStatement();
		if_statement->node_type = node_type;
		if_statement->expression = (SVMExpression*)expression->compile();
		if_statement->block = (SVMBlock*)block->compile();
		if_statement->else_node = else_node->compile();
	
		return if_statement;
		
	}
	
};

struct SASTLoop : public SASTNode {
	
	SBlock* block;
	SBlock* parent_block;
	
	SASTExpression* expression;
	SASTLoop* parent_loop;
	
	SLoopType loop_type;
	
	virtual SVMNode* compile() {
	
		SVMLoop* loop = new SVMLoop();
		loop->node_type = node_type;
		loop->loop_type = loop_type;
		loop->block = (SVMBlock*)block->compile();
		loop->expression = (SVMExpression*)expression->compile();
		
		return loop;
	
	}
	
};

struct SASTLoopFor : public SASTLoop {
	
	SASTAssignment* initial_assign;
	SASTAssignment* increment;
	
	virtual SVMNode* compile() {
	
		SVMLoopFor* loop = new SVMLoopFor();
		loop->node_type = node_type;
		loop->loop_type = loop_type;
		loop->block = (SVMBlock*)block->compile();
		loop->expression = (SVMExpression*)expression->compile();
		
		loop->initial_assign = (SVMAssignment*)initial_assign->compile();
		loop->increment = (SVMAssignment*)increment->compile();
		
		return loop;
	
	}
	
};

#endif /* SASTNodes_h */
