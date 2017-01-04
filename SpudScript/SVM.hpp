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

class SVM {
    
    public:
    
        void executeCode(std::vector<SASTNode*> nodes);
        float evaluateNode(SASTNode* node);
    
        SVariable* resolveVarible(std::string name);
    
    //private:
    
        void castVariable(std::string type, std::string name);
        std::map<std::string, SVariable> global_variables;
        std::map<std::string, void(*)()> functions;
        std::map<std::string, SBlock*> script_functions;
    
        SBlock* current_block;
    
};

#endif /* SVM_hpp */
