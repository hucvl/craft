#pragma once

#include "Scene6Settings.h"
#include "SimulationBase.h"
#include "Box2D/Extension/b2VisBody.hpp"
#include "Box2D/Extension/b2VisPolygonShape.hpp"
#include <math.h> 

namespace svqa {
    class Scene6Simulation : public SimulationBase
    {
    public:
        typedef std::shared_ptr<Scene6Simulation> Ptr;
        Scene6Simulation(Scene6Settings::Ptr settings) : SimulationBase(settings)
        {

        }
        
        virtual SimulationID getIdentifier() override
        {
            return SimulationID::ID_Scene6;
        }
        
        
        void InitializeScene() override {
            // Basket
            // make x coord. random
            AddTargetBasket(b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, 5.0, 10.0), -1.2f), 0.0f);
            
            //left
            // make angle random
            AddStaticObject(b2Vec2(-12.0f, 28.0f), 5*  M_PI  / getExtremeCases(m_sStaticObjectOrientationType, 6.25, 7.0),
                            SimulationObject::STATIC_PLATFORM);
            
            //right
            // make angle random
            AddStaticObject(b2Vec2(16.0f, 14.0f), - 15 * M_PI / getExtremeCases(m_sStaticObjectOrientationType, 16.0, 19.0),
                            SimulationObject::STATIC_PLATFORM);
            
            
            // mid
            // make y-coord random
            AddStaticObject(b2Vec2(5.0f, getExtremeCases(m_sStaticObjectOrientationType, 17.0, 20.0)), M_PI, SimulationObject::STATIC_PLATFORM);
            
            // bottom
            float random_4 = getExtremeCases(m_sStaticObjectOrientationType, 3.0, 9.0);
            AddStaticObject(b2Vec2(-5.0f,  random_4),  M_PI , SimulationObject::STATIC_PLATFORM);
            AddStaticObject(b2Vec2(-19.0f, random_4),  M_PI , SimulationObject::STATIC_PLATFORM);
            
            
            if (m_bIncludeDynamicObjects){
                // make y-coord random
                AddRandomDynamicObject(
                    b2Vec2(-12.0f, 31.0f),
                    b2Vec2(0.0f, -15.0f)
                );
                
                // make x coord. random
                // make Velocity random
                AddRandomDynamicObject(
                    b2Vec2(4.0f, getExtremeCases(m_sStaticObjectOrientationType, 17.0, 20.0) + 2.0f),
                    b2Vec2(-8.0f, 0.0f),
                    0b100,
                    SimulationObject::CIRCLE
                );
                
                AddRandomDynamicObject(
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType,-20.0, -16.0), random_4 + 2.0f),  // make x coord. random
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType,8.0, 3.0), 0.0f),     // make x-velocity random
                    0b100,
                    SimulationObject::CIRCLE
                );
            }
        }
        
    private:

    };
}
