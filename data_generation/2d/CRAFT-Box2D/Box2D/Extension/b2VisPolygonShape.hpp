//
//  b2VisPolygonShape.hpp
//  Box2D
//
//  Created by Tayfun Ateş on 9.10.2019.
//

#ifndef b2VisPolygonShape_hpp
#define b2VisPolygonShape_hpp

#include "Box2D/Collision/Shapes/b2PolygonShape.h"
#include <vector>

class b2VisPolygonShape : public b2PolygonShape
{
public:
    b2VisPolygonShape();
    virtual ~b2VisPolygonShape();
    
    std::vector<b2Vec2> getTextureCoords() const;
};


#endif /* b2VisPolygonShape_hpp */
