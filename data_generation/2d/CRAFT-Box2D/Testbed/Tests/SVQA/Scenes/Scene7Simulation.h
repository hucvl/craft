#pragma once

#include "Scene7Settings.h"
#include "SimulationBase.h"
#include "Box2D/Extension/b2VisBody.hpp"
#include "Box2D/Extension/b2VisPolygonShape.hpp"
#include <math.h> 

namespace svqa {
    class Scene7Simulation : public SimulationBase
    {
    public:
        typedef std::shared_ptr<Scene7Simulation> Ptr;
        Scene7Simulation(Scene7Settings::Ptr settings) : SimulationBase(settings)
        {

        }
        
        virtual SimulationID getIdentifier() override
        {
            return SimulationID::ID_Scene7;
        }
        
        void InitializeScene() override {
            // Basket
            AddTargetBasket(b2Vec2(getExtremeCases(m_sStaticObjectOrientationType,17, 20), -1.2f), 0.0f);
            
            // inclined
            AddStaticObject(b2Vec2(getExtremeCases(m_sStaticObjectOrientationType,-15, -12), getExtremeCases(m_sStaticObjectOrientationType,16, 18)), 7 * M_PI / 8,
                            SimulationObject::STATIC_PLATFORM);  // -13 17
            
            AddStaticObject(b2Vec2(  7.0f, getExtremeCases(m_sStaticObjectOrientationType,8, 9)), 7 * M_PI / 8,
                            SimulationObject::STATIC_PLATFORM);  //  8 < y < 9
            
            // mid
            AddStaticObject(b2Vec2(-10.0f, getExtremeCases(m_sStaticObjectOrientationType,8.0f, 9.0f)), M_PI,
                            SimulationObject::STATIC_PLATFORM);
            
            float x = getExtremeCases(m_sStaticObjectOrientationType,10, 34);
            AddStaticObject(b2Vec2(10.0f, getExtremeCases(m_sStaticObjectOrientationType,21,23)), - x * M_PI / (x+2),
                            SimulationObject::STATIC_PLATFORM);  // -13 17
            
            
            if (m_bIncludeDynamicObjects) {
                AddRandomDynamicObject(
                    b2Vec2(-12.0f, 25.0f),
                    b2Vec2(0.0f, 0.0f)
                );
                
                AddRandomDynamicObject(
                    b2Vec2(-15.0f, 38.0f),
                    b2Vec2(0.0f, 0.0f)
                );
                
                AddRandomDynamicObject(
                    b2Vec2(10.0f, 26.0f),
                    b2Vec2(0.0f, 0.0f)
                );
                
                AddRandomDynamicObject(
                    b2Vec2(-12.0f, 11.0f),
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType,3, 6), 0.0f),
                    0b110,
                    SimulationObject::CIRCLE,
                    SimulationObject::LARGE
                );
            }
            
            
        }
        
    private:

    };
}
