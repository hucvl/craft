#pragma once

#include "Settings.h"

namespace svqa
{
    struct Scene17Settings : Settings
    {
        typedef std::shared_ptr<Scene17Settings> Ptr;
        
        Scene17Settings() {}
        
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
