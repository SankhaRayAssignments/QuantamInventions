#pragma once
#include "IRenderObjects.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H

using namespace std;
class CTextMarker : public IRenderObjects
{
private:
    vector <glm::vec3> mVertices;
    vector <glm::vec4> mColors;
    vector <glm::vec2> mTexCoords;

    vector <GLuint> mTexIds;        // Texture Ids

    // Shaders
    ShaderHandler *mShaderTex;

public:
    CTextMarker(void);
    ~CTextMarker(void);
    void LoadScene();
    void DrawScene(CCamera *cam);
    void DrawScene(CCamera *cam, GLuint texId, glm::vec3& position);
    void DestroyScene();

    bool LoadDataFromFile();

    GLuint getTexID(string& text, glm::vec4& bg_color, glm::vec4& font_color);

private:

    glm::vec4 *getBMPfromFreeType(string &text, const char* fontName, glm::vec4& bg_color, glm::vec4& font_color, int *w, int *h);

    void draw_bitmap( FT_GlyphSlot slot,  glm::vec4& fontColor, glm::vec4* pxl, int w, int h);
};
