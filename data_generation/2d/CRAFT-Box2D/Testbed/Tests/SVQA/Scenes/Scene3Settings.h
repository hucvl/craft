#pragma once
#pragma once

#include "Settings.h"

namespace svqa
{
    struct Scene3Settings : Settings
    {
        typedef std::shared_ptr<Scene3Settings> Ptr;
        
        Scene3Settings() {}
        
        
        void to_json(json& j) {
            
            Settings::to_json(j);
        }
        
        void from_json(const json& j) {
            Settings::from_json(j);
            
        }
    };
}
