#pragma once

#include "Scene4Settings.h"
#include "SimulationBase.h"
#include "Box2D/Extension/b2VisBody.hpp"
#include "Box2D/Extension/b2VisPolygonShape.hpp"
#include <math.h> 

namespace svqa {
	class Scene4Simulation : public SimulationBase
    {
    public:
        typedef std::shared_ptr<Scene4Simulation> Ptr;
        Scene4Simulation(Scene4Settings::Ptr settings) : SimulationBase(settings)
        {

        }
        
        virtual SimulationID getIdentifier() override
        {
            return SimulationID::ID_Scene4;
        }
        
        void InitializeScene() override {
            // Basket
            AddTargetBasket(b2Vec2(21.0f, -1.2f), 0.0f);
            
            /// TODO: Make statics random
            float h1 = getExtremeCases(m_sStaticObjectOrientationType, 22, 24);
            AddStaticObject(b2Vec2(-18.0f,h1), 0, SimulationObject::STATIC_PLATFORM);
            AddStaticObject(b2Vec2(-8.0f, h1), 0, SimulationObject::STATIC_PLATFORM);
            AddStaticObject(b2Vec2(-2.0f, h1), 0, SimulationObject::STATIC_PLATFORM);
            
            float h2 = getExtremeCases(m_sStaticObjectOrientationType, 15, 17);
            AddStaticObject(b2Vec2(-18.0f,h2), 0, SimulationObject::STATIC_PLATFORM);
            AddStaticObject(b2Vec2(-9.0f, h2), 0, SimulationObject::STATIC_PLATFORM);
            
            float h3 = getExtremeCases(m_sStaticObjectOrientationType, 9, 11);
            AddStaticObject(b2Vec2(-18.0f, h3), 0, SimulationObject::STATIC_PLATFORM);
            
            float h4 = getExtremeCases(m_sStaticObjectOrientationType, 3, 5);
            AddStaticObject(b2Vec2(-18.0f, h4), 0, SimulationObject::STATIC_PLATFORM);
            AddStaticObject(b2Vec2(-4.0f,  h4), 0, SimulationObject::STATIC_PLATFORM);
            AddStaticObject(b2Vec2(9.5f,   h4), 0, SimulationObject::STATIC_PLATFORM);
            
            
            if (m_bIncludeDynamicObjects) {
                
                AddRandomDynamicObject(
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType,-17.0f, -15.0f), 24.5f),
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType,5.0f, 15.0f), 0.0f),
                    0b100,
                    SimulationObject::Shape::CIRCLE
                );
                
                AddRandomDynamicObject(
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType,-18.0f, -16.0f), 17.8f),
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType,5.0f, 15.0f), 0.0f),
                    0b100,
                    SimulationObject::Shape::CIRCLE
                );
                
                AddRandomDynamicObject(
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType,-18.0f, -17.0f), 11.5f),
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType,5.0f, 15.0f), 0.0f),
                    0b100,
                    SimulationObject::Shape::CIRCLE
                );
                
                AddRandomDynamicObject(
                    b2Vec2(-18.0f, 5.5f),
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType,5.0f, 15.0f), 0.0f),
                    0b100,
                    SimulationObject::Shape::CIRCLE
                );
                
            }
            
        }
        
    private:

	};
}
