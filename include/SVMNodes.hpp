//
//  SVMNodes.h
//  SpudScript
//
//  Created by Logan Pazol on 1/20/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SVMNodes_h
#define SVMNodes_h

#ifdef GCC
	#pragma GCC visibility push(hidden)
#endif

enum SNodeType {
	
	STypeExpression,
	STypeDeclaration,
	STypeAssignment,
	STypeFunctionCall,
	STypeBlock,
	STypeFunctionDef,
	STypeIfExpression,
	STypeLoop,
	STypeReturn
	
};

struct SVMNode {
	
	SNodeType node_type;
	virtual ~SVMNode() {}
	
};

// Expression node declaration
enum SExpressionNodeType {
	
	SExpressionNodeTypeLiteral,
	SExpressionNodeTypeExpression,
	SExpressionNodeTypeVariable,
	SExpressionNodeTypeOperator,
	SExpressionNodeTypeFunction
	
};

class SExpressionNode {
	
	public:
	
		SExpressionNodeType type;
		virtual ~SExpressionNode() {}
	
};

struct SVMExpression : public SVMNode {
	
	std::vector<SExpressionNode*> nodes;
	size_t destination_type;
	
	virtual ~SVMExpression() {
	
		for (int i = 0; i < nodes.size(); i++)
			delete nodes[i];
	
	}
	
};


struct SVMDeclaration : public SVMNode {
	
	size_t type;
	std::string identifier;
	
};

struct SVMAssignment : public SVMNode {
	
	SVMDeclaration* declaration;
	SVMExpression* expression;
	std::string identifier;
	
	virtual ~SVMAssignment() {
	
		// Declaration is optional, make sure we have it before we delete it
		if (declaration)
			delete declaration;
		
		delete expression;
	
	}
	
};

struct SVMFunctionCall : public SVMNode {
	
	std::string identifier;
	std::vector<SVMExpression*> expressions;
	
	virtual ~SVMFunctionCall() {
		
		for (int i = 0; i < expressions.size(); i++)
			delete expressions[i];
		
	}
	
};

struct SVMBlock : public SVMNode {
	
	std::vector<SVMNode*> nodes;
	SVMNode* owner = nullptr;
	
	std::map<std::string, SVariable> variables;
	bool func_override = false;

	void unload() {
		
		// We have an unload function instead of a delete, because blocks are duplicated at runtime, so the nodes would be deleted
		for (int i = 0; i < nodes.size(); i++)
			delete nodes[i];
		
	}
	
};

struct SVMFunctionDefinition : public SVMNode {
	
	std::string identifier;
	SVMBlock* block;
	
	std::vector<SVMDeclaration*> args;
	
	virtual ~SVMFunctionDefinition() {
		
		block->unload();
		delete block;
		
		for (int i = 0; i < args.size(); i++)
			delete args[i];
		
	}
	
};

struct SVMIfStatement : public SVMNode {
	
	SVMBlock* block;
	SVMExpression* expression;
	SVMNode* else_node;
	
	virtual ~SVMIfStatement() {
		
		block->unload();
		delete block;
		
		delete expression;
		
		// Make sure we have an else node before we delete it, it is optional
		if (else_node)
			delete else_node;
		
	}
	
};

enum SLoopType {
	
	SLoopTypeWhile,
	SLoopTypeFor
	
};

struct SVMLoop : public SVMNode {
	
	SVMBlock* block;
	SVMExpression* expression;
	SLoopType loop_type;
	
	virtual ~SVMLoop() {
		
		block->unload();
		delete block;
		
		delete expression;
		
	}
	
};

struct SVMLoopFor : public SVMLoop {
	
	SVMAssignment* initial_assign;
	SVMAssignment* increment;
	
	virtual ~SVMLoopFor() {
		
		delete initial_assign;
		delete increment;
		
	}
	
};

struct SVMReturn : public SVMNode {
	
	SVMExpression* expression;
	
	virtual ~SVMReturn() { delete expression; }
	
};

#ifdef GCC
	#pragma GCC visibility pop
#endif

#endif /* SVMNodes_h */
