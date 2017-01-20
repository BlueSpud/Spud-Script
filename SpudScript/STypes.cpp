//
//  STypes.cpp
//  SpudScript
//
//  Created by Logan Pazol on 1/5/17.
//  Copyright © 2017 Logan Pazol. All rights reserved.
//

#include "STypes.hpp"

EXPOSE_SCRIPT_TYPE(int);
EXPOSE_SCRIPT_TYPE(float);
EXPOSE_SCRIPT_TYPE(long);
EXPOSE_SCRIPT_TYPE(double);
EXPOSE_SCRIPT_TYPE(char);
EXPOSE_SCRIPT_TYPE(bool);

STypeRegistry* STypeRegistry::instance() {

    static STypeRegistry* instance = NULL;
    
	if (instance == NULL) {
		
        instance = new STypeRegistry();
		
		// We do a special registration of strings, use char*
		instance->factories[instance->hasher("string")] = new SStringFactory();
		instance->registered_types.push_back("string");
		instance->cpp_class_names[instance->hasher(typeid(char*).name())] = instance->hasher("string");
		
	}
	
    return instance;

}

size_t STypeRegistry::getTypeSize(size_t type) {
	
	// Check if it is a normal data type
	if (factories.count(type))
		return factories[type]->size;
	else return 4;
	
}

size_t STypeRegistry::hashString(const std::string& type) { return instance()->hasher(type); }

bool STypeRegistry::isOfType(size_t check, const std::string type) {
	
	// Hash the type
	size_t hash = hasher(type);
	return hash == check;
}

SVariable STypeRegistry::getMemeber(SVariable* variable, std::string name) {

    SVariable member;

    // Check that we can resolve it first
    if (instance()->variable_lookups.count(variable->type) && instance()->variable_lookups[variable->type].count(name)) {
    
        char* base_pointer = reinterpret_cast<char*>(variable->value);
        member.value = reinterpret_cast<void*>(base_pointer + instance()->variable_lookups[variable->type][name].byte_offset);
		member.type = instance()->variable_lookups[variable->type][name].type;
    
    }
    
    return member;

}

void STypeRegistry::performCopy(void*& dest, void* from, size_t type) {
	
	// Strings need to be deep copied
	if (STypeRegistry::instance()->isOfType(type, "string")) {
		
		// Figure out how big we need to allocate
		char** from_pointer = (char**)from;
		size_t length = strlen(from_pointer[0]);
		
		// Create a new block for the contents and copy it in
		char* buffer = (char*)malloc(sizeof(char) * (length + 1));
		memcpy(buffer, from_pointer[0], sizeof(char) * (length + 1));
		
		// Make the destinaiton pointer
		char** ptr = (char**)malloc(sizeof(char**));
		ptr[0] = buffer;
		
		memcpy(dest, ptr, sizeof(char**));
		free(ptr);
		
		return;
		
	}
	
	// Do a regular copy
	size_t size = instance()->getTypeSize(type);
	memcpy(dest, from, size);
	
}
