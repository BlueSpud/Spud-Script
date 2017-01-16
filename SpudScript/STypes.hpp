//
//  STypes.hpp
//  SpudScript
//
//  Created by Logan Pazol on 1/5/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#ifndef STypes_hpp
#define STypes_hpp

#include <iostream>
#include <vector>
#include <map>

class STypeRegistry;

class SClassFactoryBase {

	friend class STypeRegistry;
	
	public:
	
		virtual void* createObject() = 0;
	
	protected:
	
		size_t size;
	
};

template <class T>
class SClassFactory : public SClassFactoryBase {
	
    public:
    
		SClassFactory(size_t _size) { size = _size; }
        ~SClassFactory();
    
		virtual void* createObject() { return malloc(size); }

};

class SStringFactory : public SClassFactoryBase {

	virtual void* createObject() {
		
		// First we allocate space for a string and a pointer
		char* buffer = (char*)malloc(sizeof(char) * 2);
		sprintf(buffer, "");
		
		char** ptr = (char**)malloc(sizeof(char**));
		ptr[0] = buffer;
		
		return ptr;
		
	}
	
};

struct SVariableLocation {

    size_t byte_offset;
    std::string type;

};

struct SVariable {
    
    void* value;
    std::string type;
    
};

// Function to cast a type to c++
template <class T>
T castCPP(void* value) { return (*(T*)value); }

struct SVariableMember {
    
    void* value;
    const char* type;
    
};

class STypeRegistry {

    public:
    
        static STypeRegistry* instance();
    
        SVariable getMemeber(SVariable* variable, std::string name);
		size_t getTypeSize(const std::string& type);
	
		template <class T>
		bool registerCPPClass(const char* name);
	
		void performCopy(void*& dest, void* from, const std::string& type);
	
        std::map<std::string, SClassFactoryBase*> factories;
        std::map<std::string, std::map<std::string, SVariableLocation>> variable_lookups;
		std::map<std::string, std::string> cpp_class_names;
	
        std::vector<std::string> registered_types;
    
};

#define EXPOSE_SCRIPT_TYPE(c) bool class_reg_##c = STypeRegistry::instance()->registerCPPClass<c>(#c);
#define EXPOSE_SCRIPT_MEMBER(c, n, t) STypeRegistry::instance()->variable_lookups[#c][#n].byte_offset = offsetof(c, n); STypeRegistry::instance()->variable_lookups[#c][#n].type = #t;

template <class T>
bool STypeRegistry::registerCPPClass(const char* name) {
	
	// Create a class factory and make sure we know we registered it as a type
	instance()->factories[name] = new SClassFactory<T>(sizeof(T));
	instance()->registered_types.push_back(name);
	instance()->cpp_class_names[typeid(T).name()] = name;
	
	return true;
	
}

#endif /* STypes_hpp */
