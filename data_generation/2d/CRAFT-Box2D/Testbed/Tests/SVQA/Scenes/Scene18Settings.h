#pragma once

#include "Settings.h"

namespace svqa
{
    struct Scene18Settings : Settings
    {
        typedef std::shared_ptr<Scene18Settings> Ptr;
        
        Scene18Settings() {}
        
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
