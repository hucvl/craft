#pragma once

#include "Settings.h"

namespace svqa
{
    struct Scene6Settings : Settings
    {
        typedef std::shared_ptr<Scene6Settings> Ptr;
        
        Scene6Settings() {}
        
        void to_json(json& j) {
            Settings::to_json(j);
        }
        
        void from_json(const json& j) {
            Settings::from_json(j);
        }
    };
}
