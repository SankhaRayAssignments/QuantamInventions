#include "CFeaturesGL.h"
#include "mb_GLDebug.h"
#include "math_helper.h"

CFeaturesGL::CFeaturesGL(string data_path)
{
    mFilePath = data_path;
    mbDataLoaded = false;
    mb_Draw = true;
    draw_feature = 1;
}

CFeaturesGL::~CFeaturesGL(void)
{
}

bool CFeaturesGL::LoadDataFromFile()
{
    if ( mbDataLoaded ) return true;
    CFileIO file(mFilePath);
    if ( !file.isFileOpened() )
        return false;

    printf("Loading Feature Data.....\n");

    CFeatures a_feature;
    mNumFeatures = file.GetInt("Number of Features :");

    // Colors
    glm::vec4 color_yellow = glm::vec4(255,255,206,255);    color_yellow /= 255.0f;// yellow
    glm::vec4 color_green  = glm::vec4(0,0.5,0,1);  // green
    glm::vec4 color_orange = glm::vec4(255,128,64,255); color_orange /= 255.0f;// orange
    glm::vec4 color_blue   = glm::vec4(0,0,1,1);    // Blue
    glm::vec4 color_red    = glm::vec4(1,0,0,1);    // Red
    glm::vec4 color;
    //glm::vec4(0, 128, 0, 255);        // green

    glm::vec4 color_grey   = glm::vec4(230.0f); color_grey /= 255.0f;
    for ( int i = 0; i < mNumFeatures; i++)
    {
        a_feature.mFeatureName = file.GetString("Feature No.");
        a_feature.mNumVerts = file.GetInt();

        a_feature.mVertices.clear();
        a_feature.mColor.clear();

        if ( i == 0 )
            color = color_yellow;
        else if ( i == 1 )
            color = color_blue;
        else if ( i == 2 )
            color = color_yellow;
        else if ( i == 3 )
            color = color_blue;
        else if ( i == 180 )    // Its a playground
            color = color_green;
        else if ( i >= 4 )
        {
            if ( i % 2 )
                color = color_green;
            else
            {
                color  = glm::vec4(100,100,255,255);    color /= 255.0f;// light blue
            }
        }

        for ( int v = 0; v < a_feature.mNumVerts; v++ )
        {
            glm::vec3 vert = file.GetVerts();
            if ( i == 0 || i == 2)
                vert.y = -0.2f;
            else
            {
                if ( i % 2 )
                    vert.y = -0.1f;
                else vert.y = 0.0f;
            }
            a_feature.mVertices.push_back(vert);
            a_feature.mColor.push_back ( color );
        }

        float area = calculate_polygon_area(&a_feature.mVertices[0], a_feature.mNumVerts);

        vector <glm::vec3> convex_hull = convexHull(&a_feature.mVertices[0], a_feature.mNumVerts);
        if ( convex_hull.size() != a_feature.mVertices.size())
            printf("Mismath in vertices %d, prev verts %d, curr verts %d\n", i, a_feature.mVertices.size(), convex_hull.size());
        a_feature.mVertices = convex_hull;
        a_feature.mNumVerts = a_feature.mVertices.size();

        mFeatures.push_back ( a_feature );
    }

    //draw_feature = mNumFeatures;
    mbDataLoaded = true;
    printf("Finished Loading Feature Data.....\n");

    return true;
}
void CFeaturesGL::LoadScene()
{
    // Open Shaders etc..
    vector<string> attribs;
    attribs.push_back("aPosition");
    attribs.push_back("aColor");

    vector<string> uniforms;
    uniforms.push_back("uMVPMatrix");

    mShader = new ShaderHandler("shaders/buildings.vert", "shaders/buildings.frag", attribs, uniforms);
}

void CFeaturesGL::DrawScene(CCamera *cam)
{
    if ( !mb_Draw ) return;

    string s_mvp = "uMVPMatrix";
    string pos   = "aPosition";
    string col   = "aColor";

    // Set the shader program
    mShader->useShader();

    glm::mat4 model(1.0);
    glm::mat4 viewproj = cam->getCameraMatrix();
    glm::mat4 mvp = viewproj * model;

    // Pass the transformationMatrix to the shader using its location
    glUniformMatrix4fv( mShader->getUniformHandle( s_mvp ), 1, GL_FALSE, glm::value_ptr(mvp));
    glDisable ( GL_CULL_FACE );
    for ( int i = 0; i < mNumFeatures; i++)
        //for ( int i = 0; i < draw_feature; i++)
    {
        mShader->setAttribs( pos, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), &mFeatures[i].mVertices[0]);
        mShader->setAttribs( col, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), &mFeatures[i].mColor[0]);

        glDrawArrays(GL_POLYGON, 0, mFeatures[i].mNumVerts);
    }
    glEnable ( GL_CULL_FACE );

    mShader->unsetAttribs();
}

void CFeaturesGL::DestroyScene()
{
    // Close shaders etc...
    mShader->DetachShaders();
    delete mShader;
}
