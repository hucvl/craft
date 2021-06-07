#pragma once

#include "Settings.h"

namespace svqa
{
    struct Scene15Settings : Settings
    {
        typedef std::shared_ptr<Scene15Settings> Ptr;
        
        Scene15Settings() {}
        
        int numberOfObjects;
        int numberOfObstacles;
        
        void to_json(json& j) {
            Settings::to_json(j);
        }
        
        void from_json(const json& j) {
            Settings::from_json(j);
        }
    };
}
