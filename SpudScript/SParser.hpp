//
//  SParser.hpp
//  SpudScript
//
//  Created by Logan Pazol on 12/28/16.
//  Copyright © 2016 Logan Pazol. All rights reserved.
//

#ifndef SParser_hpp
#define SParser_hpp

#include "SLexer.hpp"
#include "SASTNodes.h"

class SAST {
    
    public:
    
        std::vector<SASTNode*> parseTokens(std::vector<SToken>& tokens);
    
};

#endif /* SParser_hpp */
