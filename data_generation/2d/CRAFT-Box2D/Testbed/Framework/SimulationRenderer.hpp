//
//  SimulationRenderer.hpp
//  Testbed
//
//  Created by Tayfun Ateş on 5.10.2019.
//

#ifndef SimulationRenderer_hpp
#define SimulationRenderer_hpp

#include "SimulationDefines.h"
#include "Box2D/Box2D.h"
#include <string>

#define RENDER_TEXTURES 1

struct b2AABB;
struct GLRenderPoints;
struct GLRenderLines;
struct GLRenderTriangles;

// This class implements debug drawing callbacks that are invoked
// inside b2World::Step.
class SimulationRenderer : public b2Draw
{
public:
    SimulationRenderer();
    virtual ~SimulationRenderer();

    void Create();
    void Destroy();

    void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

    void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
    
    void DrawTexturedPolygon(const b2Vec2* vertices, const b2Vec2* textureCoordinates, int32 vertexCount, const b2Color& color, uint32 glTexId, int matTexId) override;

    void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) override;

    void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) override;
    
    void DrawTexturedCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color, uint32 glTexId, int matTexId) override;

    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;

    void DrawRectangleChain(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color, float width) override;

    void DrawTexturedRectangleChain(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color, float width, uint32 glTexId, int matTexId) override;

    void DrawTransform(const b2Transform& xf) override;

    void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) override;

    void DrawString(int x, int y, const char* string, ...);

    void DrawString(const b2Vec2& p, const char* string, ...);

    void DrawAABB(b2AABB* aabb, const b2Color& color);
    
    void setFileOutput(const std::string& filePath, const int& width, const int& height);

    void Flush();
    
    void Finish();
    
    //Setters and getters
    void setIsDebugMode(const bool& isDebug);
    bool getIsDebugMode() const;
    
    bool writingToVideo()
    {
        //TODO: Check extension check
        return m_sPath != "";
    }
    
    void SaveAsImage(std::string path);
    
private:
    GLRenderPoints* m_points;
    GLRenderLines* m_lines;
    GLRenderTriangles* m_triangles;
    
    bool m_bIsDebugMode;
    std::string m_sPath;
    int m_nWidth;
    int m_nHeight;

    unsigned char* m_PixelBuffer = NULL;
};

#if USE_DEBUG_DRAW
#else
extern SimulationRenderer g_debugDraw;
#endif

#endif /* SimulationRenderer_hpp */
