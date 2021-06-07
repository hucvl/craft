#pragma once

#include "Settings.h"

namespace svqa
{
    struct Scene1Settings : Settings
    {
        typedef std::shared_ptr<Scene1Settings> Ptr;
        
        Scene1Settings() {}
        
        
        void to_json(json& j) {
            
            Settings::to_json(j);
        }
        
        void from_json(const json& j) {
            Settings::from_json(j);
            
        }
    };
}
