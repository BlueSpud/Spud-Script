//
//  main.cpp
//  SpudScript
//
//  Created by Logan Pazol on 12/28/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "SVM.hpp"

SVM vm;

void cppFunction() {
    
    std::cout << "INCREMENTING A\n";
    vm.resolveVarible("a")->value++;
    
}

int main(int argc, const char * argv[]) {
    
    // Create a lexer
    SLexer lexer;
    SAST parser;
    
    // Tokens for lexer
    lexer.types.push_back("int");
    lexer.types.push_back("float");
    
    lexer.keywords.push_back("if");
    lexer.keywords.push_back("return");
    lexer.keywords.push_back("else");
    lexer.keywords.push_back("func");
    
    lexer.operators.push_back("=");
    lexer.operators.push_back("+");
    lexer.operators.push_back("-");
    lexer.operators.push_back("*");
    lexer.operators.push_back("/");
    lexer.operators.push_back("%");
    
    // Add in a test function to the VM
    vm.functions["cppFunction"] = &cppFunction;
    
    // Read the code
    std::ifstream in_stream = std::ifstream("./script.ss");
    std::string code, line;
    
    while (std::getline(in_stream, line, '\n'))
        code = code + line + "\n";
        
    // Go!
    std::vector<SToken> tokens = lexer.lexSource(code);
    std::vector<SASTNode*> nodes = parser.parseTokens(tokens);
    
//    for (int i = 0; i < nodes.size(); i++)
//        std::cout << SASTNodeTypeNames[nodes[i]->node_type] << std::endl;
    
    vm.executeCode(nodes);
    
    std::cout << vm.resolveVarible("a")->value << std::endl;
    //std::cout << *vm.resolveVarible<float>("b") << std::endl;
    //std::cout << *vm.resolveVarible<float>("c") << std::endl;
    
}
