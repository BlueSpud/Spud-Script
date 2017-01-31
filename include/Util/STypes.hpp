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

#ifdef GCC
	#pragma GCC visibility push(hidden)
#endif

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
    
		virtual void* createObject() {
			
			// Create space for the object and then allocate the object inside of the block
			void* memory = calloc(1,size);
			new (memory) T();
			
			return memory;
		}

};

class SStringFactory : public SClassFactoryBase {

	virtual void* createObject() {
		
		// First we allocate space for a string and a pointer
		char* buffer = (char*)calloc(1,sizeof(char) * 2);
		//sprintf(buffer, "");
		
		char** ptr = (char**)calloc(1,sizeof(char**));
		ptr[0] = buffer;
		
		return ptr;
		
	}
	
};

struct SVariableLocation {

    size_t byte_offset;
    size_t type;

};

struct SVariable {
    
    void* value = nullptr;
    size_t type = 0;
    
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
		size_t getTypeSize(size_t type);
	
		static size_t hashString(const std::string& type);
		bool isOfType(size_t check, const std::string type);
	
		template <class T>
		bool registerCPPClass(const char* name);
	
		bool registerCPPMember(std::string class_name, std::string name, size_t offset, size_t type);
	
		void performCopy(void*& dest, void* from, size_t type);
	
		std::hash<std::string> hasher;
	
        std::map<size_t, SClassFactoryBase*> factories;
        std::map<size_t, std::map<std::string, SVariableLocation>> variable_lookups;
		std::map<size_t, size_t> cpp_class_names;
	
		std::vector<std::string> registered_types;
    
};

template <class T>
bool STypeRegistry::registerCPPClass(const char* name) {
	
	// Create a class factory and make sure we know we registered it as a type
	instance()->factories[hasher(name)] = new SClassFactory<T>(sizeof(T));
	instance()->registered_types.push_back(name);
	instance()->cpp_class_names[hasher(typeid(T).name())] = hasher(name);
	
	return true;
	
}

#ifdef GCC
	#pragma GCC visibility pop
#endif

#define EXPOSE_SCRIPT_TYPE(c) bool class_reg_##c = STypeRegistry::instance()->registerCPPClass<c>(#c);
#define EXPOSE_SCRIPT_MEMBER(c, n, t) bool is_registered_##c##n = STypeRegistry::instance()->registerCPPMember(#c, #n, offsetof(c, n), STypeRegistry::instance()->hasher(#t));

#endif /* STypes_hpp */
