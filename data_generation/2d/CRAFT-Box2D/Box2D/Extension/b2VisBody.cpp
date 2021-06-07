//
//  b2VisBody.cpp
//  Box2D
//
//  Created by Tayfun Ateş on 5.10.2019.
//

#include "b2VisBody.hpp"
#include "Box2D/Box2D.h"

b2VisBody::b2VisBody(const b2BodyDef* bd, b2World* world) : b2Body(bd, world)
{
    m_nUniqueId = -1;
    setColor(b2Color(1.0f, 1.0f, 1.0f, 1.0f));
}

b2VisBody::~b2VisBody()
{
    
}

b2Color b2VisBody::getColor() const
{
    return m_Color;
}

void b2VisBody::setColor(const b2Color& col)
{
    m_Color = col;
}

b2VisTexture::Ptr b2VisBody::getTexture() const
{
    return m_pTexture;
}

void b2VisBody::setTexture(b2VisTexture::Ptr tex)
{
    m_pTexture = tex;
}

bool b2VisBody::hasAttachedTexture() const
{
    return m_pTexture.get() != nullptr && m_pTexture->getTextureId()>0;
}

void b2VisBody::setUniqueId(const int& id)
{
    m_nUniqueId = id;
}

bool b2VisBody::hasSensorFixture() const
{
    for (const b2Fixture* f = b2Body::GetFixtureList(); f; f = f->GetNext())
    {
        if (f->IsSensor()) return true;
    }
    return false;
}

int b2VisBody::getUniqueId()
{
    if(m_nUniqueId>=0) {
        return m_nUniqueId;
    }
    int index = 0;
    b2VisBody* list = (b2VisBody*) m_world->GetBodyList();
    for (b2VisBody* b = (b2VisBody*)list; b; b = (b2VisBody*)b->GetNext()) {
        bool isSensor = b->hasSensorFixture();
        if (b == this && !isSensor) {
            setUniqueId(index);
            return index;
        }
        else if (!isSensor) {
            index++;
        }
    }
    return -1;
}


