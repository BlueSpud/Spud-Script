//
//  Slexer.cpp
//  SpudScript
//
//  Created by Logan Pazol on 12/28/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include "SLexer.hpp"

SLexerData SLexer::data;

SLexerData::SLexerData() {
	
	// Add in keywords
	keywords.push_back("if");
	keywords.push_back("return");
	keywords.push_back("else");
	keywords.push_back("func");
	keywords.push_back("while");
	keywords.push_back("for");
	
	// Add in operators, order does matter for some of these
	operators.push_back("==");
	operators.push_back("!=");
	operators.push_back("!");
	operators.push_back("=");
	
	operators.push_back("&&");
	operators.push_back("||");
	
	operators.push_back("+");
	operators.push_back("-");
	operators.push_back("*");
	operators.push_back("/");
	operators.push_back("%");
	
	operators.push_back("<=");
	operators.push_back(">=");
	operators.push_back("<");
	operators.push_back(">");

}

std::vector<SToken> SLexer::lexSource(std::string src) {
    
    std::vector<SToken> tokens;
    
    // Go char by char and read the string
    for (int i = 0; i < src.length(); i++) {
        
        std::string char_at = src.substr(i, 1);
        
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
        if (i + 1 < src.length() && !src.substr(i, 2).compare("//")) {
            
            // Run until we get a new line or we hit end of file
            while (i < src.length()) {
                
                char_at = src.substr(i, 1);
                
                if (!char_at.compare("\n"))
                    break;
                
                i++;
                
            }
            
        }
        
        // Check for operators
		SToken operator_token = getTokenFromArray(data.operators, i, src, STokenTypeOperator);
		if (operator_token.string.length()) {
			
			tokens.push_back(operator_token);
			i = i + (int)operator_token.string.length() - 1;
			continue;
			
		}
		
        // Check for types
        SToken type_token = getTokenFromArray(STypeRegistry::instance()->registered_types, i, src, STokenTypeType);
        if (type_token.string.length()) {
            
            tokens.push_back(type_token);
            i = i + (int)type_token.string.length() - 1;
            continue;
            
        }
        
        // Check for keywords
        SToken keyword_token = getTokenFromArray(data.keywords, i, src, STokenTypeKeyword);
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
			while (i < src.length()) {
				
				char_at = src.substr(i, 1);
				
				if (!char_at.compare(".")) {
					
					// If there was already a decimal, create another number
					// This should be an error when the tree is made
					if (found_decimal == true)
						throw std::runtime_error("Unexpected .");
					
					found_decimal = true;
					
					
				} else if (!(char_at.c_str()[0] >= '0' && char_at.c_str()[0] <= '9'))
					break;
				
				i++;
				
			}
			
			// Get the token for the number
			SToken token;
			token.type = STokenTypeNumber;
			token.string = src.substr(start, i - start);
			
			tokens.push_back(token);
			i--;
			continue;
			
		}
		
        // If all else failed, we can assume that this was an identifier
        if ((char_at.c_str()[0] >= 'a' && char_at.c_str()[0] <= 'z') || (char_at.c_str()[0] >= 'A' && char_at.c_str()[0] <= 'Z') || !char_at.compare("_")) {
			
            // Start parsing the identifier
            int start = i;
			
            while (i < src.length()) {
				
                char_at = src.substr(i, 1);
                
                // These are what is valid as the second char for a identifier, numbers are allowed
                if (!(char_at.c_str()[0] >= 'a' && char_at.c_str()[0] <= 'z') &&
					!(char_at.c_str()[0] >= 'A' && char_at.c_str()[0] <= 'Z') &&
					!(char_at.c_str()[0] >= '0' && char_at.c_str()[0] <= '9') &&
					char_at.c_str()[0] != '.' &&
					char_at.compare("_"))
                    break;
             
                i++;
                
            }
            
            // Get the token for the identifier
            SToken token;
            token.type = STokenTypeIdentifier;
            token.string = src.substr(start, i - start);
            
            tokens.push_back(token);
            i--;
            continue;

        }
		
		// Check for strings
		if (!char_at.compare("\"")) {
			
			// A string was opened, start filling it
			i++;
			
			SToken token;
			token.type = STokenTypeString;
			char_at = src.substr(i, 1);
			
			while (char_at.compare("\"")) {
				
				token.string = token.string + char_at;
				i++;
				
				if (i < src.length())
					char_at = src.substr(i, 1);
				else throw std::runtime_error("\" expected");
				
			}

			tokens.push_back(token);
			continue;
		
		}
        
    }
    
    return tokens;
    
}

SToken SLexer::getTokenFromArray(const std::vector<std::string> array, int position, const std::string& src, STokenType type) {
    
    // See if this is the start of a keyword
    for (int k = 0; k < array.size(); k++) {
        
        // Check if there is enough room for the keyword
        if (src.length() - position >= array[k].length()) {
            
            if (!src.substr(position, array[k].length()).compare(array[k])) {
                
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
