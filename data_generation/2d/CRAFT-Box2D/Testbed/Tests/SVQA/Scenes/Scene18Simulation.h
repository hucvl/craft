#pragma once

#include "Scene18Settings.h"
#include "SimulationBase.h"
#include "Box2D/Extension/b2VisBody.hpp"
#include "Box2D/Extension/b2VisPolygonShape.hpp"
#include <math.h> 

namespace svqa {
    class Scene18Simulation : public SimulationBase
    {
    public:
        typedef std::shared_ptr<Scene18Simulation> Ptr;
        Scene18Simulation(Scene18Settings::Ptr settings) : SimulationBase(settings)
        {

        }
        
        virtual SimulationID getIdentifier() override
        {
            return SimulationID::ID_Scene18;
        }
        
        void InitializeScene() override {

            // from SCENE 1

            // Basket
            AddTargetBasket(b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, -3.0, 3.0), -1.2f), 0.0f);

            // Bottom Left  Floor
            AddStaticObject(b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, -13.0f, -11.0f), getExtremeCases(m_sStaticObjectOrientationType, 5.0f, 7.0f)),
                0, SimulationObject::STATIC_PLATFORM);

            // Bottom Right Floor
            AddStaticObject(b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, 11.0f, 13.0f), getExtremeCases(m_sStaticObjectOrientationType, 5.0f, 7.0f)),
                0, SimulationObject::STATIC_PLATFORM);


            // Bottom Left  Vertical Floor
            AddStaticObject(b2Vec2(-18.0f, getExtremeCases(m_sStaticObjectOrientationType, 15.0f, 17.0f)),
                4 * M_PI / 5, SimulationObject::STATIC_PLATFORM);

   

            AddStaticObject(b2Vec2(-4.0f, 25.0f), 0, SimulationObject::STATIC_BALL);



            if (m_bIncludeDynamicObjects) {
                // Stationary Bottom-Left
                float32 leftBottomCircleCoorX = (getExtremeCases(m_sStaticObjectOrientationType, -10.0, -7.0));
                AddRandomDynamicObject(
                    b2Vec2(leftBottomCircleCoorX, 9.0f),
                    b2Vec2(0.0f, 0.0f)
                );

                // Stationary Bottom-Right
                float32 rightBottomCircleCoorX = (getExtremeCases(m_sStaticObjectOrientationType, 10.0, 15.0));
                AddRandomDynamicObject(
                    b2Vec2(rightBottomCircleCoorX, 9.0f),
                    b2Vec2(0.0f, 0.0f)
                );

                // Moving on the Right Floor
                AddRandomDynamicObject(
                    b2Vec2(22.0f, 17.0f),
                    b2Vec2(-10.0f, -4.0f)
                );

                // Falling Circle from mid
                AddRandomDynamicObject(
                    b2Vec2(-10.0f, (getExtremeCases(m_sStaticObjectOrientationType, 35.0f, 45.0f))),
                    b2Vec2(3.0f, -8.0f)
                );

                // Falling Circle from right
                AddRandomDynamicObject(
                    b2Vec2(16.0f, 30.0f),
                    b2Vec2(-21.0f, -5.0f)
                );
            }

   
        }
            


    private:

    };
}
