//
//  SVMNodes.h
//  SpudScript
//
//  Created by Logan Pazol on 1/20/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SVMNodes_h
#define SVMNodes_h

enum SNodeType {
	
	STypeExpression,
	STypeDeclaration,
	STypeAssignment,
	STypeFunctionCall,
	STypeBlock,
	STypeFunctionDef,
	STypeIfExpression,
	STypeLoop
	
};

struct SVMNode { SNodeType node_type; };

// Forward declaration of expression node
struct SExpressionNode;

struct SVMExpression : public SVMNode {
	
	std::vector<SExpressionNode*> nodes;
	size_t destination_type;
	
};


struct SVMDeclaration : public SVMNode {
	
	size_t type;
	std::string identifier;
	
};

struct SVMAssignment : public SVMNode {
	
	SVMDeclaration* declaration;
	SVMExpression* expression;
	std::string identifier;
	
};

struct SVMFunctionCall : public SVMNode {
	
	std::string identifier;
	std::vector<SVMExpression*> expressions;
	
};

struct SVMBlock : public SVMNode {
	
	std::vector<SVMNode*> nodes;
	SVMNode* owner = nullptr;
	
	std::map<std::string, SVariable> variables;
	
	bool func_override = false;
	
};

struct SVMFunctionDefinition : public SVMNode {
	
	std::string identifier;
	SVMBlock* block;
	
	std::vector<SVMDeclaration*> args;
	
};

struct SVMIfStatement : public SVMNode {
	
	SVMBlock* block;
	SVMExpression* expression;
	SVMNode* else_node;
	
};

enum SLoopType {
	
	SLoopTypeWhile,
	SLoopTypeFor
	
};

struct SVMLoop : public SVMNode {
	
	SVMBlock* block;
	SVMExpression* expression;
	SLoopType loop_type;
	
};

struct SVMLoopFor : public SVMLoop {
	
	SVMAssignment* initial_assign;
	SVMAssignment* increment;
	
};

#endif /* SVMNodes_h */
