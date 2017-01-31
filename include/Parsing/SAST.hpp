//
//  SAST.hpp
//  SpudScript
//
//  Created by Logan Pazol on 12/28/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SAST_hpp
#define SAST_hpp

#include "SLexer.hpp"
#include "SExpression.hpp"

#ifdef GCC
	#pragma GCC visibility push(hidden)
#endif

class SAST {
    
    public:
    
        static std::vector<SASTNode*> parseTokens(std::vector<SToken>& tokens);
    
    private:
	
		#define PARSE_ARGS std::vector<SToken>& tokens, int& i
	
        static bool parseSemi(PARSE_ARGS);
		static bool parseStartBlock(PARSE_ARGS, SBlock*& current_block);
        static bool parseEndBlock(PARSE_ARGS, std::vector<SASTNode*>& nodes, SBlock*& current_block, SASTIfStatement*& current_if, SASTLoop*& current_loop);
		static bool endIf(PARSE_ARGS, SBlock*& current_block, SASTIfStatement*& current_if);
		static bool endLoop(SBlock*& current_block, SASTLoop*& current_loop);
		static bool parseAssignment(PARSE_ARGS, std::vector<SASTNode*>* node_place);
	
		static SASTExpression* parseExpression(PARSE_ARGS);
		static SExpressionNode* parseExpressionNode(PARSE_ARGS);
	
		static SASTDeclaration* parseDeclaration(PARSE_ARGS);
		static SASTFunctionCall* parseFunctionCall(PARSE_ARGS);
		static SASTFunctionDefinition* parseFunctionDef(PARSE_ARGS, SBlock*& current_block);
		static SASTIfStatement* parseIfStatement(PARSE_ARGS, SBlock*& current_block, SASTIfStatement*& current_if);
		static SASTLoop* parseWhileLoop(PARSE_ARGS, SBlock*& current_block, SASTLoop*& current_loop);
		static SASTLoop* parseForLoop(PARSE_ARGS, SBlock*& current_block, SASTLoop*& current_loop);
		static SASTReturn* parseReturn(PARSE_ARGS);
	
};

#ifdef GCC
	#pragma GCC visibility pop
#endif

#endif /* SAST_hpp */
