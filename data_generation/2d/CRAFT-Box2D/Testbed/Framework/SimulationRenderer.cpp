//
//  SimulationRenderer.cpp
//  Testbed
//
//  Created by Tayfun Ateş on 5.10.2019.
//

#include "SimulationRenderer.hpp"
#include "Camera.hpp"

#if defined(__APPLE_CC__)
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#else
#include "Testbed/glad/glad.h"
#endif

#include "Testbed/glfw/glfw3.h"
#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include <png.h>

extern "C" {
#include "VideoWriter.h"
}

#include "Testbed/imgui/imgui.h"
#include <iostream>

#define BUFFER_OFFSET(x)  ((const void*) (x))

#if USE_DEBUG_DRAW
#else

#define TEXTURE_SQUARE_EDGE_LENGTH 7.5

SimulationRenderer g_debugDraw;

//
static void sCheckGLError()
{
    GLenum errCode = glGetError();
    if (errCode != GL_NO_ERROR)
    {
        fprintf(stderr, "OpenGL error = %d\n", errCode);
        assert(false);
    }
}

// Prints shader compilation errors
static void sPrintLog(GLuint object)
{
    GLint log_length = 0;
    if (glIsShader(object))
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
    else if (glIsProgram(object))
        glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
    else
    {
        fprintf(stderr, "printlog: Not a shader or a program\n");
        return;
    }

    char* log = (char*)malloc(log_length);

    if (glIsShader(object))
        glGetShaderInfoLog(object, log_length, NULL, log);
    else if (glIsProgram(object))
        glGetProgramInfoLog(object, log_length, NULL, log);

    fprintf(stderr, "%s", log);
    free(log);
}


//
static GLuint sCreateShaderFromString(const char* source, GLenum type)
{
    GLuint res = glCreateShader(type);
    const char* sources[] = { source };
    glShaderSource(res, 1, sources, NULL);
    glCompileShader(res);
    GLint compile_ok = GL_FALSE;
    glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
    if (compile_ok == GL_FALSE)
    {
        fprintf(stderr, "Error compiling shader of type %d!\n", type);
        sPrintLog(res);
        glDeleteShader(res);
        return 0;
    }

    return res;
}

//
static GLuint sCreateShaderProgram(const char* vs, const char* fs)
{
    GLuint vsId = sCreateShaderFromString(vs, GL_VERTEX_SHADER);
    GLuint fsId = sCreateShaderFromString(fs, GL_FRAGMENT_SHADER);
    assert(vsId != 0 && fsId != 0);

    GLuint programId = glCreateProgram();
    glAttachShader(programId, vsId);
    glAttachShader(programId, fsId);
    glBindFragDataLocation(programId, 0, "color");
    glLinkProgram(programId);

    glDeleteShader(vsId);
    glDeleteShader(fsId);

    GLint status = GL_FALSE;
    glGetProgramiv(programId, GL_LINK_STATUS, &status);
    assert(status != GL_FALSE);
    
    return programId;
}

//
struct GLRenderPoints
{
    void Create()
    {
        const char* vs = \
        "#version 330\n"
        "uniform mat4 projectionMatrix;\n"
        "layout(location = 0) in vec2 v_position;\n"
        "layout(location = 1) in vec4 v_color;\n"
        "layout(location = 2) in float v_size;\n"
        "out vec4 f_color;\n"
        "void main(void)\n"
        "{\n"
        "    f_color = v_color;\n"
        "    gl_Position = projectionMatrix * vec4(v_position, 0.0f, 1.0f);\n"
        "   gl_PointSize = v_size;\n"
        "}\n";
        
        const char* fs = \
        "#version 330\n"
        "in vec4 f_color;\n"
        "out vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "    color = f_color;\n"
        "}\n";
        
        m_programId = sCreateShaderProgram(vs, fs);
        m_projectionUniform = glGetUniformLocation(m_programId, "projectionMatrix");
        m_vertexAttribute = 0;
        m_colorAttribute = 1;
        m_sizeAttribute = 2;
        
        // Generate
        glGenVertexArrays(1, &m_vaoId);
        glGenBuffers(3, m_vboIds);
        
        glBindVertexArray(m_vaoId);
        glEnableVertexAttribArray(m_vertexAttribute);
        glEnableVertexAttribArray(m_colorAttribute);
        glEnableVertexAttribArray(m_sizeAttribute);
        
        // Vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
        glVertexAttribPointer(m_vertexAttribute, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
        glVertexAttribPointer(m_colorAttribute, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors, GL_DYNAMIC_DRAW);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[2]);
        glVertexAttribPointer(m_sizeAttribute, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_sizes), m_sizes, GL_DYNAMIC_DRAW);

        sCheckGLError();
        
        // Cleanup
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        
        m_count = 0;
    }
    
    void Destroy()
    {
        if (m_vaoId)
        {
            glDeleteVertexArrays(1, &m_vaoId);
            glDeleteBuffers(2, m_vboIds);
            m_vaoId = 0;
        }
        
        if (m_programId)
        {
            glDeleteProgram(m_programId);
            m_programId = 0;
        }
    }
    
    void Vertex(const b2Vec2& v, const b2Color& c, float32 size)
    {
        if (m_count == e_maxVertices)
            Flush();
        
        m_vertices[m_count] = v;
        m_colors[m_count] = c;
        m_sizes[m_count] = size;
        ++m_count;
    }
    
    void Flush()
    {
        if (m_count == 0)
            return;
        
        glUseProgram(m_programId);
        
        float32 proj[16] = { 0.0f };
        g_camera.BuildProjectionMatrix(proj, 0.0f);
        
        glUniformMatrix4fv(m_projectionUniform, 1, GL_FALSE, proj);
        
        glBindVertexArray(m_vaoId);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(b2Vec2), m_vertices);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(b2Color), m_colors);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[2]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(float32), m_sizes);

        glEnable(GL_PROGRAM_POINT_SIZE);
        glDrawArrays(GL_POINTS, 0, m_count);
        glDisable(GL_PROGRAM_POINT_SIZE);

        sCheckGLError();
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);
        
        m_count = 0;
    }
    
    enum { e_maxVertices = 512 };
    b2Vec2 m_vertices[e_maxVertices];
    b2Color m_colors[e_maxVertices];
    float32 m_sizes[e_maxVertices];

    int32 m_count;
    
    GLuint m_vaoId;
    GLuint m_vboIds[3];
    GLuint m_programId;
    GLint m_projectionUniform;
    GLint m_vertexAttribute;
    GLint m_colorAttribute;
    GLint m_sizeAttribute;
};

//
struct GLRenderLines
{
    void Create()
    {
        const char* vs = \
        "#version 330\n"
        "uniform mat4 projectionMatrix;\n"
        "layout(location = 0) in vec2 v_position;\n"
        "layout(location = 1) in vec4 v_color;\n"
        "out vec4 f_color;\n"
        "void main(void)\n"
        "{\n"
        "    f_color = v_color;\n"
        "    gl_Position = projectionMatrix * vec4(v_position, 0.0f, 1.0f);\n"
        "}\n";
        
        const char* fs = \
        "#version 330\n"
        "in vec4 f_color;\n"
        "out vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "    color = f_color;\n"
        "}\n";
        
        m_programId = sCreateShaderProgram(vs, fs);
        m_projectionUniform = glGetUniformLocation(m_programId, "projectionMatrix");
        m_vertexAttribute = 0;
        m_colorAttribute = 1;
        
        // Generate
        glGenVertexArrays(1, &m_vaoId);
        glGenBuffers(2, m_vboIds);
        
        glBindVertexArray(m_vaoId);
        glEnableVertexAttribArray(m_vertexAttribute);
        glEnableVertexAttribArray(m_colorAttribute);
        
        // Vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
        glVertexAttribPointer(m_vertexAttribute, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
        glVertexAttribPointer(m_colorAttribute, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors, GL_DYNAMIC_DRAW);
        
        sCheckGLError();
        
        // Cleanup
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        
        m_count = 0;
    }
    
    void Destroy()
    {
        if (m_vaoId)
        {
            glDeleteVertexArrays(1, &m_vaoId);
            glDeleteBuffers(2, m_vboIds);
            m_vaoId = 0;
        }
        
        if (m_programId)
        {
            glDeleteProgram(m_programId);
            m_programId = 0;
        }
    }
    
    void Vertex(const b2Vec2& v, const b2Color& c)
    {
        if (m_count == e_maxVertices)
            Flush();
        
        m_vertices[m_count] = v;
        m_colors[m_count] = c;
        ++m_count;
    }
    
    void Flush()
    {
        if (m_count == 0)
            return;
        
        glUseProgram(m_programId);
        
        float32 proj[16] = { 0.0f };
        g_camera.BuildProjectionMatrix(proj, 0.1f);
        
        glUniformMatrix4fv(m_projectionUniform, 1, GL_FALSE, proj);
        
        glBindVertexArray(m_vaoId);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(b2Vec2), m_vertices);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(b2Color), m_colors);
        
        glDrawArrays(GL_LINES, 0, m_count);
        
        sCheckGLError();
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);
        
        m_count = 0;
    }
    
    enum { e_maxVertices = 2 * 512 };
    b2Vec2 m_vertices[e_maxVertices];
    b2Color m_colors[e_maxVertices];
    
    int32 m_count;
    
    GLuint m_vaoId;
    GLuint m_vboIds[2];
    GLuint m_programId;
    GLint m_projectionUniform;
    GLint m_vertexAttribute;
    GLint m_colorAttribute;
};

#if RENDER_TEXTURES
struct GLRenderTriangles
{
    void Create()
    {
        const char* vs = \
            "#version 330\n"
            "uniform mat4 projectionMatrix;\n"
            "layout(location = 0) in vec2 v_position;\n"
            "layout(location = 1) in vec4 v_color;\n"
            "layout(location = 2) in vec2 v_texCoord;\n"
            "layout(location = 3) in int v_matIndex;\n"
            "out vec4 f_color;\n"
            "out vec2 f_texCoord;\n"
            "flat out int f_matIndex;\n"
            "void main(void)\n"
            "{\n"
            "    f_color = v_color;\n"
            "    f_texCoord = v_texCoord;\n"
            "    f_matIndex = v_matIndex;\n"
            "    gl_Position = projectionMatrix * vec4(v_position, 0.0f, 1.0f);\n"
            "}\n";
        
        const char* fs = \
            "#version 330\n"
            "in vec4 f_color;\n"
            "in vec2 f_texCoord;\n"
            "flat in int f_matIndex;\n"
            "out vec4 color;\n"
            "uniform sampler2D eyesTexture;\n" 
            "void main(void)\n"
            "{\n"
            //"    if (f_matIndex==1) { color = vec4(0.0, 0.0, 0.0, 1.0); }" // If platform
            //"    else if (f_matIndex==2) { color = vec4(0.9, 0.9, 0.9, 1.0); } \n" // If Sensor body
            //"    else { \n"
            "        vec4 texCol = (f_matIndex==0) ? texture(eyesTexture, f_texCoord) : vec4(0.0,1.0,0.0,1.0);\n"
            "        if ((texCol.r <= 0.05 && texCol.g >= 0.95 && texCol.b <= 0.05)) { color = f_color; } \n"
            "        else { color = texCol; } \n"
            //"    } \n"
            "   "
            "}\n";
        
        m_textureUniforms = std::vector<GLint>(3, -1);
        m_textureIds = std::vector<GLint>(3, -1);

        m_programId = sCreateShaderProgram(vs, fs);
        m_projectionUniform = glGetUniformLocation(m_programId, "projectionMatrix");
        m_textureUniforms[0] = (glGetUniformLocation(m_programId, "eyesTexture"));
        m_vertexAttribute = 0;
        m_colorAttribute = 1;
        m_TextureCoordAttribute = 2;
        m_MaterialIndexAttribute = 3;

        // Generate
        glGenVertexArrays(1, &m_vaoId);
        glGenBuffers(4, m_vboIds);

        glBindVertexArray(m_vaoId);
        glEnableVertexAttribArray(m_vertexAttribute);
        glEnableVertexAttribArray(m_colorAttribute);
        glEnableVertexAttribArray(m_TextureCoordAttribute);
        glEnableVertexAttribArray(m_MaterialIndexAttribute);

        // Vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
        glVertexAttribPointer(m_vertexAttribute, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
        glVertexAttribPointer(m_colorAttribute, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors, GL_DYNAMIC_DRAW);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[2]);
        glVertexAttribPointer(m_TextureCoordAttribute, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_texCoordinates), m_texCoordinates, GL_DYNAMIC_DRAW);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[3]);
        glVertexAttribPointer(m_MaterialIndexAttribute, 1, GL_INT, GL_FALSE, 0, BUFFER_OFFSET(0));
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_materials), m_materials, GL_DYNAMIC_DRAW);

        sCheckGLError();

        // Cleanup
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        m_count = 0;
    }

    void Destroy()
    {
        if (m_vaoId)
        {
            glDeleteVertexArrays(1, &m_vaoId);
            glDeleteBuffers(4, m_vboIds);
            m_vaoId = 0;
        }

        if (m_programId)
        {
            glDeleteProgram(m_programId);
            m_programId = 0;
        }
    }

    void Vertex(const b2Vec2& v, const b2Color& c, const b2Vec2& t, const int& m)
    {
        if (m_count == e_maxVertices)
            Flush();

        m_vertices[m_count] = v;
        m_colors[m_count] = c;
        m_texCoordinates[m_count] = t;
        m_materials[m_count] = m;
        ++m_count;
    }

    void Flush()
    {
        if (m_count == 0)
            return;
        
        glUseProgram(m_programId);
        
        float32 proj[16] = { 0.0f };
        g_camera.BuildProjectionMatrix(proj, 0.2f);
        
        glUniformMatrix4fv(m_projectionUniform, 1, GL_FALSE, proj);
        
        glBindVertexArray(m_vaoId);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(b2Vec2), m_vertices);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(b2Color), m_colors);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[2]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(b2Vec2), m_texCoordinates);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[3]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(int), m_materials);
        
        if(m_textureIds[0]>0) {
            glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
            glBindTexture(GL_TEXTURE_2D, m_textureIds[0]);
            glUniform1i(m_textureUniforms[0], 0);
        }

        //if (m_textureIds[1] > 0) {
        //    glActiveTexture(GL_TEXTURE1); // activate the texture unit first before binding texture
        //    glBindTexture(GL_TEXTURE_2D, m_textureIds[1]);
        //    glUniform1i(m_textureUniforms[1], 1);
        //}
        //
        //if (m_textureIds[2] > 0) {
        //    glActiveTexture(GL_TEXTURE2); // activate the texture unit first before binding texture
        //    glBindTexture(GL_TEXTURE_2D, m_textureIds[2]);
        //    glUniform1i(m_textureUniforms[2], 2);
        //}
        
        glEnable(GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDrawArrays(GL_TRIANGLES, 0, m_count);
        glDisable(GL_BLEND);
        
        sCheckGLError();
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);
        
        m_count = 0;
    }
    
    enum { e_maxVertices = 3 * 512 };
    b2Vec2 m_vertices[e_maxVertices];
    b2Color m_colors[e_maxVertices];
    b2Vec2 m_texCoordinates[e_maxVertices];
    int m_materials[e_maxVertices];

    int32 m_count;

    GLuint m_vaoId;
    GLuint m_vboIds[4];
    GLuint m_programId;
    GLint m_projectionUniform;
    std::vector<GLint> m_textureUniforms;
    GLint m_vertexAttribute;
    GLint m_colorAttribute;
    GLint m_TextureCoordAttribute;
    GLint m_MaterialIndexAttribute;
    std::vector<GLint> m_textureIds;
};
#else
struct GLRenderTriangles
{
    void Create()
    {
        const char* vs = \
            "#version 330\n"
            "uniform mat4 projectionMatrix;\n"
            "layout(location = 0) in vec2 v_position;\n"
            "layout(location = 1) in vec4 v_color;\n"
            "out vec4 f_color;\n"
            "void main(void)\n"
            "{\n"
            "    f_color = v_color;\n"
            "    gl_Position = projectionMatrix * vec4(v_position, 0.0f, 1.0f);\n"
            "}\n";

        const char* fs = \
            "#version 330\n"
            "in vec4 f_color;\n"
            "out vec4 color;\n"
            "void main(void)\n"
            "{\n"
            "    color = f_color;\n"
            "}\n";

        m_programId = sCreateShaderProgram(vs, fs);
        m_projectionUniform = glGetUniformLocation(m_programId, "projectionMatrix");
        m_vertexAttribute = 0;
        m_colorAttribute = 1;

        // Generate
        glGenVertexArrays(1, &m_vaoId);
        glGenBuffers(2, m_vboIds);

        glBindVertexArray(m_vaoId);
        glEnableVertexAttribArray(m_vertexAttribute);
        glEnableVertexAttribArray(m_colorAttribute);

        // Vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
        glVertexAttribPointer(m_vertexAttribute, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
        glVertexAttribPointer(m_colorAttribute, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors, GL_DYNAMIC_DRAW);

        sCheckGLError();

        // Cleanup
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        m_count = 0;
    }

    void Destroy()
    {
        if (m_vaoId)
        {
            glDeleteVertexArrays(1, &m_vaoId);
            glDeleteBuffers(2, m_vboIds);
            m_vaoId = 0;
        }

        if (m_programId)
        {
            glDeleteProgram(m_programId);
            m_programId = 0;
        }
    }

    void Vertex(const b2Vec2& v, const b2Color& c)
    {
        if (m_count == e_maxVertices)
            Flush();

        m_vertices[m_count] = v;
        m_colors[m_count] = c;
        ++m_count;
    }

    void Flush()
    {
        if (m_count == 0)
            return;
        
        glUseProgram(m_programId);
        
        float32 proj[16] = { 0.0f };
        g_camera.BuildProjectionMatrix(proj, 0.2f);
        
        glUniformMatrix4fv(m_projectionUniform, 1, GL_FALSE, proj);
        
        glBindVertexArray(m_vaoId);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(b2Vec2), m_vertices);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(b2Color), m_colors);
        
        glEnable(GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDrawArrays(GL_TRIANGLES, 0, m_count);
        glDisable(GL_BLEND);
        
        sCheckGLError();
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glUseProgram(0);
        
        m_count = 0;
    }
    
    enum { e_maxVertices = 3 * 512 };
    b2Vec2 m_vertices[e_maxVertices];
    b2Color m_colors[e_maxVertices];

    int32 m_count;

    GLuint m_vaoId;
    GLuint m_vboIds[2];
    GLuint m_programId;
    GLint m_projectionUniform;
    GLint m_vertexAttribute;
    GLint m_colorAttribute;
};
#endif

//
SimulationRenderer::SimulationRenderer()
{
    m_points = NULL;
    m_lines = NULL;
    m_triangles = NULL;
    
    m_bIsDebugMode = false;
}

//
SimulationRenderer::~SimulationRenderer()
{
    if (m_PixelBuffer != NULL)
    {
        free(m_PixelBuffer);
        m_PixelBuffer = NULL;
    }
}

//
void SimulationRenderer::Create()
{
    m_points = new GLRenderPoints;
    m_points->Create();
    m_lines = new GLRenderLines;
    m_lines->Create();
    m_triangles = new GLRenderTriangles;
    m_triangles->Create();
}

//
void SimulationRenderer::Destroy()
{
    m_points->Destroy();
    delete m_points;
    m_points = NULL;

    m_lines->Destroy();
    delete m_lines;
    m_lines = NULL;

    m_triangles->Destroy();
    delete m_triangles;
    m_triangles = NULL;
}

//
void SimulationRenderer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    b2Vec2 p1 = vertices[vertexCount - 1];
    for (int32 i = 0; i < vertexCount; ++i)
    {
        b2Vec2 p2 = vertices[i];
        m_lines->Vertex(p1, color);
        m_lines->Vertex(p2, color);
        p1 = p2;
    }
}

void SimulationRenderer::DrawTexturedPolygon(const b2Vec2* vertices, const b2Vec2* textureCoordinates, int32 vertexCount, const b2Color& color, uint32 glTexId, int matTexId)
{
#if RENDER_TEXTURES
    const float transConst = m_bIsDebugMode ? 0.5 : 1.0;
    b2Color fillColor(transConst * color.r, transConst * color.g, transConst * color.b, transConst);
    
    m_triangles->m_textureIds[matTexId] = glTexId;

    for (int32 i = 1; i < vertexCount - 1; ++i)
    {
        m_triangles->Vertex(vertices[0], fillColor, textureCoordinates[0], matTexId);
        m_triangles->Vertex(vertices[i], fillColor, textureCoordinates[i], matTexId);
        m_triangles->Vertex(vertices[i+1], fillColor, textureCoordinates[i+1], matTexId);
    }

    if(m_bIsDebugMode) {
        b2Vec2 p1 = vertices[vertexCount - 1];
        for (int32 i = 0; i < vertexCount; ++i)
        {
            b2Vec2 p2 = vertices[i];
            m_lines->Vertex(p1, color);
            m_lines->Vertex(p2, color);
            p1 = p2;
        }
    }
#else
    DrawSolidPolygon(vertices, vertexCount, color);
#endif
}

//
void SimulationRenderer::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
#if RENDER_TEXTURES
#else
    const float transConst = m_bIsDebugMode ? 0.5 : 1.0;
    b2Color fillColor(transConst * color.r, transConst * color.g, transConst * color.b, transConst);

    for (int32 i = 1; i < vertexCount - 1; ++i)
    {
        m_triangles->Vertex(vertices[0], fillColor);
        m_triangles->Vertex(vertices[i], fillColor);
        m_triangles->Vertex(vertices[i+1], fillColor);
    }

    if(m_bIsDebugMode) {
        b2Vec2 p1 = vertices[vertexCount - 1];
        for (int32 i = 0; i < vertexCount; ++i)
        {
            b2Vec2 p2 = vertices[i];
            m_lines->Vertex(p1, color);
            m_lines->Vertex(p2, color);
            p1 = p2;
        }
    }
#endif
}

//
void SimulationRenderer::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
    const float32 k_segments = 16.0f;
    const float32 k_increment = 2.0f * b2_pi / k_segments;
    float32 sinInc = sinf(k_increment);
    float32 cosInc = cosf(k_increment);
    b2Vec2 r1(1.0f, 0.0f);
    b2Vec2 v1 = center + radius * r1;
    for (int32 i = 0; i < k_segments; ++i)
    {
        // Perform rotation to avoid additional trigonometry.
        b2Vec2 r2;
        r2.x = cosInc * r1.x - sinInc * r1.y;
        r2.y = sinInc * r1.x + cosInc * r1.y;
        b2Vec2 v2 = center + radius * r2;
        m_lines->Vertex(v1, color);
        m_lines->Vertex(v2, color);
        r1 = r2;
        v1 = v2;
    }
}

void SimulationRenderer::DrawTexturedCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color, uint32 glTexId, int matTexId)
{
#if RENDER_TEXTURES
    const float32 k_segments = 16.0f;
    const float32 k_increment = 2.0f * b2_pi / k_segments;
    float32 sinInc = sinf(k_increment);
    float32 cosInc = cosf(k_increment);
    b2Vec2 v0 = center;
    b2Vec2 r1(cosInc, sinInc);
    b2Vec2 v1 = center + radius * r1;
    b2Color fillColor(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
    
    m_triangles->m_textureIds[matTexId] = glTexId;
    
    for (int32 i = 0; i < k_segments; ++i)
    {
        // Perform rotation to avoid additional trigonometry.
        b2Vec2 r2;
        r2.x = cosInc * r1.x - sinInc * r1.y;
        r2.y = sinInc * r1.x + cosInc * r1.y;
        b2Vec2 v2 = center + radius * r2;
        
        b2Vec2 t0 = b2Vec2(v0.x / TEXTURE_SQUARE_EDGE_LENGTH, v0.y / TEXTURE_SQUARE_EDGE_LENGTH);
        b2Vec2 t1 = b2Vec2(v1.x / TEXTURE_SQUARE_EDGE_LENGTH, v1.y / TEXTURE_SQUARE_EDGE_LENGTH);
        b2Vec2 t2 = b2Vec2(v2.x / TEXTURE_SQUARE_EDGE_LENGTH, v2.y / TEXTURE_SQUARE_EDGE_LENGTH);
        
        
        m_triangles->Vertex(v0, fillColor, t0, matTexId);
        m_triangles->Vertex(v1, fillColor, t1, matTexId);
        m_triangles->Vertex(v2, fillColor, t2, matTexId);
        r1 = r2;
        v1 = v2;
    }

    r1.Set(1.0f, 0.0f);
    v1 = center + radius * r1;
    for (int32 i = 0; i < k_segments; ++i)
    {
        b2Vec2 r2;
        r2.x = cosInc * r1.x - sinInc * r1.y;
        r2.y = sinInc * r1.x + cosInc * r1.y;
        b2Vec2 v2 = center + radius * r2;
        m_lines->Vertex(v1, color);
        m_lines->Vertex(v2, color);
        r1 = r2;
        v1 = v2;
    }
#else
    DrawSolidCircle(center, radius, axis, color);
#endif
}

//
void SimulationRenderer::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
#if RENDER_TEXTURES
#else
    const float32 k_segments = 16.0f;
    const float32 k_increment = 2.0f * b2_pi / k_segments;
    float32 sinInc = sinf(k_increment);
    float32 cosInc = cosf(k_increment);
    b2Vec2 v0 = center;
    b2Vec2 r1(cosInc, sinInc);
    b2Vec2 v1 = center + radius * r1;
    b2Color fillColor(color.r, color.g, color.b, 1.0);
    for (int32 i = 0; i < k_segments; ++i)
    {
        // Perform rotation to avoid additional trigonometry.
        b2Vec2 r2;
        r2.x = cosInc * r1.x - sinInc * r1.y;
        r2.y = sinInc * r1.x + cosInc * r1.y;
        b2Vec2 v2 = center + radius * r2;
        m_triangles->Vertex(v0, fillColor);
        m_triangles->Vertex(v1, fillColor);
        m_triangles->Vertex(v2, fillColor);
        r1 = r2;
        v1 = v2;
    }

    if(m_bIsDebugMode) {
        r1.Set(1.0f, 0.0f);
        v1 = center + radius * r1;
        for (int32 i = 0; i < k_segments; ++i)
        {
            b2Vec2 r2;
            r2.x = cosInc * r1.x - sinInc * r1.y;
            r2.y = sinInc * r1.x + cosInc * r1.y;
            b2Vec2 v2 = center + radius * r2;
            m_lines->Vertex(v1, color);
            m_lines->Vertex(v2, color);
            r1 = r2;
            v1 = v2;
        }

        // Draw a line fixed in the circle to animate rotation.
        b2Vec2 p = center + radius * axis;
        m_lines->Vertex(center, color);
        m_lines->Vertex(p, color);
    }
#endif
}

//
void SimulationRenderer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
    m_lines->Vertex(p1, color);
    m_lines->Vertex(p2, color);
}

void SimulationRenderer::DrawRectangleChain(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color, float width)
{
    float dx = p2.x - p1.x; //delta x
    float dy = p2.y - p1.y; //delta y
    float linelength = sqrtf(dx * dx + dy * dy);
    dx /= linelength;
    dy /= linelength;
    //Ok, (dx, dy) is now a unit vector pointing in the direction of the line
    //A perpendicular vector is given by (-dy, dx)
    const float thickness = width; //Some number
    const float px = 0.5f * thickness * (-dy); //perpendicular vector with lenght thickness * 0.5
    const float py = 0.5f * thickness * dx;
    
    std::vector<b2Vec2> vertices = {b2Vec2(p1.x + px, p1.y + py), b2Vec2(p2.x + px, p2.y + py), b2Vec2(p2.x - px, p2.y - py), b2Vec2(p1.x - px, p1.y - py)};
    DrawSolidPolygon(vertices.data(), vertices.size(), color);
}

void SimulationRenderer::DrawTexturedRectangleChain(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color, float width, uint32 glTexId, int matTexId)
{
    float dx = p2.x - p1.x; //delta x
    float dy = p2.y - p1.y; //delta y
    float linelength = sqrtf(dx * dx + dy * dy);
    dx /= linelength;
    dy /= linelength;
    //Ok, (dx, dy) is now a unit vector pointing in the direction of the line
    //A perpendicular vector is given by (-dy, dx)
    const float thickness = width; //Some number
    const float px = 0.5f * thickness * (-dy); //perpendicular vector with lenght thickness * 0.5
    const float py = 0.5f * thickness * dx;

    std::vector<b2Vec2> vertices = { 
        b2Vec2(p1.x + px, p1.y + py),
        b2Vec2(p2.x + px, p2.y + py),
        b2Vec2(p2.x - px, p2.y - py),
        b2Vec2(p1.x - px, p1.y - py) 
    };

    std::vector<b2Vec2> texCoords = {
        b2Vec2(vertices[0].x / TEXTURE_SQUARE_EDGE_LENGTH, vertices[0].y / TEXTURE_SQUARE_EDGE_LENGTH),
        b2Vec2(vertices[1].x / TEXTURE_SQUARE_EDGE_LENGTH, vertices[1].y / TEXTURE_SQUARE_EDGE_LENGTH),
        b2Vec2(vertices[2].x / TEXTURE_SQUARE_EDGE_LENGTH, vertices[2].y / TEXTURE_SQUARE_EDGE_LENGTH),
        b2Vec2(vertices[3].x / TEXTURE_SQUARE_EDGE_LENGTH, vertices[3].y / TEXTURE_SQUARE_EDGE_LENGTH),
    };

    DrawTexturedPolygon(vertices.data(), texCoords.data(), vertices.size(), color, glTexId, matTexId);
}

//
void SimulationRenderer::DrawTransform(const b2Transform& xf)
{
    const float32 k_axisScale = 0.4f;
    b2Color red(1.0f, 0.0f, 0.0f);
    b2Color green(0.0f, 1.0f, 0.0f);
    b2Vec2 p1 = xf.p, p2;

    m_lines->Vertex(p1, red);
    p2 = p1 + k_axisScale * xf.q.GetXAxis();
    m_lines->Vertex(p2, red);

    m_lines->Vertex(p1, green);
    p2 = p1 + k_axisScale * xf.q.GetYAxis();
    m_lines->Vertex(p2, green);
}

//
void SimulationRenderer::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
{
    m_points->Vertex(p, color, size);
}

//
void SimulationRenderer::DrawString(int x, int y, const char *string, ...)
{
    va_list arg;
    va_start(arg, string);
    ImGui::Begin("Overlay", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar);
    ImGui::SetCursorPos(b2Vec2(float(x), float(y)));
    ImGui::TextColoredV(ImColor(230, 153, 153, 255), string, arg);
    ImGui::End();
    va_end(arg);
}

//
void SimulationRenderer::DrawString(const b2Vec2& pw, const char *string, ...)
{
    b2Vec2 ps = g_camera.ConvertWorldToScreen(pw);

    va_list arg;
    va_start(arg, string);
    ImGui::Begin("Overlay", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar);
    ImGui::SetCursorPos(ps);
    ImGui::TextColoredV(ImColor(230, 153, 153, 255), string, arg);
    ImGui::End();
    va_end(arg);
}

//
void SimulationRenderer::DrawAABB(b2AABB* aabb, const b2Color& c)
{
    b2Vec2 p1 = aabb->lowerBound;
    b2Vec2 p2 = b2Vec2(aabb->upperBound.x, aabb->lowerBound.y);
    b2Vec2 p3 = aabb->upperBound;
    b2Vec2 p4 = b2Vec2(aabb->lowerBound.x, aabb->upperBound.y);
    
    m_lines->Vertex(p1, c);
    m_lines->Vertex(p2, c);

    m_lines->Vertex(p2, c);
    m_lines->Vertex(p3, c);

    m_lines->Vertex(p3, c);
    m_lines->Vertex(p4, c);

    m_lines->Vertex(p4, c);
    m_lines->Vertex(p1, c);
}

bool save_png_libpng(const char *filename, unsigned char* pixels, int w, int h)
{
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png)
        return false;

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_write_struct(&png, &info);
        return false;
    }

    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        png_destroy_write_struct(&png, &info);
        return false;
    }

    png_init_io(png, fp);
    png_set_IHDR(png, info, w, h, 8 /* depth */, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_colorp palette = (png_colorp)png_malloc(png, PNG_MAX_PALETTE_LENGTH * sizeof(png_color));
    if (!palette) {
        fclose(fp);
        png_destroy_write_struct(&png, &info);
        return false;
    }
    png_set_PLTE(png, info, palette, PNG_MAX_PALETTE_LENGTH);
    png_write_info(png, info);
    png_set_packing(png);

    png_bytepp rows = (png_bytepp)png_malloc(png, h * sizeof(png_bytep));
    for (int i = 0; i < h; ++i)
        rows[i] = (png_bytep)(pixels + (h - i - 1) * w * 3);

    png_write_image(png, rows);
    png_write_end(png, info);
    png_free(png, palette);
    png_destroy_write_struct(&png, &info);

    fclose(fp);
    delete[] rows;
    return true;
}
    
void SimulationRenderer::SaveAsImage(std::string path)
{
    //glFlush();
    //unsigned char* image = (unsigned char*)malloc(sizeof(unsigned char) * 3 * width * height);
    //glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);
    
    sCheckGLError();
    
    flipVertically(m_PixelBuffer, m_nWidth, m_nHeight, 3);
    save_png_libpng(path.c_str(), m_PixelBuffer, m_nWidth, m_nHeight);
    flipVertically(m_PixelBuffer, m_nWidth, m_nHeight, 3);
}

//
void SimulationRenderer::Flush()
{
    m_triangles->Flush();
    m_lines->Flush();
    m_points->Flush();

    unsigned int width = m_nWidth;
    unsigned int height = m_nHeight;
    
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, m_PixelBuffer); 
    
    if (writingToVideo()) {
        videoFlush(m_PixelBuffer, width, height);
    }
}

void SimulationRenderer::Finish()
{
    if (writingToVideo()) {
        deinit();
    }
}

//Setters and getters
void SimulationRenderer::setIsDebugMode(const bool &isDebug) {
    m_bIsDebugMode = isDebug;
}

bool SimulationRenderer::getIsDebugMode() const {
    return m_bIsDebugMode;
}

void SimulationRenderer::setFileOutput(const std::string& filePath, const int& width, const int& height)
{
    m_sPath = filePath;
    m_nWidth = width;
    m_nHeight = height;

    if (m_PixelBuffer != NULL) {
        free(m_PixelBuffer);
        m_PixelBuffer = NULL;
    }

    m_PixelBuffer = (unsigned char*)malloc(sizeof(unsigned char) * 3 * width * height);

    if (writingToVideo())
    {
        init(m_sPath, m_nWidth, m_nHeight);
    }
}

#endif

