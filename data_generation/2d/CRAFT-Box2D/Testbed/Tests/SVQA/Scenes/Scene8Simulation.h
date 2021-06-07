#pragma once

#include "Scene8Settings.h"
#include "SimulationBase.h"
#include "Box2D/Extension/b2VisBody.hpp"
#include "Box2D/Extension/b2VisPolygonShape.hpp"
#include <math.h> 

namespace svqa {
    class Scene8Simulation : public SimulationBase
    {
    public:
        typedef std::shared_ptr<Scene8Simulation> Ptr;
        Scene8Simulation(Scene8Settings::Ptr settings) : SimulationBase(settings)
        {

        }
        
        virtual SimulationID getIdentifier() override
        {
            return SimulationID::ID_Scene8;
        }
        
        void InitializeScene() override {
            // TODO: Does not easily produce intended outcomes.
            //       We may need to change positions and orientations of dynamic objects,
            //       especially one at the top.
            
            // Basket
            AddTargetBasket(b2Vec2(getExtremeCases(m_sStaticObjectOrientationType,19, 22),
                                   getExtremeCases(m_sStaticObjectOrientationType,17, 20)), 0.0f);
            
            // top
            float topY = getExtremeCases(m_sStaticObjectOrientationType,28, 30);
            AddStaticObject(b2Vec2(getExtremeCases(m_sStaticObjectOrientationType,-9, -11), topY - 2.0f),
                            4 * M_PI / 5, SimulationObject::STATIC_PLATFORM);
            AddStaticObject(b2Vec2(getExtremeCases(m_sStaticObjectOrientationType,-1, 1), topY),
                            4 * M_PI / 5, SimulationObject::STATIC_PLATFORM);
            
            
            //AddStaticObject(b2Vec2(getExtremeCases(m_sStaticObjectOrientationType,6, 8), topY), M_PI, SimulationObject::STATIC_PLATFORM);
            //AddStaticObject(b2Vec2(-getExtremeCases(m_sStaticObjectOrientationType,-4, -2), topY), M_PI, SimulationObject::STATIC_PLATFORM);
            // TODO: Maybe change the direction of this object?
            
            // bottom
            float bottomPlatformHeight = getExtremeCases(m_sStaticObjectOrientationType,6, 8.5f);
            float bottomLayeredRampHeight = bottomPlatformHeight + 4.6;
            
            auto layeredRampHeight = [bottomLayeredRampHeight](int layerCount) {
                float layerHeight = 3.25f;
                return bottomLayeredRampHeight + layerHeight * layerCount;
            };
            AddStaticObject(b2Vec2(-17.0f, bottomPlatformHeight), M_PI, SimulationObject::STATIC_PLATFORM);
            AddStaticObject(b2Vec2(-2.8f, layeredRampHeight(0)), -4 * M_PI / 5, SimulationObject::STATIC_PLATFORM);
            AddStaticObject(b2Vec2(3.3f, layeredRampHeight(1)), -4 * M_PI / 5, SimulationObject::STATIC_PLATFORM);
            AddStaticObject(b2Vec2(9.3f, layeredRampHeight(2)), -4 * M_PI / 5, SimulationObject::STATIC_PLATFORM);
            
            
            if (m_bIncludeDynamicObjects) {
                    AddRandomDynamicObject(
                        b2Vec2(-12, topY + getExtremeCases(m_sStaticObjectOrientationType,26, 38)),
                        b2Vec2(0, 0.0f)
                    );
                    
                    AddRandomDynamicObject(
                        b2Vec2(-2 , topY +getExtremeCases(m_sStaticObjectOrientationType,26, 38)),
                        b2Vec2(0, 0.0f)
                    );
                    
                    AddRandomDynamicObject(
                        b2Vec2((getExtremeCases(m_sStaticObjectOrientationType,-13, -1)) ,
                               topY +getExtremeCases(m_sStaticObjectOrientationType,43, 52)),
                        b2Vec2(0, 0.0f)
                    );

                    AddRandomDynamicObject(
                        b2Vec2(-getExtremeCases(m_sStaticObjectOrientationType,15, 17), bottomPlatformHeight + 2),
                        b2Vec2(getExtremeCases(m_sStaticObjectOrientationType,24, 33), 0.0f), // 20 40
                        0b100,
                        SimulationObject::CIRCLE
                    );
            }
            
        }
        
    private:

    };
}
