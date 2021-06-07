//
//  JSONHelper.h
//  Testbed
//
//  Created by Tayfun Ateş on 20.10.2019.
//

#ifndef JSONHelper_h
#define JSONHelper_h

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace JSONHelper {

	static bool saveJSON(const json& j, int indent, const std::string& filePath)
	{
		std::ofstream o(filePath);
		if (o.is_open()) {
			o << j.dump(indent);
			return true;
		}
		return false;
	}

	static bool saveJSON(const json& j, const std::string& filePath)
	{
		std::ofstream o(filePath);
		if (o.is_open()) {
			o << j;
			return true;
		}
		return false;
	}


	static bool loadJSON(json& j, const std::string& filePath)
	{
		std::ifstream i(filePath);
		if (i.is_open()) {
			i >> j;
			return true;
		}
		return false;
	}

}

#endif /* JSONHelper_h */
