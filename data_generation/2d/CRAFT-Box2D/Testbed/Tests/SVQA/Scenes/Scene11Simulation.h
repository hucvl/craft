#pragma once

#include "Scene11Settings.h"
#include "SimulationBase.h"
#include "Box2D/Extension/b2VisBody.hpp"
#include "Box2D/Extension/b2VisPolygonShape.hpp"
#include <math.h> 

namespace svqa {
    class Scene11Simulation : public SimulationBase
    {
    public:
        typedef std::shared_ptr<Scene11Simulation> Ptr;
        Scene11Simulation(Scene11Settings::Ptr settings) : SimulationBase(settings)
        {

        }
        
        virtual SimulationID getIdentifier() override
        {
            return SimulationID::ID_Scene11;
        }
        
        void InitializeScene() override {

            // Basket
            float basket_pos_x = getExtremeCases(m_sStaticObjectOrientationType, -18.0f, -21.0f);
            AddTargetBasket(b2Vec2(basket_pos_x, -1.2f), 0.0f);
            
            // Right platform
            float right_platform_pos_x = getExtremeCases(m_sStaticObjectOrientationType, 12.5f, 15.0f);
            AddStaticObject(b2Vec2(right_platform_pos_x, 5.0f), 0, SimulationObject::STATIC_PLATFORM);

            // Left platform
            float left_platform_pos_x =  getExtremeCases(m_sStaticObjectOrientationType, -12.5f, -11.0f);
            AddStaticObject(b2Vec2(left_platform_pos_x, 5.0f), 0, SimulationObject::STATIC_PLATFORM);

            // Static ball
            float static_ball_pos_y = getExtremeCases(m_sStaticObjectOrientationType, 2.5f, 4.5f);
            AddStaticObject(b2Vec2(1.0f, static_ball_pos_y), 0, SimulationObject::STATIC_BALL);


            // Right inclined-platform     
            float x = getExtremeCases(m_sStaticObjectOrientationType, 4.5f, 5.5f);
            AddStaticObject(b2Vec2(right_platform_pos_x + 1.0f, 32.0f), (M_PI / x) , SimulationObject::STATIC_PLATFORM);

            // Left inclined-platform
            float left_platform_pos_y = getExtremeCases(m_sStaticObjectOrientationType, 15.5f, 21.5f);
            AddStaticObject(b2Vec2(left_platform_pos_x + 5.0f, left_platform_pos_y), M_PI / 5.0f , SimulationObject::STATIC_PLATFORM);



            if (m_bIncludeDynamicObjects) {

                // Circle on the Right platform
                AddRandomDynamicObject(
                    b2Vec2(right_platform_pos_x + 2.0f, 6.5f),
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, -15.5f, -21.5f), 0.0f)
                );

                // Circle on the Left platform
                AddRandomDynamicObject(
                    b2Vec2(left_platform_pos_x - 2.0f, 6.5f),
                    b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, 15.5f, 21.5f), 0.0f)
                );

                // Circle on the Right inclined-platform
                AddRandomDynamicObject(
                    b2Vec2(right_platform_pos_x + 2.0f, 38.5f),
                    b2Vec2(-M_PI, -x)
                );
            }
        }
            


    private:

    };
}
