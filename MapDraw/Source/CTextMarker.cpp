#include "CTextMarker.h"
#include "math_helper.h"
#include "mb_GLDebug.h"
#include "OSUtil.h"
#include <glm/gtx/vector_angle.hpp>
#include <iostream>

CTextMarker::CTextMarker(void)
{
    mFilePath = "";
    mbDataLoaded = false;
    mb_Draw = true;
}

CTextMarker::~CTextMarker(void)
{
}

bool CTextMarker::LoadDataFromFile()
{
    mbDataLoaded = true;
    return mbDataLoaded;
}

void CTextMarker::LoadScene()
{
    // Open Shaders etc..
    vector<string> attribs;
    attribs.push_back("aPosition");
    attribs.push_back("aColor");

    vector<string> uniforms;
    uniforms.push_back("uMVPMatrix");

    mShader = new ShaderHandler("shaders/buildings.vert", "shaders/buildings.frag", attribs, uniforms);

    attribs.clear();
    attribs.push_back("aPosition");
    attribs.push_back("aTexCoord");
    mShaderTex = new ShaderHandler("shaders/text_marker.vert", "shaders/text_marker.frag", attribs, uniforms);

    // Create the Vertices
    // We need a rectangle and a triangle
    glm::vec3 rect_point_0 = glm::vec3(0,0,0);
    glm::vec3 rect_point_1 = glm::vec3(0,1,0);
    glm::vec3 rect_point_2 = glm::vec3(2,1,0);
    glm::vec3 rect_point_3 = glm::vec3(2,0,0);

    // And We need a traingle just below the triangles
    glm::vec3 tri_point_0 = rect_point_0;
    glm::vec3 tri_point_1 = glm::vec3(0.125,-1,0);
    glm::vec3 tri_point_2 = glm::vec3(0.25,0,0);

    glm::vec4 aColor(100); aColor /= 255.0f;
    // Push to vertex array
    // Rectangles as quads
    mVertices.push_back(rect_point_0);  mTexCoords.push_back(glm::vec2(0,0));
    mVertices.push_back(rect_point_1);  mTexCoords.push_back(glm::vec2(0,1));
    mVertices.push_back(rect_point_2);  mTexCoords.push_back(glm::vec2(1,1));
    mVertices.push_back(rect_point_3);  mTexCoords.push_back(glm::vec2(1,0));

    // Below Traingle
    mVertices.push_back(tri_point_1); mColors.push_back(glm::vec4(1));
    mVertices.push_back(tri_point_0); mColors.push_back(aColor);
    mVertices.push_back(tri_point_2); mColors.push_back(aColor);
}

void CTextMarker::DrawScene(CCamera *cam)
{
}

void CTextMarker::DrawScene(CCamera *cam, GLuint texId, glm::vec3& position)
{
    if ( !mb_Draw ) return;
    if ( cam->isTopView() && cam->getHeight() > 100.0f) return;

    // attributes
    string pos      = "aPosition";
    string col      = "aColor";
    string texCoord = "aTexCoord";

    // uniforms
    string s_mvp    = "uMVPMatrix";

    // STEP 1: Now Render the QUAD with Text
    // Set the shader program
    mShaderTex->useShader();
    glm::vec3 lookVec = cam->getLookVec();
    glm::mat4 model(1.0);
    model = glm::translate(model, position);            // Translate by Road poistion
    model = glm::translate(model, glm::vec3(0,1,0));    // Traslate by its own height

    float angle = 0.0f;

    if ( !cam->isTopView())
    {
        model = glm::rotate(model, cam->getRotation(), glm::vec3(0,1,0));
    }
    else
    {
        model = glm::rotate(model, cam->getRotation(), glm::vec3(0,1,0));
        model = glm::rotate(model, -90.0f, glm::vec3(1,0,0));
    }

    glm::mat4 viewproj = cam->getCameraMatrix();
    glm::mat4 mvp = viewproj * model;

    // Pass the transformationMatrix to the shader using its location
    glUniformMatrix4fv( mShaderTex->getUniformHandle( s_mvp ), 1, GL_FALSE, glm::value_ptr(mvp));

    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0 );
    glBindTexture(GL_TEXTURE_2D, texId);
    mShaderTex->setAttribs( pos, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), &mVertices[0]);
    mShaderTex->setAttribs( texCoord, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), &mTexCoords[0]);
    glDrawArrays(GL_QUADS, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);

    mShaderTex->unsetAttribs();

    if ( !cam->isTopView() )
    {
        // STEP 2: Now Render the triangle
        // Set the shader program
        mShader->useShader();
        // Pass the transformationMatrix to the shader using its location
        glUniformMatrix4fv( mShaderTex->getUniformHandle( s_mvp ), 1, GL_FALSE, glm::value_ptr(mvp));
        // then the triangle using Triangle fan
        mShader->setAttribs( pos, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), &mVertices[4]);
        mShader->setAttribs( col, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), &mColors[0]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 3 );

        mShader->unsetAttribs();
    }
}

void CTextMarker::DestroyScene()
{
    // Close shaders etc...
    for ( int texId = 0; texId < (int) mTexIds.size(); texId++)
        glDeleteTextures(1, &mTexIds[texId]);

    mShader->DetachShaders();
    mShaderTex->DetachShaders();

    delete mShader;
    delete mShaderTex;
}

GLuint CTextMarker::getTexID(string& text, glm::vec4& bg_color, glm::vec4& font_color)
{
    // Get BMP data from free type
    int w, h;
    glm::vec4 *pixels = getBMPfromFreeType(text, "C:\\Windows\\Fonts\\Calibri.ttf", bg_color, font_color, &w, &h);

    // Load pixels to gpu
    GLuint tex;
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    // Set Texture Parameters
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, (void *)&pixels[0]);

    mTexIds.push_back(tex);

    delete pixels;

    return tex;
}

void
CTextMarker::draw_bitmap( FT_GlyphSlot slot,
                         glm::vec4& fontColor,
                         glm::vec4* pxl, int w, int h)
{
    FT_Bitmap bitmap = slot->bitmap;
    int      x = slot->bitmap_left;
    int      y = h-slot->bitmap_top;

    FT_Int  i, j, p, q;
    FT_Int  x_max = x + bitmap.width;
    FT_Int  y_max = y + bitmap.rows;

    for ( i = x, p = 0; i < x_max; i++, p++ )
    {
        for ( j = y, q = 0; j < y_max; j++, q++ )
        {
            if ( i < 0      || j < 0       ||
                i >= w || j >= h )
                continue;

            if (bitmap.buffer[q * bitmap.width + p] >= 128 )
            {
                pxl[ (h-1-j)*w + i ] = fontColor;
            }
        }
    }
}

glm::vec4 *CTextMarker::getBMPfromFreeType(string &text, const char* fontName, glm::vec4& bg_color, glm::vec4& font_color, int *w, int *h)
{
    FT_Library      library;
    FT_Face         face;

    FT_Matrix       matrix;                 /* transformation matrix */
    FT_Vector       pen;                    /* untransformed origin  */

    FT_GlyphSlot    slot;

    int fontsize  = 30;

    std::ifstream fontFile(fontName, std::ios::binary);
    if (!fontFile)
    {
        std::cerr << "font file not found " << fontName;
        return NULL;
    }
    // Read the entire file to a memory buffer.
    fontFile.seekg(0, std::ios::end);
    std::fstream::pos_type fontFileSize = fontFile.tellg();
    fontFile.seekg(0);
    unsigned char *fontBuffer = new unsigned char[fontFileSize];
    fontFile.read((char *)fontBuffer, fontFileSize);


    float angle = 0.0f;
    /* set up matrix */
    matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
    matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
    matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
    matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

    pen.x = 0;
    pen.y = 0;

    // Initialize FreeType.
    FT_Init_FreeType(&library);

    // Create a face from a memory buffer.  Be sure not to delete the memory
    // buffer until you are done using that font as FreeType will reference
    // it directly.
    FT_New_Memory_Face(library, fontBuffer, fontFileSize, 0, &face);
    slot = face->glyph;

    FT_Set_Char_Size(face, fontsize << 6, fontsize << 6, 0, 0);


    int text_size = text.size();
    int width = 0, height = 0;
    // Load the glyph we are looking for.
    for (int i = 0; i < text_size; i++) {
        /* set transformation */
        FT_Set_Transform( face, &matrix, &pen );

        if(FT_Load_Char( face, text[i], FT_LOAD_RENDER ))
            continue;

        width += slot->bitmap.width;
        height = max(height, (int)slot->bitmap.rows);

        pen.x += slot->advance.x;
        pen.y += slot->advance.y;
    }

    width  += 100;
    height += 10;

    // Allocate space for large bitmap
    glm::vec4 *pixels = new glm::vec4 [width * height];

    // Fill the pixels with bg color
    for ( int i = 0; i < width; i++)
        for ( int j = 0; j < height; j++)
            pixels[i*height + j] = bg_color;

    pen.x = (10 << 6);
    pen.y = ((height - (fontsize - fontsize/6) ) << 6);

    FT_Bitmap* bitmap = &slot->bitmap;

    // Create the actual bitmap
    for (int i = 0; i < text_size; i++) {

        FT_Set_Transform( face, &matrix, &pen );

        if(FT_Load_Char( face, text[i], FT_LOAD_RENDER ))
            continue;

        // Fill our pixels
        draw_bitmap(slot, font_color, pixels, width, height);

        pen.x += slot->advance.x;
        pen.y += slot->advance.y;
    }

    // Clean up the library
    FT_Done_FreeType(library);

    *w = width;
    *h = height;

    return pixels;
}