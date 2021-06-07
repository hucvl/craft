#pragma once

#include "Settings.h"

namespace svqa
{
    struct Scene8Settings : Settings
    {
        typedef std::shared_ptr<Scene8Settings> Ptr;
        
        Scene8Settings() {}
        
        
        void to_json(json& j) {
            Settings::to_json(j);
        }
        
        void from_json(const json& j) {
            Settings::from_json(j);
        }
    };
}
