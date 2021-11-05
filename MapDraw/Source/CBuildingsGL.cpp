#include "CBuildingsGL.h"
#include "mb_GLDebug.h"

static const float BUILDING_Y = 0.2f;

CBuildingsGL::CBuildingsGL(string data_path)
{
    mFilePath = data_path;
    mbDataLoaded = false;
    mb_Draw = true;
}

CBuildingsGL::~CBuildingsGL(void)
{
}

bool CBuildingsGL::LoadDataFromFile()
{
    if ( mbDataLoaded ) return true;

    CFileIO file(mFilePath);
    if ( !file.isFileOpened() )
        return false;

    printf("Loading Buildings Data.....\n");

    CBuilding a_building;
    glm::vec4 color;
    mNumBuildings = file.GetInt("Number of buildings :");

    for ( int i = 0; i < mNumBuildings; i++)
    {
        a_building.mBuildingName = "Building " + file.GetString("Building");
        a_building.mNumVerts = file.GetInt();

        a_building.mVertices.clear();

        for ( int v = 0; v < a_building.mNumVerts; v++ )
        {
            glm::vec3 pos = file.GetVerts();
            pos.y += BUILDING_Y;

            a_building.mVertices.push_back(pos);

            if ( v == 0 )   // First the first vertex as yellow
                color = glm::vec4(1,1,0,1); // yellow
            else color = glm::vec4(1,0,0,1);    // red

            a_building.mColor.push_back ( color );
        }

        mBuildings.push_back ( a_building );
    }

    mTextureMarker.LoadDataFromFile();
    mbDataLoaded = true;
    printf("Finished Loading Buildings Data.....\n");

    return true;
}

void CBuildingsGL::LoadScene()
{
    // Open Shaders etc..
    vector<string> attribs;
    attribs.push_back("aPosition");
    attribs.push_back("aColor");

    vector<string> uniforms;
    uniforms.push_back("uMVPMatrix");

    mShader = new ShaderHandler("shaders/buildings.vert", "shaders/buildings.frag", attribs, uniforms);

    mTextureMarker.LoadScene();

    // Create Texture marker
    glm::vec4 bg_color   = glm::vec4(250,213,147,255);  bg_color /= 255.0f; // Light Orange
    glm::vec4 font_color = glm::vec4(0,0,0,1);      // Black
    for ( int i = 0; i < mNumBuildings; i++)
    {
        CBuilding *a_building = &mBuildings[i];
        a_building->mTextureId = mTextureMarker.getTexID ( a_building->mBuildingName, bg_color, font_color );
    }
}

void CBuildingsGL::DrawScene(CCamera *cam)
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

    // Pass the transformationMatrix to the shader  using its location
    glUniformMatrix4fv( mShader->getUniformHandle( s_mvp ), 1, GL_FALSE, glm::value_ptr(mvp));

    glDisable ( GL_CULL_FACE );

    for ( int i = 0; i < mNumBuildings; i++)
    {
        mShader->setAttribs( pos, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), &mBuildings[i].mVertices[0]);
        mShader->setAttribs( col, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), &mBuildings[i].mColor[0]);
        glDrawArrays(GL_POLYGON, 0, mBuildings[i].mNumVerts);
    }

    mShader->unsetAttribs();

    glFrontFace ( GL_CW );
    // Now Draw the road markers
    for ( int i = 0; i <  mNumBuildings; i++)
    {
        mTextureMarker.DrawScene(cam, mBuildings[i].mTextureId, mBuildings[i].mVertices[mBuildings[i].mVertices.size()/2]);
    }
    glFrontFace ( GL_CCW );
}

void CBuildingsGL::DestroyScene()
{
    // Close shaders etc...
    mTextureMarker.DestroyScene();
    mShader->DetachShaders();
    delete mShader;
}
