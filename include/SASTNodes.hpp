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

#include "SVMNodes.hpp"

#ifdef GCC
	#pragma GCC visibility push(hidden)
#endif

struct SASTNode {
	
	SNodeType node_type;

	virtual SVMNode* compile() { return nullptr; };
	virtual ~SASTNode() {}
	
};

struct SASTExpression : public SASTNode {
	
	std::vector<SExpressionNode*> nodes;
	size_t destination_type = STypeRegistry::hashString("int");
	
	bool compiled = false;
	virtual SVMNode* compile() {
		
		SVMExpression* expression = new SVMExpression();
		expression->node_type = node_type;
		expression->destination_type = destination_type;
		expression->nodes = nodes;
		
		compiled = true;
		return expression;
		
	}
	
	virtual ~SASTExpression() {
		
		// If the script was never compiled for some reason, maybe parsing failed, we need to delete these
		if (!compiled)
			for (int i = 0; i < nodes.size(); i++)
				delete nodes[i];
		
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
		assignment->identifier = identifier.string;
		
		if (declaration)
			assignment->declaration = (SVMDeclaration*)declaration->compile();
		
		assignment->expression = (SVMExpression*)expression->compile();
		
		return assignment;
		
	}
	
	virtual ~SASTAssignment() {
	
		// Declaration is optional, so we need to make sure we have one to delete it
		if (declaration)
			delete declaration;
		
		delete expression;
	
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
	
	virtual ~SASTFunctionCall() {
	
		for (int i = 0; i < expressions.size(); i++)
			delete expressions[i];
	
	}
	
};

struct SBlock : public SASTNode {
    
    std::vector<SASTNode*> nodes;
	SASTNode* owner = nullptr;enum SExpressionNodeType {
		
		SExpressionNodeTypeLiteral,
		SExpressionNodeTypeExpression,
		SExpressionNodeTypeVariable,
		SExpressionNodeTypeOperator
		
	};
	
	class SExpressionNode {
		
	public:
		
		SExpressionNodeType type;
		
	};
	
	virtual SVMNode* compile() {
	
		SVMBlock* block = new SVMBlock();
		block->node_type = node_type;
		
		for (int i = 0; i < nodes.size(); i++)
			block->nodes.push_back(nodes[i]->compile());
		
		return block;
		
	}
	
	virtual ~SBlock() {
	
		for (int i = 0; i < nodes.size(); i++)
			delete nodes[i];
	
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
	
	virtual ~SASTFunctionDefinition() {
		
		delete block;
		for (int i = 0; i < args.size(); i++)
			delete args[i];
		
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
		
		// Else node is optional, it doesnt have to be compiled unless its there
		if (else_node)
			if_statement->else_node = else_node->compile();
	
		return if_statement;
		
	}
	
	virtual ~SASTIfStatement() {
		
		delete block;
		delete expression;
		
		// Else node is optional, so make sure there is one before deleting
		if (else_node)
			delete else_node;
		
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
	
	virtual ~SASTLoop() {
	
		delete block;
		delete expression;
		
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
	
	virtual ~SASTLoopFor() {
		
		delete initial_assign;
		delete increment;
		
	}
	
};

struct SASTReturn : public SASTNode {
	
	SASTExpression* expression;
	
	virtual SVMNode* compile() {
		
		SVMReturn* node = new SVMReturn();
		node->node_type = STypeReturn;
		node->expression = (SVMExpression*)expression->compile();
		
		return node;
		
	}
	
	virtual ~SASTReturn() { delete expression; }

};

#ifdef GCC
	#pragma GCC visibility pop
#endif

#endif /* SASTNodes_h */
