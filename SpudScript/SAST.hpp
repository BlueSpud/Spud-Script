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
#include "SExpression.h"

class SAST {
    
    public:
    
        std::vector<SASTNode*> parseTokens(std::vector<SToken>& tokens);
    
    private:
	
		#define PARSE_ARGS std::vector<SToken>& tokens, int& i
	
        bool parseSemi(PARSE_ARGS);
		bool parseStartBlock(PARSE_ARGS, SBlock*& current_block);
        bool parseEndBlock(PARSE_ARGS, std::vector<SASTNode*>& nodes, SBlock*& current_block, SASTIfStatement*& current_if, SASTLoop*& current_loop);
		bool endIf(PARSE_ARGS, SBlock*& current_block, SASTIfStatement*& current_if);
		bool endLoop(SBlock*& current_block, SASTLoop*& current_loop);
		bool parseAssignment(PARSE_ARGS, std::vector<SASTNode*>* node_place);
	
		SASTExpression* parseExpression(PARSE_ARGS);
		SExpressionNode* parseExpressionNode(PARSE_ARGS);
	
		SASTDeclaration* parseDeclaration(PARSE_ARGS);
		SASTFunctionCall* parseFunctionCall(PARSE_ARGS);
		SASTFunctionDefinition* parseFunctionDef(PARSE_ARGS, SBlock*& current_block);
		SASTIfStatement* parseIfStatement(PARSE_ARGS, SBlock*& current_block, SASTIfStatement*& current_if);
		SASTLoop* parseWhileLoop(PARSE_ARGS, SBlock*& current_block, SASTLoop*& current_loop);
	
};


#endif /* SAST_hpp */
