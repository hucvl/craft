//
//  b2VisWorld.hpp
//  Box2D
//
//  Created by Tayfun Ateş on 5.10.2019.
//

#ifndef b2VisWorld_hpp
#define b2VisWorld_hpp

#include "Box2D/Dynamics/b2World.h"

class b2VisWorld : public b2World
{
public:
    b2VisWorld(const b2Vec2& gravity);
    virtual ~b2VisWorld();
    
    /// Create a rigid body given a definition. No reference to the definition
    /// is retained.
    /// @warning This function is locked during callbacks.
    virtual b2Body* CreateBody(const b2BodyDef* def) override;
    
    /// Destroy a rigid body given a definition. No reference to the definition
    /// is retained. This function is locked during callbacks.
    /// @warning This automatically deletes all associated shapes and joints.
    /// @warning This function is locked during callbacks.
    virtual void DestroyBody(b2Body* body) override;
    
    /// Call this to draw shapes and other debug draw data. This is intentionally non-const.
    virtual void DrawDebugData() override;
    
    virtual void DrawTexturedShape(b2Fixture* fixture, const b2Transform& xf, const b2Color& color, const uint32& glTextureId, const int& textureMaterialId);
    
    virtual void DrawShape(b2Fixture* shape, const b2Transform& xf, const b2Color& color) override;
    
    //Gets the renderer to outside world
    virtual b2Draw* getRenderer();

};

#endif /* b2VisWorld_hpp */
