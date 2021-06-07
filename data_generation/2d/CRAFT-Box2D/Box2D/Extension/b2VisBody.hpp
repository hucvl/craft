//
//  b2VisBody.hpp
//  Box2D
//
//  Created by Tayfun Ateş on 5.10.2019.
//

#ifndef b2VisBody_hpp
#define b2VisBody_hpp

#include "Box2D/Dynamics/b2Body.h"
#include "Box2D/Common/b2Draw.h"
#include "Box2D/Dynamics/b2World.h"
#include "Box2D/Extension/b2VisTexture.hpp"

class b2VisBody : public b2Body
{
public:
    
    b2VisBody(const b2BodyDef* bd, b2World* world);
    virtual ~b2VisBody();
    
    //getters and setters
    b2Color getColor() const;
    void setColor(const b2Color& col);
    
    b2VisTexture::Ptr getTexture() const;
    void setTexture(b2VisTexture::Ptr tex);
    bool hasAttachedTexture() const;
    
    // Checks whether the body has a sensor fixture attached to it.
    bool hasSensorFixture() const;

    //Gets unique id associated with the body
    int getUniqueId();
    
    //Sets unique id associated with the body
    void setUniqueId(const int& id);
    
private:
    int m_nUniqueId;
    b2Color m_Color;
    b2VisTexture::Ptr m_pTexture;
};


#endif /* b2VisBody_hpp */
