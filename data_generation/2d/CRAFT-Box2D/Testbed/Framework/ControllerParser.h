//
//  ControllerParser.h
//  Testbed
//
//  Created by Tayfun Ateş on 17.11.2019.
//

#ifndef ControllerParser_h
#define ControllerParser_h

#include "ObstructionDemoSimulation.h"
#include "Scene1Simulation.h"
#include "Scene2Simulation.h"
#include "Scene3Simulation.h"
#include "Scene4Simulation.h"
#include "Scene5Simulation.h"
#include "Scene6Simulation.h"
#include "Scene7Simulation.h"
#include "Scene8Simulation.h"
#include "Scene9Simulation.h"
#include "Scene10Simulation.h"
#include "Scene11Simulation.h"
#include "Scene12Simulation.h"
#include "Scene13Simulation.h"
#include "Scene14Simulation.h"
#include "Scene15Simulation.h"
#include "Scene16Simulation.h"
#include "Scene17Simulation.h"
#include "Scene18Simulation.h"
#include "Scene19Simulation.h"
#include "Scene20Simulation.h"
#include "JSONHelper.h"

namespace svqa {
    SimulationBase::Ptr parse(const std::string& inputFile)
    {
        Settings set;
        json j;
        bool fileLoadRes = JSONHelper::loadJSON(j, inputFile);
        if(fileLoadRes) {
            set.from_json(j);
            if (set.simulationID == SimulationID::ID_ObstructionDemo) {
                ObstructionDemoSettings::Ptr setPtr = std::make_shared<ObstructionDemoSettings>();
                setPtr->from_json(j);
                return std::make_shared<ObstructionDemoSimulation>(setPtr);
            }
            else if (set.simulationID == SimulationID::ID_Scene1) {
                Scene1Settings::Ptr setPtr = std::make_shared<Scene1Settings>();
                setPtr->from_json(j);
                return std::make_shared<Scene1Simulation>(setPtr);
            }
            else if (set.simulationID == SimulationID::ID_Scene2) {
                Scene2Settings::Ptr setPtr = std::make_shared<Scene2Settings>();
                setPtr->from_json(j);
                return std::make_shared<Scene2Simulation>(setPtr);
            }
            else if (set.simulationID == SimulationID::ID_Scene3) {
                Scene3Settings::Ptr setPtr = std::make_shared<Scene3Settings>();
                setPtr->from_json(j);
                return std::make_shared<Scene3Simulation>(setPtr);
            }
            else if (set.simulationID == SimulationID::ID_Scene4) {
                Scene4Settings::Ptr setPtr = std::make_shared<Scene4Settings>();
                setPtr->from_json(j);
                return std::make_shared<Scene4Simulation>(setPtr);
            }
            else if (set.simulationID == SimulationID::ID_Scene5) {
                Scene5Settings::Ptr setPtr = std::make_shared<Scene5Settings>();
                setPtr->from_json(j);
                return std::make_shared<Scene5Simulation>(setPtr);
            }
            else if (set.simulationID == SimulationID::ID_Scene6) {
                Scene6Settings::Ptr setPtr = std::make_shared<Scene6Settings>();
                setPtr->from_json(j);
                return std::make_shared<Scene6Simulation>(setPtr);
            }
            else if (set.simulationID == SimulationID::ID_Scene7) {
                Scene7Settings::Ptr setPtr = std::make_shared<Scene7Settings>();
                setPtr->from_json(j);
                return std::make_shared<Scene7Simulation>(setPtr);
            }
            else if (set.simulationID == SimulationID::ID_Scene8) {
                Scene8Settings::Ptr setPtr = std::make_shared<Scene8Settings>();
                setPtr->from_json(j);
                return std::make_shared<Scene8Simulation>(setPtr);
            }
            else if (set.simulationID == SimulationID::ID_Scene9) {
                Scene9Settings::Ptr setPtr = std::make_shared<Scene9Settings>();
                setPtr->from_json(j);
                return std::make_shared<Scene9Simulation>(setPtr);
            }
            else if (set.simulationID == SimulationID::ID_Scene10) {
                Scene10Settings::Ptr setPtr = std::make_shared<Scene10Settings>();
                setPtr->from_json(j);
                return std::make_shared<Scene10Simulation>(setPtr);
            }
            else if (set.simulationID == SimulationID::ID_Scene11) {
                Scene11Settings::Ptr setPtr = std::make_shared<Scene11Settings>();
                setPtr->from_json(j);
                return std::make_shared<Scene11Simulation>(setPtr);
            }
            else if (set.simulationID == SimulationID::ID_Scene12) {
                Scene12Settings::Ptr setPtr = std::make_shared<Scene12Settings>();
                setPtr->from_json(j);
                return std::make_shared<Scene12Simulation>(setPtr);
            }
            else if (set.simulationID == SimulationID::ID_Scene13) {
                Scene13Settings::Ptr setPtr = std::make_shared<Scene13Settings>();
                setPtr->from_json(j);
                return std::make_shared<Scene13Simulation>(setPtr);
            }
            else if (set.simulationID == SimulationID::ID_Scene14) {
                Scene14Settings::Ptr setPtr = std::make_shared<Scene14Settings>();
                setPtr->from_json(j);
                return std::make_shared<Scene14Simulation>(setPtr);
            }
            else if (set.simulationID == SimulationID::ID_Scene15) {
                Scene15Settings::Ptr setPtr = std::make_shared<Scene15Settings>();
                setPtr->from_json(j);
                return std::make_shared<Scene15Simulation>(setPtr);
            }
            else if (set.simulationID == SimulationID::ID_Scene16) {
                Scene16Settings::Ptr setPtr = std::make_shared<Scene16Settings>();
                setPtr->from_json(j);
                return std::make_shared<Scene16Simulation>(setPtr);
            }
            else if (set.simulationID == SimulationID::ID_Scene17) {
                Scene17Settings::Ptr setPtr = std::make_shared<Scene17Settings>();
                setPtr->from_json(j);
                return std::make_shared<Scene17Simulation>(setPtr);
            }
            else if (set.simulationID == SimulationID::ID_Scene18) {
                Scene18Settings::Ptr setPtr = std::make_shared<Scene18Settings>();
                setPtr->from_json(j);
                return std::make_shared<Scene18Simulation>(setPtr);
            }
            else if (set.simulationID == SimulationID::ID_Scene19) {
                Scene19Settings::Ptr setPtr = std::make_shared<Scene19Settings>();
                setPtr->from_json(j);
                return std::make_shared<Scene19Simulation>(setPtr);
            }
            else if (set.simulationID == SimulationID::ID_Scene20) {
                Scene20Settings::Ptr setPtr = std::make_shared<Scene20Settings>();
                setPtr->from_json(j);
                return std::make_shared<Scene20Simulation>(setPtr);
            }
        }
        return nullptr;
        
    }
}

#endif /* ControllerParser_h */
