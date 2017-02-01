//
//  main.cpp
//  SpudScript
//
//  Created by Logan Pazol on 12/28/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <SScript.hpp>

SScript script;

// Declare a few functions
void greeting() { std::cout << "Hello Spud Script!\n"; }
void prints(const char* string) { std::cout << string << std::endl; }

int cppFunction(int a, int b) {

	a = a * 2 * b;
	
	return a;
	
}

// Expose the functions to script, parameters are script, function, return type, arguments (seperated by spaces)
EXPOSE_FUNC_NO_ARGS(script, greeting, void)
EXPOSE_FUNC(script, cppFunction, int, int int)
EXPOSE_FUNC(script, prints, void, string)

// A simple C++ class
class cppClass {
	
	public:
	
		int var = 24;
	
};

// Expose cppClass and its member var as an int
EXPOSE_SCRIPT_TYPE(cppClass);
EXPOSE_SCRIPT_MEMBER(cppClass, var, int)

int main(int argc, const char * argv[]) {
	
	// Read the code from a file
	std::ifstream in_stream = std::ifstream("./sample.ss");
	std::string code, line;
	
	while (std::getline(in_stream, line, '\n'))
		code = code + line + "\n";
	
	// Load the string to a runable script
	script.loadScript(code);

	// Create a variable in C++ and expose it to script
	cppClass cppObject;
	cppObject.var = 100;
	script.exposeVariable<cppClass>(&cppObject, "cppObject");
	
	// Execute the script
	script.execute();
	
	// Get the variable a from script and print its value
	int* a = script.getScriptValue<int>("a");
	std::cout << *a << std::endl;
	
}
