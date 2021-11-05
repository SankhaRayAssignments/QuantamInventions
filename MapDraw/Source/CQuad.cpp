#include "CQuad.h"

CQuad::CQuad(glm::vec3 min, glm::vec3 max)
{
    mFilePath = "";
    mbDataLoaded = false;
    mb_Draw = true;

    mMin = min;
    mMax = max;
}

CQuad::CQuad(float width, float ypos)
{
    mFilePath = "";
    mbDataLoaded = false;
    mb_Draw = true;

    mMin = glm::vec3(-width, ypos, -width);
    mMax = glm::vec3( width, ypos,  width);
}

CQuad::~CQuad(void)
{
}

bool CQuad::LoadDataFromFile()
{
    mbDataLoaded = true;
    return mbDataLoaded;
}

void CQuad::LoadScene()
{
    // Open Shaders etc..
    vector<string> attribs;
    attribs.push_back("aPosition");
    attribs.push_back("aColor");

    vector<string> uniforms;
    uniforms.push_back("uMVPMatrix");

    mShader = new ShaderHandler("shaders/buildings.vert", "shaders/buildings.frag", attribs, uniforms);

    // Create the vertices
    glm::vec3 point_0(mMin.x, mMin.y, mMin.z);
    glm::vec3 point_1(mMax.x, mMin.y, mMin.z);
    glm::vec3 point_2(mMax.x, mMin.y, mMax.z);
    glm::vec3 point_3(mMin.x, mMin.y, mMax.z);

    glm::vec4 color_yellow = glm::vec4(1,1,0,1);    // yellow
    glm::vec4 color_grey   = glm::vec4(230.0f); color_grey /= 255.0f;

    // Draw the roof . i.e sqaure/base as a quad
    mVertices.push_back(point_0);   mColor.push_back(color_grey);
    mVertices.push_back(point_3);   mColor.push_back(color_grey);
    mVertices.push_back(point_2);   mColor.push_back(color_grey);
    mVertices.push_back(point_1);   mColor.push_back(color_grey);

}

void CQuad::DrawScene(CCamera *cam)
{
    if ( !mb_Draw ) return;

    string s_mvp = "uMVPMatrix";
    string pos = "aPosition";
    string col = "aColor";

    // Set the shader program
    mShader->useShader();

    glm::mat4 model(1.0);
    glm::mat4 viewproj = cam->getCameraMatrix();
    glm::mat4 mvp = viewproj * model;

    // Pass the transformationMatrix to the shader using its location
    glUniformMatrix4fv( mShader->getUniformHandle( s_mvp ), 1, GL_FALSE, glm::value_ptr(mvp));

    mShader->setAttribs( pos, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), &mVertices[0]);
    mShader->setAttribs( col, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), &mColor[0]);
    glDrawArrays(GL_QUADS, 0, mVertices.size());

    mShader->unsetAttribs();
}

void CQuad::DestroyScene()
{
    // Close shaders etc...
    mShader->DetachShaders();

    delete mShader;
}
