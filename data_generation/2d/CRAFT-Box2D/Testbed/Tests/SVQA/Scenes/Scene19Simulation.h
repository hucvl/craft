#pragma once

#include "Scene19Settings.h"
#include "SimulationBase.h"
#include "Box2D/Extension/b2VisBody.hpp"
#include "Box2D/Extension/b2VisPolygonShape.hpp"
#include <math.h> 

namespace svqa {
    class Scene19Simulation : public SimulationBase
    {
    public:
        typedef std::shared_ptr<Scene19Simulation> Ptr;
        Scene19Simulation(Scene19Settings::Ptr settings) : SimulationBase(settings)
        {

        }
        
        virtual SimulationID getIdentifier() override
        {
            return SimulationID::ID_Scene19;
        }
        
        void InitializeScene() override {
           
            // Basket
            AddTargetBasket(b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, -19.0, -15.0), -1.2f), 0.0f);

            // ------- Top Right Platform and Circle -------
            AddStaticObject(b2Vec2(17.0f, 19.0f), 0, SimulationObject::STATIC_PLATFORM);
            //AddStaticObject(b2Vec2(9.0f, 19.0f), 0, SimulationObject::STATIC_PLATFORM);
            AddStaticObject(b2Vec2(11.0f, 19.0f), 0, SimulationObject::STATIC_RAMP);

            // ------- Inclined Platform and Rolling Circle -------
            AddStaticObject(b2Vec2(-26.5f, getExtremeCases(m_sStaticObjectOrientationType, 11.0, 14.0)), 6 * M_PI / 7, SimulationObject::STATIC_PLATFORM);

            // ------- Bottom Left Platform and Circle -------
            AddStaticObject(b2Vec2(-7.5f, getExtremeCases(m_sStaticObjectOrientationType, 5, 7)), 0, SimulationObject::STATIC_PLATFORM);
            // ------- Bottom Right Platform and Circle -------
            AddStaticObject(b2Vec2(17.0f, getExtremeCases(m_sStaticObjectOrientationType, 5, 7)), 0, SimulationObject::STATIC_PLATFORM);


            if (m_bIncludeDynamicObjects) {


                float32 tr_circle_velocities[5] = { -24.0f, -28.0f, -25.0f, -18.0f, -19.0f };

                AddRandomDynamicObject(
                    b2Vec2(22.0f, 20.0f),
                    b2Vec2(tr_circle_velocities[(int)RandomFloatFromHardware(0.0, 5.0)], 0.0f),
                    0b100,
                    SimulationObject::CIRCLE
                );

                AddRandomDynamicObject(
                    b2Vec2(-22.0f, RandomFloatFromHardware(19.0, 35.0)),
                    b2Vec2(0.0f, RandomFloatFromHardware(-9.0, 0.0))
                );

                AddRandomDynamicObject(
                    b2Vec2(RandomFloatFromHardware(-12.0, -4.0), 8.0f),
                    b2Vec2(0.0f, 0.0f)
                );

                AddRandomDynamicObject(
                    b2Vec2(22.0f, 8.0f),
                    b2Vec2(RandomFloatFromHardware(-29.0, -20.0), 0.0f)
                );

            }
      
        }
            


    private:

    };
}
