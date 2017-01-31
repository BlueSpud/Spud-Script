//
//  SExpression.hpp
//  SpudScript
//
//  Created by Logan Pazol on 1/15/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef SExpression_hpp
#define SExpression_hpp

#include "SASTNodes.hpp"

class SExpressionNodeLiteral : public SExpressionNode {
	
	public:
	
		template <class T>
		SExpressionNodeLiteral(T _value) {
		
			type = SExpressionNodeTypeLiteral;
			size = sizeof(T);
			
			// Get the script type based on the C++ class
			literal_type = STypeRegistry::instance()->cpp_class_names[STypeRegistry::hashString(typeid(T).name())];
			
			// Keep the variable
			value = calloc(1,size);
			STypeRegistry::instance()->performCopy(value, &_value, literal_type);
		
		}
	
		template <class T>
		SExpressionNodeLiteral(T _value, size_t _literal_type) {
		
			type = SExpressionNodeTypeLiteral;
			size = sizeof(T);
		
			// Get the script type based on the C++ class
			literal_type = _literal_type;
		
			// Keep the variable
			value = calloc(1,size);
			STypeRegistry::instance()->performCopy(value, &_value, literal_type);
		
		}
	
		virtual ~SExpressionNodeLiteral() { free(value); }
	
		size_t size;
	
		size_t literal_type;
		void* value;
};

class SExpressionNodeExpression : public SExpressionNode {
	
	public:
	
		SExpressionNodeExpression(SVMExpression* _expression) {
	
			type = SExpressionNodeTypeExpression;
			expression = _expression;
			
		}
	
		virtual ~SExpressionNodeExpression() { delete expression; }
	
		SVMExpression* expression;
	
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

class SExpressionNodeFunction : public SExpressionNode {
	
	public:
	
		SExpressionNodeFunction(SVMFunctionCall* _call) {
		
			type = SExpressionNodeTypeFunction;
			call = _call;
		
		}
	
	SVMFunctionCall* call;
	
};

#endif /* SExpression_hpp */
