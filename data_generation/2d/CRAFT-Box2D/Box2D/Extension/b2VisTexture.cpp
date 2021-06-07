//
//  b2VisTexture.cpp
//  Box2D
//
//  Created by Tayfun Ateş on 6.10.2019.
//

#include "b2VisTexture.hpp"
#if defined(__APPLE_CC__)
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#else
#include "Testbed/glad/glad.h"
#endif
#include <png.h>

bool loadPngImage(const char *name, int &outWidth, int &outHeight, bool &outHasAlpha, GLubyte **outData) {
    png_structp png_ptr;
    png_infop info_ptr;
    unsigned int sig_read = 0;
    int color_type, interlace_type;
    FILE *fp;
 
    if ((fp = fopen(name, "rb")) == NULL)
        return false;
 
    /* Create and initialize the png_struct
     * with the desired error handler
     * functions.  If you want to use the
     * default stderr and longjump method,
     * you can supply NULL for the last
     * three parameters.  We also supply the
     * the compiler header file version, so
     * that we know if the application
     * was compiled with a compatible version
     * of the library.  REQUIRED
     */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                     NULL, NULL, NULL);
 
    if (png_ptr == NULL) {
        fclose(fp);
        return false;
    }
 
    /* Allocate/initialize the memory
     * for image information.  REQUIRED. */
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return false;
    }
 
    /* Set error handling if you are
     * using the setjmp/longjmp method
     * (this is the normal method of
     * doing things with libpng).
     * REQUIRED unless you  set up
     * your own error handlers in
     * the png_create_read_struct()
     * earlier.
     */
    if (setjmp(png_jmpbuf(png_ptr))) {
        /* Free all of the memory associated
         * with the png_ptr and info_ptr */
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        /* If we get here, we had a
         * problem reading the file */
        return false;
    }
 
    /* Set up the output control if
     * you are using standard C streams */
    png_init_io(png_ptr, fp);
 
    /* If we have already
     * read some of the signature */
    png_set_sig_bytes(png_ptr, sig_read);
 
    /*
     * If you have enough memory to read
     * in the entire image at once, and
     * you need to specify only
     * transforms that can be controlled
     * with one of the PNG_TRANSFORM_*
     * bits (this presently excludes
     * dithering, filling, setting
     * background, and doing gamma
     * adjustment), then you can read the
     * entire image (including pixels)
     * into the info structure with this
     * call
     *
     * PNG_TRANSFORM_STRIP_16 |
     * PNG_TRANSFORM_PACKING  forces 8 bit
     * PNG_TRANSFORM_EXPAND forces to
     *  expand a palette into RGB
     */
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);
 
    png_uint_32 width, height;
    int bit_depth;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                 &interlace_type, NULL, NULL);
    outWidth = width;
    outHeight = height;
    
    outHasAlpha = (color_type & PNG_COLOR_MASK_ALPHA);
 
    unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    *outData = (unsigned char*) malloc(row_bytes * outHeight);
 
    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
 
    for (int i = 0; i < outHeight; i++) {
        // note that png is ordered top to
        // bottom, but OpenGL expect it bottom to top
        // so the order or swapped
        memcpy(*outData+(row_bytes * (outHeight-1-i)), row_pointers[i], row_bytes);
    }
 
    /* Clean up after the read,
     * and free any memory allocated */
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
 
    /* Close the file */
    fclose(fp);
 
    /* That's it */
    return true;
}

b2VisTexture::b2VisTexture(const unsigned int& texId, const int& atlasWidth, const int& atlasHeight, const b2Vec2& upperLeftInPixels, const b2Vec2& lowerRightInPixels)
{
    m_bOwner = false;
    m_nTexId = texId;
    m_nAtlasHeight = atlasWidth;
    m_nAtlasHeight = atlasHeight;
    m_vUpperLeftCornerCoord = upperLeftInPixels;
    m_vLowerRightCornerCoord = lowerRightInPixels;
    
    m_nWidth = (lowerRightInPixels.x - upperLeftInPixels.x);
    m_nHeight = (lowerRightInPixels.y - upperLeftInPixels.y);
    
    m_nMaterialIndex = -1;
}

b2VisTexture::b2VisTexture(const std::string& filePath, const int& materialIndex)
{
    int width, height;
    bool hasAlpha;
    
    GLubyte *textureImage;
    bool success = loadPngImage(filePath.c_str(), width, height, hasAlpha, &textureImage);
    
    if(success) {
        m_nAtlasWidth = width;
        m_nAtlasHeight = height;
        m_nWidth = width;
        m_nHeight = height;
        m_bOwner = true;
        m_vUpperLeftCornerCoord = b2Vec2(0.0f, 0.0f);
        m_vLowerRightCornerCoord = b2Vec2(m_nWidth-1, m_nHeight-1);
        
        m_nMaterialIndex = materialIndex;
        
        GLuint texId;
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, hasAlpha ? GL_RGBA : GL_RGB, width, height, 0, hasAlpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, textureImage);

        m_nTexId = texId;
        free(textureImage);
    }
}

b2VisTexture::b2VisTexture(const int& materialIndex) 
{
    m_nMaterialIndex = materialIndex;
}

b2VisTexture::~b2VisTexture()
{
    if(m_bOwner && m_nTexId>0) {
        glDeleteTextures(1, &m_nTexId);
    }
}

unsigned int b2VisTexture::getTextureId() const
{
    return m_nTexId;
}

int b2VisTexture::getWidth() const
{
    return m_nWidth;
}

int b2VisTexture::getHeight() const
{
    return m_nHeight;
}

int b2VisTexture::getAtlasWidth() const
{
    return m_nAtlasWidth;
}

int b2VisTexture::getAtlasHeight() const
{
    return m_nAtlasHeight;
}

b2Vec2 b2VisTexture::getUpperLeftCornerCoord() const
{
    return m_vUpperLeftCornerCoord;
}

b2Vec2 b2VisTexture::getLowerRightCornerCoord() const
{
    return m_vLowerRightCornerCoord;
}

int b2VisTexture::getMaterialIndex() const
{
    return m_nMaterialIndex;
}
