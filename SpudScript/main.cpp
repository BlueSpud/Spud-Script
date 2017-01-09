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

class Other {

    public:

		int a = 231;

};

class Test {

    public:

        int a, b = 24, c = 12, d;
    
        Other o;

};

void cppFunction(float a, float b, float c) {
	
	std::cout << "a " << a << std::endl;
	std::cout << "b " << b << std::endl;
	std::cout << "c " << c << std::endl;
    
}

EXPOSE_FUNC(vm, cppFunction)

int main(int argc, const char * argv[]) {
    
    // Create a lexer
    SLexer lexer;
    SAST parser;
	
	EXPOSE_SCRIPT_TYPE(Other);
	EXPOSE_SCRIPT_VARIABLE(Other, a, int);
	
    EXPOSE_SCRIPT_TYPE(Test);
    EXPOSE_SCRIPT_VARIABLE(Test, a, float);
    EXPOSE_SCRIPT_VARIABLE(Test, b, float);
    EXPOSE_SCRIPT_VARIABLE(Test, c, float);
    EXPOSE_SCRIPT_VARIABLE(Test, d, float);
    EXPOSE_SCRIPT_VARIABLE(Test, o, Other);
    
    // Tokens for lexer
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
	//vm.functions["cppFunction"] = &cppFunction;
    
    // Read the code
    std::ifstream in_stream = std::ifstream("./sample.ss");
    std::string code, line;
    
    while (std::getline(in_stream, line, '\n'))
        code = code + line + "\n";
        
    // Go!
    std::vector<SToken> tokens = lexer.lexSource(code);
    std::vector<SASTNode*> nodes = parser.parseTokens(tokens);

    vm.executeCode(nodes);
	
	std::cout << *(int*)vm.resolveVarible("a")->value << std::endl;
	std::cout << *(float*)vm.resolveVarible("b")->value << std::endl;
	std::cout << *(float*)vm.resolveVarible("c")->value << std::endl;
	std::cout << *(int*)vm.resolveVarible("d")->value << std::endl;
	//std::cout << *(float*)vm.resolveVarible("t.a")->value << std::endl;
//    std::cout << *(float*)vm.resolveVarible("t.c")->value << std::endl;
//    std::cout << *(float*)vm.resolveVarible("t.d")->value << std::endl;
//    std::cout << *(float*)vm.resolveVarible("t.o.a")->value << std::endl;
//    std::cout << *(float*)vm.resolveVarible("b.a")->value << std::endl;
	
}
