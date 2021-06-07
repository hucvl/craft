#pragma once

#include "Scene13Settings.h"
#include "SimulationBase.h"
#include "Box2D/Extension/b2VisBody.hpp"
#include "Box2D/Extension/b2VisPolygonShape.hpp"
#include <math.h> 

namespace svqa {
    class Scene13Simulation : public SimulationBase
    {
    public:
        typedef std::shared_ptr<Scene13Simulation> Ptr;
        Scene13Simulation(Scene13Settings::Ptr settings) : SimulationBase(settings)
        {

        }
        
        virtual SimulationID getIdentifier() override
        {
            return SimulationID::ID_Scene13;
        }
        
        void InitializeScene() override {
            // Basket   
            float basket_pos_x = getExtremeCases(m_sStaticObjectOrientationType, -21, -15);
            AddTargetBasket(b2Vec2(basket_pos_x, -1.2f), 0.0f); 

            // Static ball right
            float static_ball_1_pos_x = getExtremeCases(m_sStaticObjectOrientationType, -7, -5);
            float static_ball_1_pos_y = getExtremeCases(m_sStaticObjectOrientationType, 5, 10);
            AddStaticObject(b2Vec2(static_ball_1_pos_x, static_ball_1_pos_y), 0, SimulationObject::STATIC_BALL);

            // Static ball middle
            float static_ball_2_pos_x = getExtremeCases(m_sStaticObjectOrientationType, -13, -11);
            float static_ball_2_pos_y = getExtremeCases(m_sStaticObjectOrientationType, 15, 20);
            AddStaticObject(b2Vec2(static_ball_2_pos_x, static_ball_2_pos_y), 0, SimulationObject::STATIC_BALL);

            // Static ball left
            float static_ball_3_pos_x = getExtremeCases(m_sStaticObjectOrientationType, -19, -17);
            float static_ball_3_pos_y = getExtremeCases(m_sStaticObjectOrientationType, 25, 30);
            AddStaticObject(b2Vec2(static_ball_3_pos_x, static_ball_3_pos_y), 0, SimulationObject::STATIC_BALL);

            // Platforms that are composed of two or more parts can be implemented like this.
            // Right upper inclined platform
            float right_upper_platform_angle = getExtremeCases(m_sStaticObjectOrientationType, M_PI / 7.0f, M_PI / 5.0f);
            // Right upper inclined platform part 1
            float right_upper_platform_pos_x = getExtremeCases(m_sStaticObjectOrientationType, 14.5, 19);
            float right_upper_platform_pos_y = getExtremeCases(m_sStaticObjectOrientationType, 34, 38);
            AddStaticObject(b2Vec2(right_upper_platform_pos_x, right_upper_platform_pos_y),
                right_upper_platform_angle,
                SimulationObject::STATIC_PLATFORM);
            // Right upper inclined platform part 2
            float right_upper_platform_length = getExtremeCases(m_sStaticObjectOrientationType, 1.5, 2);
            float right_upper_platform_2_pos_x = right_upper_platform_pos_x - cos(right_upper_platform_angle) * 8.0 * right_upper_platform_length;
            float right_upper_platform_2_pos_y = right_upper_platform_pos_y - sin(right_upper_platform_angle) * 8.0 * right_upper_platform_length;
            AddStaticObject(b2Vec2(right_upper_platform_2_pos_x, right_upper_platform_2_pos_y), 
                right_upper_platform_angle, 
                SimulationObject::STATIC_PLATFORM);

            // Platforms that are composed of two or more parts can be implemented like this.
            // Right lower inclined platform
            float right_lower_platform_angle = getExtremeCases(m_sStaticObjectOrientationType, M_PI / 7.0f, M_PI / 5.0f);
            // Right lower inclined platform part 1
            float right_lower_platform_pos_x = getExtremeCases(m_sStaticObjectOrientationType, 14, 22);
            float right_lower_platform_pos_y = getExtremeCases(m_sStaticObjectOrientationType, 22, 26);
            AddStaticObject(b2Vec2(right_lower_platform_pos_x, right_lower_platform_pos_y), 
                right_lower_platform_angle,
                SimulationObject::STATIC_PLATFORM);
            // Right lower inclined platform part 2
            float right_lower_platform_length = getExtremeCases(m_sStaticObjectOrientationType, 1.5, 2);
            float right_lower_platform_2_pos_x = right_lower_platform_pos_x - cos(right_lower_platform_angle) * 8.0 * right_lower_platform_length;
            float right_lower_platform_2_pos_y = right_lower_platform_pos_y - sin(right_lower_platform_angle) * 8.0 * right_lower_platform_length;
            AddStaticObject(b2Vec2(right_lower_platform_2_pos_x, right_lower_platform_2_pos_y), 
                right_lower_platform_angle,
                SimulationObject::STATIC_PLATFORM);


            if (m_bIncludeDynamicObjects) {

                AddRandomDynamicObject(
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, static_ball_2_pos_x - 8, static_ball_2_pos_x + 8), getExtremeCases(m_sStaticObjectOrientationType, 35,48)),
                    b2Vec2(0,0)
                );

                AddRandomDynamicObject(
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, static_ball_2_pos_x - 8, static_ball_2_pos_x + 8), getExtremeCases(m_sStaticObjectOrientationType, 35, 48)),
                    b2Vec2(0, 0)
                );


                float right_lower_platform_object_offset = getExtremeCases(m_sStaticObjectOrientationType, -1, 1);
                float right_lower_platform_object_speed = getExtremeCases(m_sStaticObjectOrientationType, -2, 0);
                float right_lower_platform_object_velocity_x = right_lower_platform_object_speed * cos(right_lower_platform_angle);
                float right_lower_platform_object_velocity_y = right_lower_platform_object_speed * sin(right_lower_platform_angle);
                AddRandomDynamicObject(
                    b2Vec2(b2Vec2(right_lower_platform_pos_x + right_lower_platform_object_offset, right_lower_platform_pos_y + 1)),
                    b2Vec2(right_lower_platform_object_velocity_x, right_lower_platform_object_velocity_y)
                );
                float right_upper_platform_object_offset = getExtremeCases(m_sStaticObjectOrientationType, -1, 1);
                float right_upper_platform_object_speed = getExtremeCases(m_sStaticObjectOrientationType, -2, 0);
                float right_upper_platform_object_velocity_x = right_upper_platform_object_speed * cos(right_upper_platform_angle);
                float right_upper_platform_object_velocity_y = right_upper_platform_object_speed * sin(right_upper_platform_angle);
                AddRandomDynamicObject(
                    b2Vec2(b2Vec2(right_upper_platform_pos_x + right_upper_platform_object_offset, right_upper_platform_pos_y + 1)),
                    b2Vec2(right_upper_platform_object_velocity_x, right_upper_platform_object_velocity_y)
                );
            }
        }
            


    private:

    };
}
