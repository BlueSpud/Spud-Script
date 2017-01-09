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
    STokenTypeSemi
    
};

static std::string STokenNames[] = {
    
    "keyword",
    "type",
    "identifier",
    "number",
    "operator",
    "open paren",
    "close paren",
    "open brack",
    "close brack",
    "semi"
    
};

struct SToken {
    
    STokenType type;
    std::string string;
    
    std::string getString() const;
    
};

class SLexer {
    
    public:
    
        std::vector<std::string> keywords;
        std::vector<std::string >operators;
        std::vector<SToken> lexSource(std::string source);
    
    private:
    
        SToken getTokenFromArray(const std::vector<std::string> array, int position, const std::string& source, STokenType type);
    
};


#endif /* Slexer_hpp */
