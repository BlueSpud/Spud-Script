//
//  Slexer.hpp
//  SpudScript
//
//  Created by Logan Pazol on 12/28/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef Slexer_hpp
#define Slexer_hpp

#include <iostream>
#include <string>
#include <vector>

#include "STypes.hpp"

#ifdef GCC
	#pragma GCC visibility push(hidden)
#endif

enum STokenType {
	
    STokenTypeKeyword,
    STokenTypeType,
    STokenTypeIdentifier,
    STokenTypeNumber,
    STokenTypeOperator,
    STokenTypeOpenParen,
    STokenTypeCloseParen,
    STokenTypeOpenBrack,
    STokenTypeCloseBrack,
    STokenTypeSemi,
	STokenTypeString
    
};

struct SToken {
    
    STokenType type;
    std::string string;
    
    std::string getString() const;
    
};

struct SLexerData {
	
	SLexerData();
	
	std::vector<std::string> keywords;
	std::vector<std::string >operators;
	
};

class SLexer {
    
    public:
	
        static std::vector<SToken> lexSource(std::string src);
    
    private:
    
        static SToken getTokenFromArray(const std::vector<std::string> array, int position, const std::string& src, STokenType type);
		static SLexerData data;
	
};

#ifdef GCC
	#pragma GCC visibility pop
#endif

#endif /* Slexer_hpp */
