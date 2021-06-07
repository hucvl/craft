#pragma once

#include "Scene15Settings.h"
#include "SimulationBase.h"
#include "Box2D/Extension/b2VisBody.hpp"
#include "Box2D/Extension/b2VisPolygonShape.hpp"
#include <math.h> 

namespace svqa {
    class Scene15Simulation : public SimulationBase
    {
    public:
        typedef std::shared_ptr<Scene15Simulation> Ptr;
        Scene15Simulation(Scene15Settings::Ptr settings) : SimulationBase(settings)
        {

        }
        
        virtual SimulationID getIdentifier() override
        {
            return SimulationID::ID_Scene15;
        }
        
        void InitializeScene() override {
            // Basket   
            float basket_pos_y = 10.0f;
            float basket_pos_x = getExtremeCases(m_sStaticObjectOrientationType, 8.0f, 16.0f);
            AddTargetBasket(b2Vec2(basket_pos_x, basket_pos_y), 0.0f);

            // Right basket platform
            float right_platform_pos_x = basket_pos_x + 0.0f;
            float right_platform_pos_y = basket_pos_y - 4.12f;
            AddStaticObject(b2Vec2(right_platform_pos_x, right_platform_pos_y), 0, SimulationObject::STATIC_PLATFORM);

            // Right inclined-platform     
            float right_incl_platform_angle = M_PI / 6.0f;
            float right_incl_platform_pos_x = 18.0f;
            float right_incl_platform_pos_y = getExtremeCases(m_sStaticObjectOrientationType, 25.0f, 29.0f);
            AddStaticObject(b2Vec2(right_incl_platform_pos_x, right_incl_platform_pos_y), 
                right_incl_platform_angle,
                SimulationObject::STATIC_PLATFORM);

            // Platforms that are composed of two or more parts can be implemented like this.
            // Right lower inclined platform
            float left_lower_platform_angle = M_PI / 6.0f;
            // Right lower inclined platform part 1
            float left_lower_platform_pos_x = -8.0f;
            float left_lower_platform_pos_y = 12.0f;
            AddStaticObject(b2Vec2(left_lower_platform_pos_x, left_lower_platform_pos_y), 
                left_lower_platform_angle, 
                SimulationObject::STATIC_PLATFORM);
            // Right lower inclined platform part 2
            float left_lower_platform_2_pos_x = left_lower_platform_pos_x - cos(left_lower_platform_angle) * 8.0 * 2;
            float left_lower_platform_2_pos_y = left_lower_platform_pos_y - sin(left_lower_platform_angle) * 8.0 * 2;
            AddStaticObject(b2Vec2(left_lower_platform_2_pos_x, left_lower_platform_2_pos_y),
                left_lower_platform_angle, 
                SimulationObject::STATIC_PLATFORM);

            // Upper platform
            float upper_platform_pos_x = -12.0f;
            float upper_platform_pos_y = 36.0f;
            AddStaticObject(b2Vec2(upper_platform_pos_x, upper_platform_pos_y), - M_PI / getExtremeCases(m_sStaticObjectOrientationType, 3.0f, 5.0f), SimulationObject::STATIC_PLATFORM);

            if (m_bIncludeDynamicObjects) 
            {
                AddRandomDynamicObject(
                    b2Vec2(b2Vec2(upper_platform_pos_x, upper_platform_pos_y + 4)),
                    b2Vec2(0.0f, 0.0f)
                );
                AddRandomDynamicObject(
                    b2Vec2(b2Vec2(right_incl_platform_pos_x, right_incl_platform_pos_y + 5)),
                    b2Vec2(0.0f, 0.0f)
                );

                float v = getExtremeCases(m_sStaticObjectOrientationType, 15.0f, 23.0f);
                AddRandomDynamicObject(
                    b2Vec2(b2Vec2(left_lower_platform_pos_x - 2.0f , left_lower_platform_pos_y + 0.5f)),
                    b2Vec2(v, v)
                );
            }
        }
            


    private:

    };
}
