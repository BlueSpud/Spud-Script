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

#include <stdarg.h>

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

void prints(int a) { std::cout << a << std::endl; }

void cppFunction(float a, float b, float c) {
	
	std::cout << "a " << a << std::endl;
	std::cout << "b " << b << std::endl;
	std::cout << "c " << c << std::endl;
    
}

EXPOSE_FUNC(vm, cppFunction, void, float float float)
EXPOSE_FUNC(vm, prints, void, int)

EXPOSE_SCRIPT_TYPE(Other)

EXPOSE_SCRIPT_TYPE(Test)

int main(int argc, const char * argv[]) {
    
    // Create a lexer
    SLexer lexer;
    SAST parser;
	
	EXPOSE_SCRIPT_VARIABLE(Other, a, int);
	
    EXPOSE_SCRIPT_VARIABLE(Test, a, int);
    EXPOSE_SCRIPT_VARIABLE(Test, b, int);
    EXPOSE_SCRIPT_VARIABLE(Test, c, int);
    EXPOSE_SCRIPT_VARIABLE(Test, d, int);
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
    
    // Read the code
    std::ifstream in_stream = std::ifstream("./sample.ss");
    std::string code, line;
    
    while (std::getline(in_stream, line, '\n'))
        code = code + line + "\n";
        
    // Go!
    std::vector<SToken> tokens = lexer.lexSource(code);
    std::vector<SASTNode*> nodes = parser.parseTokens(tokens);

    vm.executeCode(nodes);
	
	vm.callFunction("test", 28, 12);
	
//	std::cout << "ga: " <<  *vm.getScriptValue<int>("ga") << std::endl;
//	std::cout << "gb: " <<  *vm.getScriptValue<int>("gb") << std::endl;
//	std::cout << "c: " <<  *vm.getScriptValue<float>("c") << std::endl;
//	std::cout << "d: " <<  *vm.getScriptValue<int>("d") << std::endl;
//	std::cout << "t.a: " << vm.getScriptValue<Test>("t")->a << std::endl;
//	std::cout << "f: " <<  *vm.getScriptValue<double>("f") << std::endl;
	
}
