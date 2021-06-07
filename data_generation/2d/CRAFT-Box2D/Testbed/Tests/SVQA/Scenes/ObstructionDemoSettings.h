#pragma once

#include "Settings.h"

namespace svqa
{
    struct ObstructionDemoSettings : Settings
    {
        typedef std::shared_ptr<ObstructionDemoSettings> Ptr;
        
        ObstructionDemoSettings() {}
        
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
