//
//  SceneState.h
//  Testbed
//
//  Created by Tayfun Ateş on 20.10.2019.
//

#ifndef SceneState_h
#define SceneState_h

#include <iostream>
#include "SimulationDefines.h"
#include <nlohmann/json.hpp>
#include "ObjectState.h"
#include "JSONHelper.h"
#include "Box2D/Extension/b2VisWorld.hpp"

using json = nlohmann::json;

struct SceneState {

	void add(const ObjectState::Ptr& objState)
	{
		objects.push_back(objState);
	}

	void clear()
	{
		objects.clear();
	}

	bool loadFromJSON(const json& j, WORLD* toWorld, float noiseAmount, int perturbationSeed)
	{
		//FIXME: Read directions and apply transformation if they can be updated
		auto objectsJsonItr = j.find(objectsKey);
		if (objectsJsonItr != j.end())
		{
			clear();

			auto objectsJson = *objectsJsonItr;
			for (auto& [key, value] : objectsJson.items()) {
				ObjectState::Ptr oState = std::make_shared<ObjectState>();
				oState->from_json(value, toWorld, noiseAmount, perturbationSeed);
				add(oState);
			}
			return true;
		}
	}

	bool loadFromJSONFile(std::string fromFile, WORLD* toWorld, float noiseAmount, int perturbationSeed) {
		if (toWorld) {
			json j;
			bool fileLoadRes = JSONHelper::loadJSON(j, fromFile);

			if (fileLoadRes) {
				return loadFromJSON(j, toWorld, noiseAmount, perturbationSeed);
			}
		}
		return false;
	}

	json toJSON() const
	{
		json jScene;
		/*for (int i = objects.size() - 1; i >= 0; i--) {
			const auto& object = objects[i];
			json jObject;
			object->to_json(jObject);
			jScene.push_back(jObject);
		}*/
		for (int i = 0; i < objects.size(); i++) {
			const auto& object = objects[i];
			json jObject;
			object->to_json(jObject);
			jScene.push_back(jObject);
		}
		json retWrapper;
		retWrapper.emplace(objectsKey, jScene);

		std::vector<float> leftDirecions = { -1.0f, 0.0f };
		std::vector<float> rightDirecions = { 1.0f, 0.0f };
		std::vector<float> aboveDirecions = { 0.0f, 1.0f };
		std::vector<float> belowDirecions = { 0.0f, -1.0f };

		json dirJson;
		dirJson.emplace("left", leftDirecions);
		dirJson.emplace("right", rightDirecions);
		dirJson.emplace("above", aboveDirecions);
		dirJson.emplace("below", belowDirecions);
		retWrapper.emplace(directionsKey, dirJson);

		return retWrapper;
	}

	bool saveToJSONFile(WORLD* fromWorld, std::string toFile) const {
		if (fromWorld) {
			return JSONHelper::saveJSON(toJSON(), toFile);
		}
		return false;
	}

	friend std::ostream& operator<<(std::ostream& os, const SceneState& state)
	{
		os << state.toJSON();
		return os;
	}



private:
	const std::string objectsKey = "objects";
	const std::string directionsKey = "directions";
	std::vector<ObjectState::Ptr> objects;
};

#endif /* SceneState_h */
