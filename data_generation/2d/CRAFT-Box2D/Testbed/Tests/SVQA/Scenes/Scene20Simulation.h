#pragma once

#include "Scene20Settings.h"
#include "SimulationBase.h"
#include "Box2D/Extension/b2VisBody.hpp"
#include "Box2D/Extension/b2VisPolygonShape.hpp"
#include <math.h> 

namespace svqa {
    class Scene20Simulation : public SimulationBase
    {
    public:
        typedef std::shared_ptr<Scene20Simulation> Ptr;
        Scene20Simulation(Scene20Settings::Ptr settings) : SimulationBase(settings)
        {

        }
        
        virtual SimulationID getIdentifier() override
        {
            return SimulationID::ID_Scene20;
        }
        
        void InitializeScene() override {

            // Basket   
            AddTargetBasket(b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, 15, 20), -1.2f), 0.0f);


            float y = getExtremeCases(m_sStaticObjectOrientationType, 6, 8);
            AddStaticObject(b2Vec2(0, y), 0, SimulationObject::STATIC_PLATFORM);
            AddStaticObject(b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, 3, 7), y), 0, SimulationObject::STATIC_PLATFORM);


            float y2 = getExtremeCases(m_sStaticObjectOrientationType, 13, 16);
            AddStaticObject(b2Vec2(-18, y2), -M_PI / getExtremeCases(m_sStaticObjectOrientationType, 4, 5), SimulationObject::STATIC_PLATFORM);


            AddStaticObject(b2Vec2(-8, 28),  M_PI / 6, SimulationObject::STATIC_PLATFORM);
            AddStaticObject(b2Vec2(7, 37), M_PI / 6, SimulationObject::STATIC_PLATFORM);


            if (m_bIncludeDynamicObjects) {
               
                float y3 = getExtremeCases(m_sStaticObjectOrientationType, 42, 46);


                AddRandomDynamicObject(b2Vec2(10, y3), b2Vec2(-1.0f, -1.0f));
                AddRandomDynamicObject(b2Vec2(1, y3-4), b2Vec2(-1.0f, -1.0f));
                AddRandomDynamicObject(b2Vec2(-10, y3-12), b2Vec2(-1.5f, -1.5f), 0b100, SimulationObject::TRIANGLE);
            }


        }
            


    private:

    };
}
