//
//  b2VisPolygonShape.cpp
//  Box2D
//
//  Created by Tayfun Ateş on 9.10.2019.
//

#include "b2VisPolygonShape.hpp"
#include "b2VisDefines.h"

b2VisPolygonShape::b2VisPolygonShape()
{
    
}

b2VisPolygonShape::~b2VisPolygonShape()
{
    
}

std::vector<b2Vec2> b2VisPolygonShape::getTextureCoords() const
{
    std::vector<b2Vec2> res(m_count);
    for (int32 i = 0; i < m_count; ++i)
    {
        res[i] = b2Vec2(m_vertices[i].x / TEXTURE_SQUARE_EDGE_LENGTH, m_vertices[i].y / TEXTURE_SQUARE_EDGE_LENGTH);
    }
    return res;
}
