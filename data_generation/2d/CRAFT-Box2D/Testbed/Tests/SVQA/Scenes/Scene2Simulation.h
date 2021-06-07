#pragma once

#include "Scene2Settings.h"
#include "SimulationBase.h"
#include "Box2D/Extension/b2VisBody.hpp"
#include "Box2D/Extension/b2VisPolygonShape.hpp"
#include <math.h> 

namespace svqa {
    class Scene2Simulation : public SimulationBase
    {
    public:
        typedef std::shared_ptr<Scene2Simulation> Ptr;
        Scene2Simulation(Scene2Settings::Ptr settings) : SimulationBase(settings)
        {

        }
        
        virtual SimulationID getIdentifier() override
        {
            return SimulationID::ID_Scene2;
        }
        
        void InitializeScene() override {
            // Basket
            AddTargetBasket(b2Vec2(getExtremeCases(m_sStaticObjectOrientationType, 1.0, 7.0), -1.2f), 0.0f);
            
            // ------- Top Right Platform and Circle -------
            AddStaticObject(b2Vec2(17.0f, 19.0f), 0, SimulationObject::STATIC_PLATFORM);
            AddStaticObject(b2Vec2(9.0f, 19.0f), 0, SimulationObject::STATIC_PLATFORM);
            AddStaticObject(b2Vec2(11.0f, 19.0f), 0, SimulationObject::STATIC_RAMP);
            
            // ------- Inclined Platform and Rolling Circle -------
            AddStaticObject(b2Vec2(-26.5f, 12.0f), 6 * M_PI / 7, SimulationObject::STATIC_PLATFORM);
            
            // ------- Bottom Left Platform and Circle -------
            AddStaticObject(b2Vec2(-7.5f, getExtremeCases(m_sStaticObjectOrientationType, 5, 7)), 0, SimulationObject::STATIC_PLATFORM);
            // ------- Bottom Right Platform and Circle -------
            AddStaticObject(b2Vec2(17.0f, getExtremeCases(m_sStaticObjectOrientationType, 5, 7)), 0, SimulationObject::STATIC_PLATFORM);
            
            
            
            std::vector<SimulationObject::Color> distinc_colors;
            while(distinc_colors.size() < 4) {
                SimulationObject::Color c = SimulationObject::getRandomColor();
                if (std::find(distinc_colors.begin(), distinc_colors.end(), c) == distinc_colors.end()) {
                    distinc_colors.push_back(c);
                }
            }
            
            
            // ------- Stack --------
            std::vector<SimulationObject::Color> stack_colors;
            while (stack_colors.size() < 6) {
                SimulationObject::Color c = SimulationObject::getRandomColor();
                if (std::find(stack_colors.begin(), stack_colors.end(), c) == stack_colors.end()) {
                    stack_colors.push_back(c);
                }
            }
            
            if (m_bIncludeDynamicObjects) {
                
                // bottom left - mid - right
                AddDynamicObject(b2Vec2(3.0f, 20.0f),b2Vec2(0.0f, 0.0f),SimulationObject::Shape::CUBE,
                                 stack_colors[0],SimulationObject::Size::SMALL);
                AddDynamicObject(b2Vec2(5.2f, 20.0f),b2Vec2(0.0f, 0.0f),SimulationObject::Shape::CUBE,
                                 stack_colors[1],SimulationObject::Size::SMALL);
                AddDynamicObject(b2Vec2(7.4f, 20.0f),b2Vec2(0.0f, 0.0f),SimulationObject::Shape::CUBE,
                                 stack_colors[2],SimulationObject::Size::SMALL);
                
                // middle left - right
                AddDynamicObject(b2Vec2(4.0f, 22.0f), b2Vec2(0.0f, 0.0f), SimulationObject::Shape::CUBE, stack_colors[3],
                                 SimulationObject::Size::SMALL);
                AddDynamicObject(b2Vec2(6.2f, 22.0f), b2Vec2(0.0f, 0.0f), SimulationObject::Shape::CUBE, stack_colors[4],
                                 SimulationObject::Size::SMALL);
                // top
                AddDynamicObject(b2Vec2(5.0f, 24.0f), b2Vec2(0.0f, 0.0f), SimulationObject::Shape::CUBE, stack_colors[5],
                                 SimulationObject::Size::SMALL);
                
                float32 tr_circle_velocities[5] = { -24.0f, -28.0f, -25.0f, -18.0f, -19.0f };
                
                AddDynamicObject(
                                 b2Vec2(22.0f, 20.0f),
                                 b2Vec2(tr_circle_velocities[(int)RandomFloatFromHardware(0.0, 5.0)], 0.0f),
                                 SimulationObject::Shape::CIRCLE,
                                 distinc_colors[0],
                                 SimulationObject::getRandomSize()
                                 );
                
                AddDynamicObject(
                                 b2Vec2(-22.0f, RandomFloatFromHardware(19.0, 35.0)),
                                 b2Vec2(0.0f, RandomFloatFromHardware(-9.0, 0.0)),
                                 SimulationObject::Shape::CIRCLE,
                                 distinc_colors[1],
                                 SimulationObject::getRandomSize()
                                 );
                
                AddDynamicObject(
                                 b2Vec2(RandomFloatFromHardware(-12.0, -4.0), 8.0f),
                                 b2Vec2(0.0f, 0.0f),
                                 SimulationObject::Shape::CIRCLE,
                                 distinc_colors[2],
                                 SimulationObject::getRandomSize()
                                 );
                
                AddDynamicObject(
                                 b2Vec2(22.0f, 8.0f),
                                 b2Vec2(RandomFloatFromHardware(-29.0, -20.0), 0.0f),
                                 SimulationObject::Shape::CIRCLE,
                                 distinc_colors[3],
                                 SimulationObject::getRandomSize()
                                 );
                
            }
            
        }
        
    private:

    };
}
