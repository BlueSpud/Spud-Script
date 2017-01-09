//
//  STypes.cpp
//  SpudScript
//
//  Created by Logan Pazol on 1/5/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#include "STypes.hpp"


SClassFactory::SClassFactory(void* _template_object, size_t _size) {
    
    // Caller is responsible for deleting _template_object
    size = _size;
    template_object = malloc(size);
    memcpy(template_object, _template_object, size);
    
}

SClassFactory::~SClassFactory() { free(template_object); }

void* SClassFactory::createObject() {

    // Copy template object and return allocated object
    void* new_object = malloc(size);
    memcpy(new_object, template_object, size);
    return new_object;

}

STypeRegistry* STypeRegistry::instance() {

    static STypeRegistry* instance = NULL;
    
    if (instance == NULL) {
    
        instance = new STypeRegistry();
        
        // Register the simple types
        instance->registered_types.push_back("int");
        instance->registered_types.push_back("float");
        instance->registered_types.push_back("double");
        instance->registered_types.push_back("bool");
        instance->registered_types.push_back("char");
        
    }
    
    return instance;

}

size_t STypeRegistry::getTypeSize(std::string type) {
	
	// Check if it is a custom data type
	if (factories.count(type))
		return factories[type]->size;
	else return 4;
	
}

SVariable STypeRegistry::getMemeber(SVariable* variable, std::string name) {

    SVariable member;

    // Check that we can resolve it first
    if (instance()->variable_lookups.count(variable->type) && instance()->variable_lookups[variable->type].count(name)) {
    
        char* base_pointer = reinterpret_cast<char*>(variable->value);
        member.value = reinterpret_cast<void*>(base_pointer + instance()->variable_lookups[variable->type][name].byte_offset);
        member.type = instance()->variable_lookups[variable->type][name].type.c_str();
    
    }
    
    return member;

}
