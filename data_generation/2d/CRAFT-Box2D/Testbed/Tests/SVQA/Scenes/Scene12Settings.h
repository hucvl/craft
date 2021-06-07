#pragma once

#include "Settings.h"

namespace svqa
{
    struct Scene12Settings : Settings
    {
        typedef std::shared_ptr<Scene12Settings> Ptr;
        
        Scene12Settings() {}
        
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
