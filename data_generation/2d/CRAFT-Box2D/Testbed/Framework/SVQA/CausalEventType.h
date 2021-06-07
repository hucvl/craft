//
//  CausalEventType.h
//  Testbed
//
//  Created by Tayfun Ateş on 28.01.2020.
//

#ifndef CausalEventType_h
#define CausalEventType_h

#include <string>
#include <nlohmann/json.hpp>

enum CausalEventType {
	Start_Event = 0,
	End_Event = 1,
	Collision_Event = 2,
	StartTouching_Event = 3,
	EndTouching_Event = 4,
	ContainerEndUp_Event = 5,
};

static std::string getTypeString(CausalEventType type) {
	switch (type) {
	case CausalEventType::Start_Event:
		return "Start";
	case CausalEventType::End_Event:
		return "End";
	case CausalEventType::Collision_Event:
		return "Collision";
	case CausalEventType::StartTouching_Event:
		return "StartTouching";
	case CausalEventType::EndTouching_Event:
		return "EndTouching";
	case CausalEventType::ContainerEndUp_Event:
		return "ContainerEndUp";
	}
	return "";
}

NLOHMANN_JSON_SERIALIZE_ENUM(CausalEventType, {
	{Start_Event, getTypeString(Start_Event)},
	{End_Event, getTypeString(End_Event)},
	{Collision_Event, getTypeString(Collision_Event)},
	{StartTouching_Event, getTypeString(StartTouching_Event)},
	{EndTouching_Event, getTypeString(EndTouching_Event)},
	{ContainerEndUp_Event, getTypeString(ContainerEndUp_Event)},
	})

#endif /* CausalEventType_h */
