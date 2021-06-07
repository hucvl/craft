#pragma once

#include "Scene12Settings.h"
#include "SimulationBase.h"
#include "Box2D/Extension/b2VisBody.hpp"
#include "Box2D/Extension/b2VisPolygonShape.hpp"
#include <math.h> 

namespace svqa {
    class Scene12Simulation : public SimulationBase
    {
    public:
        typedef std::shared_ptr<Scene12Simulation> Ptr;
        Scene12Simulation(Scene12Settings::Ptr settings) : SimulationBase(settings)
        {

        }
        
        virtual SimulationID getIdentifier() override
        {
            return SimulationID::ID_Scene12;
        }
        
        void InitializeScene() override {



            // Basket
            float basket_pos_x = getExtremeCases(m_sStaticObjectOrientationType, -2.0f, 7.0f);
            AddTargetBasket(b2Vec2(basket_pos_x, -1.2f), 0.0f);
            
            // Right Most platform
            float right_most_y = getExtremeCases(m_sStaticObjectOrientationType, 5.0f, 9.0f);
            float right_most_x = getExtremeCases(m_sStaticObjectOrientationType, 14.0f, 17.0f);
            AddStaticObject(
                b2Vec2(right_most_x, right_most_y),
                0,
                SimulationObject::STATIC_PLATFORM);

            // Left Most Platform
            float left_most_y = getExtremeCases(m_sStaticObjectOrientationType, 25.0f, 35.0f);
            float left_most_x = getExtremeCases(m_sStaticObjectOrientationType, -14.0f, -17.0f);



            // Right platform
            float right_platform_pos_x = 9.0f;
            AddStaticObject(
                b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, 7.0f, 10.0f), 19.0f), 
                M_PI / 3.0f, 
                SimulationObject::STATIC_PLATFORM);


            // Mid platform
            AddStaticObject(b2Vec2(-2.0f, 14.0f), -M_PI / 3.0f, SimulationObject::STATIC_PLATFORM);


            // Left platform
            AddStaticObject(
                b2Vec2(-9.0f, getExtremeCases(m_sStaticObjectOrientationType, 10.0f, 15.0f)),
                M_PI / -4.0f, 
                SimulationObject::STATIC_PLATFORM);


            // Left Static ball
            AddStaticObject(b2Vec2(-2.0f, 27.0f), 0, SimulationObject::STATIC_BALL);


            if (m_bIncludeDynamicObjects) {

                
                AddRandomDynamicObject(
                    b2Vec2(left_most_x - 3.0, left_most_y + 8.0f),
                    b2Vec2(5.0f, 0.0f)
                );

                /*
                                AddRandomDynamicObject(
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, -16.0f, -4.0f), 46.5f),
                    b2Vec2(0.0f, 0.0f)
                );

                */

                AddRandomDynamicObject(
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, -4.0f, 6.0f), 56.5f),
                    b2Vec2(0.0f, 0.0f)
                );

                AddRandomDynamicObject(
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, -4.0f, 9.0f), 49.5f),
                    b2Vec2(0.0f, 0.0f)
                );

                AddRandomDynamicObject(
                    b2Vec2(right_most_x + 3.0, right_most_y + 3.0f),
                    b2Vec2(-5.0f, 0.0f),
                    0b100,
                    SimulationObject::CIRCLE
                );

            }
        }
            


    private:

    };
}
