#pragma once

#include "Settings.h"

namespace svqa
{
    struct Scene9Settings : Settings
    {
        typedef std::shared_ptr<Scene9Settings> Ptr;
        
        Scene9Settings() {}
        
        void to_json(json& j) {
            Settings::to_json(j);
        }
        
        void from_json(const json& j) {
            Settings::from_json(j);
        }
    };
}
