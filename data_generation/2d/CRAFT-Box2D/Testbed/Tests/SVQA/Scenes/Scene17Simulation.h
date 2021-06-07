#pragma once

#include "Scene17Settings.h"
#include "SimulationBase.h"
#include "Box2D/Extension/b2VisBody.hpp"
#include "Box2D/Extension/b2VisPolygonShape.hpp"
#include <math.h> 

namespace svqa {
    class Scene17Simulation : public SimulationBase
    {
    public:
        typedef std::shared_ptr<Scene17Simulation> Ptr;
        Scene17Simulation(Scene17Settings::Ptr settings) : SimulationBase(settings)
        {

        }
        
        virtual SimulationID getIdentifier() override
        {
            return SimulationID::ID_Scene17;
        }
        
        void InitializeScene() override {


            // from SCENE 4
            
            // Basket
            AddTargetBasket(b2Vec2(21.0f, -1.2f), 0.0f);

            /// TODO: Make statics random
            float h1 = getExtremeCases(m_sStaticObjectOrientationType, 25, 27);
            AddStaticObject(b2Vec2(-18.0f, h1), 0, SimulationObject::STATIC_PLATFORM);
            //AddStaticObject(b2Vec2(-12.0f, h1), 0, SimulationObject::STATIC_PLATFORM);
            //AddStaticObject(b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, 5, 7), h1), 0, SimulationObject::STATIC_PLATFORM);

            float h2 = getExtremeCases(m_sStaticObjectOrientationType, 16, 19);
            AddStaticObject(b2Vec2(-18.0f, h2), 0, SimulationObject::STATIC_PLATFORM);
            AddStaticObject(b2Vec2(-11.0f, h2), 0, SimulationObject::STATIC_PLATFORM);

            float h3 = getExtremeCases(m_sStaticObjectOrientationType, 10, 13);
            AddStaticObject(b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, 11, 15), h3), 0, SimulationObject::STATIC_PLATFORM);

            float h4 = getExtremeCases(m_sStaticObjectOrientationType, 3, 5);
            AddStaticObject(b2Vec2(-18.0f, h4), 0, SimulationObject::STATIC_PLATFORM);
            AddStaticObject(b2Vec2(-4.0f, h4), 0, SimulationObject::STATIC_PLATFORM);
            AddStaticObject(b2Vec2(8.5f, h4), 0, SimulationObject::STATIC_PLATFORM);


            if (m_bIncludeDynamicObjects) {

                AddRandomDynamicObject(
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, -17.0f, -15.0f), h1+1.5f),
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, 9.0f, 15.0f), 0.0f),
                    0b100,
                    SimulationObject::Shape::CIRCLE
                );

                AddRandomDynamicObject(
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, -18.0f, -16.0f), h2+1.5f),
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, 9.0f, 15.0f), 0.0f),
                    0b100,
                    SimulationObject::Shape::CIRCLE
                );

                AddRandomDynamicObject(
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, 16.0f, 18.0f), h3+1.5f),
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, -10.0f, -5.0f), 0.0f)
                );

                AddRandomDynamicObject(
                    b2Vec2(-18.0f, 5.5f),
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, 9.0f, 15.0f), 0.0f),
                    0b100,
                    SimulationObject::Shape::CIRCLE
                );

            }

        }
            


    private:

    };
}
