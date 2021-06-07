#pragma once

#include "ObstructionDemoSettings.h"
#include "SimulationBase.h"
#include "Box2D/Extension/b2VisBody.hpp"
#include "Box2D/Extension/b2VisPolygonShape.hpp"
#include <math.h>

namespace svqa {
    class ObstructionDemoSimulation : public SimulationBase
    {
    public:
        typedef std::shared_ptr<ObstructionDemoSimulation> Ptr;
        
        ObstructionDemoSimulation(ObstructionDemoSettings::Ptr _settings_) : SimulationBase(_settings_)
        {
            
            m_nNumberOfObjects = _settings_->numberOfObjects;
            m_nNumberOfObstacles = _settings_->numberOfObstacles;
            
            m_fSpeed = RandomFloat(75.0f, 100.0f);
            
            m_vObstaclePosition = b2Vec2(RandomFloat(-10.0f, 10.0f), RandomFloat(15.0f, 40.0f));
            m_vMovingObjPosition = b2Vec2(RandomFloat(-25.0f, -10.0), 25.0f);
            
            SET_FILE_OUTPUT_TRUE(m_pSettings->outputVideoPath)
        }
        
        virtual SimulationID getIdentifier() override
        {
            return SimulationID::ID_ObstructionDemo;
        }
        
        void InitializeScene() override {
            AddSimulationObject(m_vMovingObjPosition, b2Vec2(m_fSpeed, -30.0f), SimulationObject::CIRCLE, SimulationObject::BROWN, SimulationObject::SMALL);
            
            AddSimulationObject(b2Vec2(RandomFloat(-30.0f, -10.0f), 15.0f), b2Vec2_zero, SimulationObject::CUBE, SimulationObject::RED, SimulationObject::LARGE);
            
            AddSimulationObject(b2Vec2(RandomFloat(-10.0f, 10.0f), 15.0f), b2Vec2_zero, SimulationObject::CIRCLE, SimulationObject::PURPLE, SimulationObject::SMALL);
            AddSimulationObject(b2Vec2(RandomFloat(-10.0f, 10.0f), 15.0f), b2Vec2_zero, SimulationObject::CIRCLE, SimulationObject::GREEN, SimulationObject::SMALL);
            AddSimulationObject(b2Vec2(RandomFloat(-10.0f, 10.0f), 15.0f), b2Vec2_zero, SimulationObject::CIRCLE, SimulationObject::GRAY, SimulationObject::SMALL);
        }
        
    private:
        bool m_bObstaclesCreated;
        int m_nNumberOfObjects;
        int m_nNumberOfObstacles;
        float32 m_fSpeed;
        b2Vec2 m_vMovingObjPosition;
        b2Vec2 m_vObstaclePosition;
    };
}
