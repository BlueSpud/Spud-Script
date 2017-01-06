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
#include <map>

class SClassFactory {

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

struct SVariableMember {
    
    void* value;
    const char* type;
    
};

class STypeRegistry {

    public:
    
        static STypeRegistry* instance();
    
        SVariable getMemeber(SVariable* variable, std::string name);
    
        std::map<std::string, SClassFactory*> factories;
        std::map<std::string, std::map<std::string, SVariableLocation>> variable_lookups;
    
};

#define EXPOSE_SCRIPT_TYPE(c)  { c* template_object = new c(); STypeRegistry::instance()->factories[#c] = new SClassFactory((void*)template_object, sizeof(c)); delete template_object; }

#define EXPOSE_SCRIPT_VARIABLE(c, n, t) STypeRegistry::instance()->variable_lookups[#c][#n].byte_offset = offsetof(c, n); STypeRegistry::instance()->variable_lookups[#c][#n].type = #t;

#endif /* STypes_hpp */
