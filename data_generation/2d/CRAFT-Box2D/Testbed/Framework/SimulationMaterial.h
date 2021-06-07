//
//  SimulationMaterial.h
//  Testbed
//
//  Created by Tayfun Ateş on 5.10.2019.
//

#ifndef SimulationMaterial_h
#define SimulationMaterial_h

#include <string>
#include "Box2D/Extension/b2VisTexture.hpp"
#include <nlohmann/json.hpp>

class SimulationMaterial
{
public:
    enum TYPE
    {
        EYES = 0,
        PLATFORM = 1,
        SENSOR = 2,
    };

    SimulationMaterial(TYPE t)
    {
        type = t;
    }

    float GetDensity()
    {
        return 5.0F;
    }

    TYPE type;

    //Creates the texture associated with the material
    b2VisTexture::Ptr getTexture();

private:
    static const std::string eyesFilePath;
    static const std::string platformFilePath;
    static const std::string sensorFilePath;

    static b2VisTexture::Ptr eyesTexture;
    static b2VisTexture::Ptr platformTexture;
    static b2VisTexture::Ptr sensorTexture;
};

NLOHMANN_JSON_SERIALIZE_ENUM(SimulationMaterial::TYPE, {
    {SimulationMaterial::EYES, "eyes"},
    {SimulationMaterial::PLATFORM, "platform"},
    {SimulationMaterial::SENSOR, "sensor"},
    })

#endif /* SimulationMaterial_h */
