#pragma once
#include "IRenderObjects.h"
#include "CTextMarker.h"

using namespace std;

class CBuilding
{
    string mBuildingName;
    int mNumVerts;
    vector <glm::vec3> mVertices;
    vector <glm::vec4> mColor;
    GLuint             mTextureId;

    friend class CBuildingsGL;
};

class CBuildingsGL : public IRenderObjects
{
private :
    int mNumBuildings;
    vector<CBuilding> mBuildings;
    CTextMarker mTextureMarker;

public:
    CBuildingsGL(string data_path);
    ~CBuildingsGL(void);

    void LoadScene();
    void DrawScene(CCamera *cam);
    void DestroyScene();

    bool LoadDataFromFile();
};

