//
//  Slexer.cpp
//  SpudScript
//
//  Created by Logan Pazol on 12/28/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SLexer.hpp"

std::string SToken::getString() const { return "SToken: " + STokenNames[type] + " " + string; }

std::vector<SToken> SLexer::lexSource(std::string source) {
    
    std::vector<SToken> tokens;
    
    // Go char by char and read the string
    for (int i = 0; i < source.length(); i++) {
        
        std::string char_at = source.substr(i, 1);
        
        // Check for brackets
        if (!char_at.compare("(")) {
            
            SToken token;
            token.type = STokenTypeOpenParen;
            token.string = "(";
            
            tokens.push_back(token);
            continue;
            
        }
        
        if (!char_at.compare(")")) {
            
            SToken token;
            token.type = STokenTypeCloseParen;
            token.string = ")";
            
            tokens.push_back(token);
            continue;
            
        }
        
        if (!char_at.compare("{")) {
            
            SToken token;
            token.type = STokenTypeOpenBrack;
            token.string = "{";
            
            tokens.push_back(token);
            continue;
            
        }
        
        if (!char_at.compare("}")) {
            
            SToken token;
            token.type = STokenTypeCloseBrack;
            token.string = "}";
            
            tokens.push_back(token);
            continue;
            
        }
        
        if (!char_at.compare(";")) {
            
            SToken token;
            token.type = STokenTypeSemi;
            token.string = ";";
            
            tokens.push_back(token);
            continue;
            
        }
        
        // Check for comments
        if (i + 1 < source.length() && !source.substr(i, 2).compare("//")) {
            
            // Run until we get a new line or we hit end of file
            while (i < source.length()) {
                
                char_at = source.substr(i, 1);
                
                if (!char_at.compare("\n"))
                    break;
                
                i++;
                
            }
            
        }
        
        // Check for operators
        for (int o = 0; o < operators.size(); o++) {
            
            if (!char_at.compare(operators[o])) {
                
                SToken token;
                token.type = STokenTypeOperator;
                token.string = char_at;
                
                tokens.push_back(token);
                continue;
                
            }
            
        }
        
        // Check for types
        SToken type_token = getTokenFromArray(types, i, source, STokenTypeType);
        if (type_token.string.length()) {
            
            tokens.push_back(type_token);
            i = i + (int)type_token.string.length() - 1;
            continue;
            
        }
        
        // Check for keywords
        SToken keyword_token = getTokenFromArray(keywords, i, source, STokenTypeKeyword);
        if (keyword_token.string.length()) {
            
            tokens.push_back(keyword_token);
            i = i + (int)keyword_token.string.length() - 1;
            continue;
            
        }
        
        // Check for numbers
        bool found_decimal = false;
        if (!char_at.compare(".") || (char_at.c_str()[0] >= '0' && char_at.c_str()[0] <= '9')) {
            
            int start = i;
            
            // Start parsing the number
            while (i < source.length()) {
                
                char_at = source.substr(i, 1);
                
                if (!char_at.compare(".")) {
                    
                    // If there was already a decimal, create another number
                    // This should be an error when the tree is made
                    if (found_decimal == true)
                        break;
                    
                    found_decimal = true;
                    
                    
                } else if (!(char_at.c_str()[0] >= '0' && char_at.c_str()[0] <= '9'))
                    break;
                
                i++;
                
            }
            
            // Get the token for the number
            SToken token;
            token.type = STokenTypeNumber;
            token.string = source.substr(start, i - start);
            
            tokens.push_back(token);
            i--;
            continue;
            
        }
        
        // If all else failed, we can assume that this was an identifier
        if ((char_at.c_str()[0] >= 'a' && char_at.c_str()[0] <= 'z') || (char_at.c_str()[0] >= 'A' && char_at.c_str()[0] <= 'Z')) {
            
            // Start parsing the identifier
            int start = i;
            
            while (i < source.length()) {
                
                char_at = source.substr(i, 1);
                
                // If it not a letter, we're done
                if (!(char_at.c_str()[0] >= 'a' && char_at.c_str()[0] <= 'z') && !(char_at.c_str()[0] >= 'A' && char_at.c_str()[0] <= 'Z') && char_at.c_str()[0] != '.')
                    break;
             
                i++;
                
            }
            
            // Get the token for the identifier
            SToken token;
            token.type = STokenTypeIdentifier;
            token.string = source.substr(start, i - start);
            
            tokens.push_back(token);
            i--;
            continue;

        }
        
    }
    
    return tokens;
    
}

SToken SLexer::getTokenFromArray(const std::vector<std::string> array, int position, const std::string& source, STokenType type) {
    
    // See if this is the start of a keyword
    for (int k = 0; k < array.size(); k++) {
        
        // Check if there is enough room for the keyword
        if (source.length() - position >= array[k].length()) {
            
            if (!source.substr(position, array[k].length()).compare(array[k])) {
                
                // Keyword matched
                SToken token;
                token.type = type;
                token.string = array[k];
                
                return token;
                
            }
            
        }
        
    }
    
    return SToken();
    
}
