//
//  SVM.hpp
//  SpudScript
//
//  Created by Logan Pazol on 12/31/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SVM_hpp
#define SVM_hpp

#include <map>
#include "SParser.hpp"

struct SVariable {
    
    float value;
    std::string type;
    
};

class SVM {
    
    public:
    
        void executeCode(std::vector<SASTNode*> nodes);
        float evaluateNode(SASTNode* node);
    
    //private:
    
        void castVariable(std::string type, std:: string variable);
        std::map<std::string, SVariable> variables;
        std::map<std::string, void(*)()> functions;
        std::map<std::string, SBlock*> script_functions;
    
};

#endif /* SVM_hpp */
