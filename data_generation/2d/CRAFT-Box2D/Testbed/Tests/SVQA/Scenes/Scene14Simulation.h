#pragma once

#include "Scene14Settings.h"
#include "SimulationBase.h"
#include "Box2D/Extension/b2VisBody.hpp"
#include "Box2D/Extension/b2VisPolygonShape.hpp"
#include <math.h> 

namespace svqa {
    class Scene14Simulation : public SimulationBase
    {
    public:
        typedef std::shared_ptr<Scene14Simulation> Ptr;
        Scene14Simulation(Scene14Settings::Ptr settings) : SimulationBase(settings)
        {

        }
        
        virtual SimulationID getIdentifier() override
        {
            return SimulationID::ID_Scene14;
        }
        
        void InitializeScene() override {

            // Basket
            float basket_pos_x = getExtremeCases(m_sStaticObjectOrientationType, -1.5f, 1.5f);
            AddTargetBasket(b2Vec2(basket_pos_x, -1.2f), 0.0f);


            // Left platform
            float left_platform_pos_x = -12.0f;
            AddStaticObject(b2Vec2(left_platform_pos_x, 12.0f), 0, SimulationObject::STATIC_PLATFORM);


            // Right inclined-platform     
            float right_platform_pos_x = 12.0f;
            AddStaticObject(
                b2Vec2(right_platform_pos_x - 5.0f, 30.0f),
                M_PI / getExtremeCases(m_sStaticObjectOrientationType, 4.0f, 7.0f),
                SimulationObject::STATIC_PLATFORM
            );

            // Left inclined-platform
            AddStaticObject(
                b2Vec2(left_platform_pos_x - 5.0f, 28.0f), 
                -M_PI / getExtremeCases(m_sStaticObjectOrientationType, 4.0f, 7.0f),
                SimulationObject::STATIC_PLATFORM);
            


            if (m_bIncludeDynamicObjects) {

              
                AddRandomDynamicObject(
                    b2Vec2(left_platform_pos_x - 4.0f, 14.0f),
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, 3.0f, 7.0f), 0.0f),
                    0b000,
                    SimulationObject::CIRCLE
                );

               
                AddRandomDynamicObject(
                    b2Vec2(right_platform_pos_x - 5.0f, getExtremeCases(m_sStaticObjectOrientationType, 32.0f, 34.0f)),
                    b2Vec2(0.0f, 0.0f)
                );

                
                AddRandomDynamicObject(
                    b2Vec2(left_platform_pos_x - 5.0f, getExtremeCases(m_sStaticObjectOrientationType, 36.0f, 38.0f)),
                    b2Vec2(0.0f, 0.0f)
                );
            }


        }
            


    private:

    };
}
