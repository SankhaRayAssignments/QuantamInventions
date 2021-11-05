#include "CRoadsGL.h"
#include "mb_GLDebug.h"
#include "math_helper.h"

static const float ROAD_Y  = 0.1f;
static const float RPATH_Y = 0.2f;

CRoadsGL::CRoadsGL(string data_path)
{
    mFilePath = data_path;
    mbDataLoaded = false;
    mb_Draw = true;

    mColor = glm::vec4(0,0,0,1);
    mWidth = 1.0f;
}

CRoadsGL::~CRoadsGL(void)
{
}

bool CRoadsGL::LoadDataFromFile()
{
    CFileIO file(mFilePath);
    std::size_t found = mFilePath.find("routed_path");
    mb_IsRoad = (found == std::string::npos);

    printf("Loading Road Data.....\n");

    Roads a_road;
    mNumRoads = file.GetInt("Number of Roads :");

    for ( int i = 0; i < mNumRoads; i++)
    {
        if (mb_IsRoad)
        {
            a_road.mRoadName = "Road No. " + file.GetString("Road No.");
        }
        else
            a_road.mRoadName = file.GetString("RPath No.");

        a_road.mNumVerts = file.GetInt();

        a_road.mVertices.clear();
        a_road.mBroadRoad.clear();
        a_road.mColor.clear();

        glm::vec4 vert_color(0,0,0,1);
        for ( int v = 0; v < a_road.mNumVerts; v++ )
        {
            glm::vec3 pos = file.GetVerts();
            if ( mb_IsRoad ) pos.y = ROAD_Y;
            else pos.y = RPATH_Y;
            a_road.mVertices.push_back(pos);
            a_road.mVerticesColor.push_back(vert_color);
        }

        // Calculate broad road here
        for ( int i = 0; i < a_road.mNumVerts - 1; i++)
        {
            glm::vec3 p1 = a_road.mVertices[i];
            glm::vec3 p2 = a_road.mVertices[i+1];

            // Get the vector direction
            glm::vec3 dir  = p1 - p2;
            //glm::vec3 perp = glm::vec3(dir.z, found ? ROAD_Y : RPATH_Y, -dir.x);  // get the perpendicluar
            glm::vec3 perp = glm::vec3(dir.z, dir.y, -dir.x);   // get the perpendicluar
            perp = glm::normalize(perp);        // normalize

            glm::vec3 point_0 = p1 + (mWidth/2)*perp ;      // Left
            glm::vec3 point_1 = p1 - (mWidth/2)*perp;       // Right

            glm::vec3 point_2 = p2 + (mWidth/2)*perp;       // Left
            glm::vec3 point_3 = p2 - (mWidth/2)*perp;       // Right

            /* Traingles */
            a_road.mBroadRoad.push_back(point_0);   a_road.mColor.push_back ( mColor);
            a_road.mBroadRoad.push_back(point_1);   a_road.mColor.push_back ( mColor);
            a_road.mBroadRoad.push_back(point_2);   a_road.mColor.push_back ( mColor);

            a_road.mBroadRoad.push_back(point_1);   a_road.mColor.push_back ( mColor);
            a_road.mBroadRoad.push_back(point_3);   a_road.mColor.push_back ( mColor);
            a_road.mBroadRoad.push_back(point_2);   a_road.mColor.push_back ( mColor);
        }

        mRoads.push_back ( a_road );
    }

    if ( mb_IsRoad ) mTextureMarker.LoadDataFromFile();
    mbDataLoaded = true;
    printf("Finished Loading Road Data.....\n");

    return true;
}

void CRoadsGL::LoadScene()
{
    // Open Shaders etc..
    vector<string> attribs;
    attribs.push_back("aPosition");
    attribs.push_back("aColor");

    vector<string> uniforms;
    uniforms.push_back("uMVPMatrix");

    mShader = new ShaderHandler("shaders/buildings.vert", "shaders/buildings.frag", attribs, uniforms);

    if ( mb_IsRoad )
    {
        mTextureMarker.LoadScene();

        // Create Texture marker
        glm::vec4 bg_color   = glm::vec4(1,1,1,1);  // white
        glm::vec4 font_color = glm::vec4(1,0,0,1);  // Red
        for ( int i = 0; i < mNumRoads; i++)
        {
            Roads *a_road = &mRoads[i];
            a_road->mTextureId = mTextureMarker.getTexID ( a_road->mRoadName, bg_color, font_color );
        }
    }
}

void CRoadsGL::DrawScene(CCamera *cam)
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
    //glFrontFace ( GL_CW );
    for ( int i = 0; i <  mNumRoads; i++)
    {
        mShader->setAttribs( pos, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), &mRoads[i].mBroadRoad[0]);
        mShader->setAttribs( col, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), &mRoads[i].mColor[0]);

        glDrawArrays(GL_TRIANGLES, 0, (mRoads[i].mNumVerts - 1)  * 6);
    }
    glEnable( GL_CULL_FACE );

    // Draw the inner roads
    for ( int i = 0; i <  mNumRoads; i++)
    {
        mShader->setAttribs( pos, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), &mRoads[i].mVertices[0]);
        mShader->setAttribs( col, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), &mRoads[i].mVerticesColor[0]);
        glDrawArrays(GL_LINES, 0, mRoads[i].mNumVerts);
    }

    mShader->unsetAttribs();

    if ( mb_IsRoad )
    {
        glFrontFace ( GL_CW );
        // Now Draw the road markers
        for ( int i = 0; i <  mNumRoads; i++)
        {
            // Get a in between position for road marker
            glm::vec3 pos = 0.5f * (mRoads[i].mVertices[0] + mRoads[i].mVertices[ 1 ]) ;
            pos.y = ROAD_Y;
            mTextureMarker.DrawScene(cam, mRoads[i].mTextureId, pos);
        }
        glFrontFace ( GL_CCW );
    }
}

void CRoadsGL::DestroyScene()
{
    // Close shaders etc...
    mTextureMarker.DestroyScene();
    mShader->DetachShaders();
    delete mShader;
}

void CRoadsGL::setColor(glm::vec4& color)
{
    mColor = color;
}
void CRoadsGL::setWidth(float width)
{
    mWidth = width;
}
