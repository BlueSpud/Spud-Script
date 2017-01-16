//
//  SExpression.hpp
//  SpudScript
//
//  Created by Logan Pazol on 1/15/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SExpression_hpp
#define SExpression_hpp

#include "SASTNodes.h"

enum SExpressionNodeType {
	
	SExpressionNodeTypeLiteral,
	SExpressionNodeTypeExpression,
	SExpressionNodeTypeVariable,
	SExpressionNodeTypeOperator
	
};

class SExpressionNode {
	
	public:
	
		SExpressionNodeType type;
	
};

class SExpressionNodeLiteral : public SExpressionNode {
	
	public:
	
		template <class T>
		SExpressionNodeLiteral(T _value) {
		
			type = SExpressionNodeTypeLiteral;
			size = sizeof(T);
			
			// Get the script type based on the C++ class
			literal_type = STypeRegistry::instance()->cpp_class_names[typeid(T).name()];
			
			// Keep the variable
			value = malloc(size);
			STypeRegistry::instance()->performCopy(value, &_value, literal_type);
		
		}
	
		template <class T>
		SExpressionNodeLiteral(T _value, const std::string& _literal_type) {
		
			type = SExpressionNodeTypeLiteral;
			size = sizeof(T);
		
			// Get the script type based on the C++ class
			literal_type = _literal_type;
		
			// Keep the variable
			value = malloc(size);
			STypeRegistry::instance()->performCopy(value, &_value, literal_type);
		
		}
	
		size_t size;
	
		std::string literal_type;
		void* value;
};

class SExpressionNodeExpression : public SExpressionNode {
	
	public:
	
		SExpressionNodeExpression(SASTExpression* _expression) {
	
			type = SExpressionNodeTypeExpression;
			expression = _expression;
			
		}
	
		SASTExpression* expression;
	
};

class SExpressionNodeVariable : public SExpressionNode {
	
	public:
	
		SExpressionNodeVariable(std::string _var_name) {
		
			type = SExpressionNodeTypeVariable;
			var_name = _var_name;
		
		}
	
		std::string var_name;
	
};

class SExpressionNodeOperator : public SExpressionNode {
	
	public:
	
		SExpressionNodeOperator(std::string _op) {
		
			type = SExpressionNodeTypeOperator;
			op = _op;
		
		}
	
		std::string op;
	
};

// Actual definition for expression
struct SASTExpression : public SASTNode {
	
	std::vector<SExpressionNode*> nodes;
	std::string destination_type = "int";
	
};

#endif /* SExpression_hpp */