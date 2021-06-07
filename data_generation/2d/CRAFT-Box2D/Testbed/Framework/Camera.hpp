//
//  Camera.hpp
//  Testbed
//
//  Created by Tayfun Ateş on 5.10.2019.
//

#ifndef Camera_hpp
#define Camera_hpp

#include "Box2D/Box2D.h"

class Camera
{
public:
    Camera()
    {
        m_center.Set(0.0f, 20.0f);
        m_zoom = 1.0f;
        m_width = 1280;
        m_height = 800;
    }

    b2Vec2 ConvertScreenToWorld(const b2Vec2& screenPoint);
    b2Vec2 ConvertWorldToScreen(const b2Vec2& worldPoint);
    void BuildProjectionMatrix(float32* m, float32 zBias);

    b2Vec2 m_center;
    float32 m_zoom;
    int32 m_width;
    int32 m_height;
};

extern Camera g_camera;

#endif /* Camera_hpp */
