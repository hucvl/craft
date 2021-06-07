//
//  CausalEdge.cpp
//  Testbed
//
//  Created by Tayfun Ateş on 28.01.2020.
//

#include "CausalEvent.hpp"

namespace svqa
{
    int CausalEvent::getStepCount() const
    {
        return m_nStepCount;
    }

    std::string CausalEvent::getStrRepresentation()
    {
        std::string eventType = getTypeStr() + ":" + std::to_string(m_nStepCount);  
        std::string imgPaths[2];

        auto objects = getObjects();
        int i = 0;
        for(auto& obj : objects) {
            ObjectState* objState = (ObjectState*) obj->GetUserData();
            std::string imgPath = objState->getImagePath();
            imgPaths[i++] = imgPath;
        }
        std::string resultStr = "";

        if (getTypeStr() == "Start") {
             resultStr += "<<TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\"><TR><TD ALIGN = \"center\" COLSPAN = \"2\"><FONT POINT-SIZE = \"24.0\" FACE = \"ambrosia\">" + eventType + "</FONT></TD></TR></TABLE>>";
        }
        else if (getTypeStr() == "End") {
            resultStr += "<<TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\"><TR><TD ALIGN = \"center\" COLSPAN = \"2\"><FONT POINT-SIZE = \"24.0\" FACE = \"ambrosia\">" + eventType + "</FONT></TD></TR></TABLE>>";
        }
        else {
            resultStr += "<<TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\"><TR><TD ALIGN = \"center\" COLSPAN = \"2\"><FONT POINT-SIZE = \"24.0\" FACE = \"ambrosia\">" + eventType + "</FONT></TD></TR><TR><TD><IMG SRC = " + imgPaths[0] + "\"/></TD><TD><IMG SRC =" + imgPaths[1] + "\"/></TD></TR></TABLE>>";
        }

        return resultStr;
    }

    json CausalEvent::toJSON() const
    {
        json jEvent;
        jEvent.emplace("id", (long long) this);
        jEvent.emplace("step", m_nStepCount);
        jEvent.emplace("type", getType());
        
        std::vector<int> objIds;
        auto objects = getObjects();
        for(auto& obj : objects) {
            objIds.push_back(obj->getUniqueId());
        }
        jEvent.emplace("objects", objIds);
        return jEvent;
    }
}
