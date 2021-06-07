//
//  SimulationMaterial.cpp
//  Testbed
//
//  Created by Tayfun Ateş on 6.10.2019.
//

#include "SimulationMaterial.h"

const std::string SimulationMaterial::eyesFilePath = "Data/Textures/eyes.png";

b2VisTexture::Ptr SimulationMaterial::eyesTexture;
b2VisTexture::Ptr SimulationMaterial::platformTexture;
b2VisTexture::Ptr SimulationMaterial::sensorTexture;

b2VisTexture::Ptr SimulationMaterial::getTexture()
{
    if (!SimulationMaterial::platformTexture) {
        SimulationMaterial::platformTexture = b2VisTexture::Ptr(new b2VisTexture( SimulationMaterial::TYPE::PLATFORM));
    }
    
    if (!SimulationMaterial::sensorTexture) {
        SimulationMaterial::sensorTexture = b2VisTexture::Ptr(new b2VisTexture( SimulationMaterial::TYPE::SENSOR));
    }
    
    if (!SimulationMaterial::eyesTexture) {
        SimulationMaterial::eyesTexture = b2VisTexture::Ptr(new b2VisTexture(SimulationMaterial::eyesFilePath, SimulationMaterial::TYPE::EYES));
    }
    
    if (type == EYES) {
        return SimulationMaterial::eyesTexture;
    }
     
    if (type == PLATFORM) {
        return SimulationMaterial::platformTexture;
    }
    
    if (type == SENSOR) {
        return SimulationMaterial::sensorTexture;
    }
    
    return nullptr;
}
