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

        int a = 0, b = 24, c = 12, d = 0;
    
        Other o;
	
		static int test;

};

int Test::test;

void prints(char* a) {
	
	std::cout << a << std::endl;

}

void cppFunction(float a, float b, float c) {
	
	std::cout << "a " << a << std::endl;
	std::cout << "b " << b << std::endl;
	std::cout << "c " << c << std::endl;
    
}

int cppReturn() { return 25; }

EXPOSE_FUNC(vm, cppFunction, void, float float float)
EXPOSE_FUNC(vm, prints, void, string)
EXPOSE_FUNC_NO_ARGS(vm, cppReturn, int)

EXPOSE_SCRIPT_TYPE(Other)

EXPOSE_SCRIPT_TYPE(Test)

int main(int argc, const char * argv[]) {
    
    // Create a lexer
    SLexer lexer;
    SAST parser;
	
	// TODO, fix this!
	EXPOSE_SCRIPT_MEMBER(Other, a, int);
	
    EXPOSE_SCRIPT_MEMBER(Test, a, int);
    EXPOSE_SCRIPT_MEMBER(Test, b, int);
    EXPOSE_SCRIPT_MEMBER(Test, c, int);
    EXPOSE_SCRIPT_MEMBER(Test, d, int);
    EXPOSE_SCRIPT_MEMBER(Test, o, Other);
    
    // Tokens for lexer
    lexer.keywords.push_back("if");
    lexer.keywords.push_back("return");
    lexer.keywords.push_back("else");
    lexer.keywords.push_back("func");
	lexer.keywords.push_back("while");
	lexer.keywords.push_back("for");
	
	// Order does matter for some of these
	lexer.operators.push_back("==");
	lexer.operators.push_back("!=");
	lexer.operators.push_back("!");
    lexer.operators.push_back("=");
	
	lexer.operators.push_back("&&");
	lexer.operators.push_back("||");
	
    lexer.operators.push_back("+");
    lexer.operators.push_back("-");
    lexer.operators.push_back("*");
    lexer.operators.push_back("/");
	lexer.operators.push_back("%");
	
	lexer.operators.push_back("<=");
	lexer.operators.push_back(">=");
	lexer.operators.push_back("<");
	lexer.operators.push_back(">");
    
    // Read the code
    std::ifstream in_stream = std::ifstream("./sample.ss");
    std::string code, line;
    
    while (std::getline(in_stream, line, '\n'))
        code = code + line + "\n";
        
    // Go!
    std::vector<SToken> tokens = lexer.lexSource(code);
    std::vector<SASTNode*> nodes = parser.parseTokens(tokens);
	std::vector<SVMNode*> compiled;
	
	for (int i = 0; i < nodes.size(); i++)
		compiled.push_back(nodes[i]->compile());
	
	for (int i = 0; i < nodes.size(); i++)
		delete nodes[i];
	
	Test* some = new Test;
	vm.exposeVariable<Test>(some, "some");

	try { vm.executeCode(compiled); } catch (std::runtime_error e) {
		
		std::cout << "[Spud Script] There was a runtime error executing the script: " << e.what() << std::endl;
		
	}
	
	for (int i = 0; i < compiled.size(); i++)
		delete compiled[i];

}


