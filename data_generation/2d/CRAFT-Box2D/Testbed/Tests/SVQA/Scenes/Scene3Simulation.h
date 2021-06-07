#pragma once

#include "Scene3Settings.h"
#include "SimulationBase.h"
#include "Box2D/Extension/b2VisBody.hpp"
#include "Box2D/Extension/b2VisPolygonShape.hpp"
#include <math.h> 

namespace svqa {
    class Scene3Simulation : public SimulationBase
    {
    public:
        typedef std::shared_ptr<Scene3Simulation> Ptr;
        Scene3Simulation(Scene3Settings::Ptr settings) : SimulationBase(settings)
        {

        }
        
        virtual SimulationID getIdentifier() override
        {
            return SimulationID::ID_Scene3;
        }
        
        void InitializeScene() override {
            
            float32 bottomHeight = getExtremeCases(m_sStaticObjectOrientationType,1.0f, 4.0f);
            
            // Basket
            AddTargetBasket(b2Vec2(21.0f, -1.2f), 0.0f);
            
            
            // ------- Bottom Platform -------
            AddStaticObject(b2Vec2(-10.0f, bottomHeight), 0, SimulationObject::STATIC_PLATFORM);
            AddStaticObject(b2Vec2(-2.0f, bottomHeight), 0, SimulationObject::STATIC_PLATFORM);
            AddStaticObject(b2Vec2(9.5f, bottomHeight), 0, SimulationObject::STATIC_PLATFORM);
            
            
            // ------- Inclined Platform-------
            AddStaticObject(b2Vec2(-13.0f, getExtremeCases(m_sStaticObjectOrientationType,18.8,19.6)), 4 * M_PI / 5,
                            SimulationObject::STATIC_PLATFORM);
            
            // ------- Top Platform-------
            AddStaticObject(b2Vec2(getExtremeCases(m_sStaticObjectOrientationType,-13, -11.0f), 9.2f), 0, SimulationObject::STATIC_PLATFORM);
            AddStaticObject(b2Vec2(getExtremeCases(m_sStaticObjectOrientationType,8.0f, 13.0f), 9.2f), 0, SimulationObject::STATIC_PLATFORM);
            
            if (m_bIncludeDynamicObjects){
                
                AddRandomDynamicObject(
                    b2Vec2(-18.0f, getExtremeCases(m_sStaticObjectOrientationType,26.0f, 35.0f)),
                    b2Vec2(0.0f, 0.0f)
                );
                
                AddRandomDynamicObject(
                    b2Vec2(-18.0f, 11.2f),
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType,5.0f, 10.0f), 0.0f),
                    0b100,
                    SimulationObject::Shape::CIRCLE);
                
                AddRandomDynamicObject(
                    b2Vec2(-18.0f, bottomHeight + 2.0f),
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType,5.0f, 10.0f), 0.0f),
                    0b100,
                    SimulationObject::Shape::CIRCLE);
            }
            
        }
        
    private:

    };
}
