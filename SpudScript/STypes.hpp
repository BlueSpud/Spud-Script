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

class SClassFactory {

	friend class STypeRegistry;
	
    public:
    
        SClassFactory(void* _template_object, size_t _size);
        ~SClassFactory();
    
        void* createObject();
    
    private:
    
        void* template_object;
        size_t size;

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
	
		size_t getTypeSize(std::string type);
	
        std::map<std::string, SClassFactory*> factories;
        std::map<std::string, std::map<std::string, SVariableLocation>> variable_lookups;
    
        std::vector<std::string> registered_types;
    
};

#define EXPOSE_SCRIPT_TYPE(c)  { c* template_object = new c(); STypeRegistry::instance()->factories[#c] = new SClassFactory((void*)template_object, sizeof(c)); delete template_object; } STypeRegistry::instance()->registered_types.push_back(#c);

#define EXPOSE_SCRIPT_VARIABLE(c, n, t) STypeRegistry::instance()->variable_lookups[#c][#n].byte_offset = offsetof(c, n); STypeRegistry::instance()->variable_lookups[#c][#n].type = #t;

#endif /* STypes_hpp */
