#include "CPOIsGL.h"
#include "mb_GLDebug.h"
#include <glm/gtx/rotate_vector.hpp>

static const float POI_Y = 0.3f;

CPOIsGL::CPOIsGL(string data_path)
{
    mFilePath = data_path;
    mbDataLoaded = false;
    mb_Draw = true;
}

CPOIsGL::~CPOIsGL(void)
{
}

bool CPOIsGL::LoadDataFromFile()
{
    if ( mbDataLoaded ) return true;
    CFileIO file(mFilePath);
    if ( !file.isFileOpened() )
        return false;

    printf("Loading Point of Interest Data.....\n");

    mNumPOIs = file.GetInt("Number of POIs :");

    for ( int i = 0; i < mNumPOIs; i++)
    {
        POI a_poi;
        glm::vec3 pos = file.GetVerts();
        pos.y = POI_Y;

        a_poi.mVertices.push_back(pos);


        // Create POI primitive
        glm::vec3 point_0 = a_poi.mVertices[0];

        const float height  = 2.0f;
        const float L       = 1.0f;

        float h = point_0.x;
        float point_base_mid_y = point_0.y + height;
        float k = point_0.z;

        // Calculate roof tops
        glm::vec3 point_base_0 = glm::vec3(h - 0.5f * L, point_base_mid_y, k - 0.5*L);
        glm::vec3 point_base_1 = glm::vec3(h + 0.5f * L, point_base_mid_y, k - 0.5*L);
        glm::vec3 point_base_2 = glm::vec3(h + 0.5f * L, point_base_mid_y, k + 0.5*L);
        glm::vec3 point_base_3 = glm::vec3(h - 0.5f * L, point_base_mid_y, k + 0.5*L);

        // Colors
        glm::vec4 color_yellow = glm::vec4(1,1,0,1);    // yellow
        glm::vec4 color_green  = glm::vec4(0,1,0,1);    // green
        glm::vec4 color_orange = glm::vec4(255,128,64,255); color_orange /= 255.0f;// orange
        glm::vec4 color_blue   = glm::vec4(0,0,1,1);    // Blue
        glm::vec4 color_red    = glm::vec4(1,0,0,1);    // Red
        glm::vec4 color_purple(166,0,218,255); color_purple/=255.0f;

        // Put it into list
        // Triangle Fans
        a_poi.mPyramidVertices.push_back(point_0);      a_poi.mColor.push_back(glm::vec4(1.0f));
        a_poi.mPyramidVertices.push_back(point_base_0); a_poi.mColor.push_back(color_yellow);
        a_poi.mPyramidVertices.push_back(point_base_1); a_poi.mColor.push_back(color_yellow);
        a_poi.mPyramidVertices.push_back(point_base_2); a_poi.mColor.push_back(color_purple);
        a_poi.mPyramidVertices.push_back(point_base_3); a_poi.mColor.push_back(color_purple);
        a_poi.mPyramidVertices.push_back(point_base_0); a_poi.mColor.push_back(color_yellow);

        // Draw the roof . i.e sqaure/base as a quad
        a_poi.mPyramidVertices.push_back(point_base_0); a_poi.mColor.push_back(color_purple);
        a_poi.mPyramidVertices.push_back(point_base_3); a_poi.mColor.push_back(color_purple);
        a_poi.mPyramidVertices.push_back(point_base_2); a_poi.mColor.push_back(color_purple);
        a_poi.mPyramidVertices.push_back(point_base_1); a_poi.mColor.push_back(glm::vec4(1.0f));

        mPOIs.push_back(a_poi);
    }

    mbDataLoaded = true;
    printf("Finished Loading Point of Interest Data.....\n");

    return true;
}

void CPOIsGL::LoadScene()
{
    // Open Shaders etc..
    vector<string> attribs;
    attribs.push_back("aPosition");
    attribs.push_back("aColor");

    vector<string> uniforms;
    uniforms.push_back("uMVPMatrix");

    mShader = new ShaderHandler("shaders/buildings.vert", "shaders/buildings.frag", attribs, uniforms);

    GLuint  vertexBuffer = 0;
}

void CPOIsGL::DrawScene(CCamera *cam)
{
    if ( !mb_Draw ) return;

    if ( cam->isTopView() && cam->getHeight() > 100.0f) return;

    string s_mvp = "uMVPMatrix";
    string pos = "aPosition";
    string col = "aColor";

    // Set the shader program
    mShader->useShader();

    glm::mat4 model(1.0);
    if( !cam->isTopView())
    {
        // Scale it down.
        model = glm::scale(model, glm::vec3(0.25));
    }

#if 0   //TODO
    // Rotate Along its own axis
    static float angle = 1.0f;
    model = glm::rotate(model, angle, glm::vec3(0,1,0));
    angle += 0.1f;
    if ( angle >= 360.0f) angle -= 360.0f;
#endif

    glm::mat4 viewproj = cam->getCameraMatrix();
    glm::mat4 mvp = viewproj * model;

    // Pass the transformationMatrix to the shader using its location
    glUniformMatrix4fv( mShader->getUniformHandle( s_mvp ), 1, GL_FALSE, glm::value_ptr(mvp));

    for ( int i = 0; i < mNumPOIs; i++)
    {
        // Draw the sides first using Triangle fan
        mShader->setAttribs( pos, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), &mPOIs[i].mPyramidVertices[0]);
        mShader->setAttribs( col, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), &mPOIs[i].mColor[0]);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 6 );

        // Now draw the quad
        mShader->setAttribs( pos, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), &mPOIs[i].mPyramidVertices[6]);
        mShader->setAttribs( col, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), &mPOIs[i].mColor[6]);
        glDrawArrays(GL_QUADS, 0, 4);
    }

    mShader->unsetAttribs();
}

void CPOIsGL::DestroyScene()
{
    // Close shaders etc...
    mShader->DetachShaders();
    delete mShader;
}
