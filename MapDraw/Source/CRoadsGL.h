#pragma once
#include "IRenderObjects.h"
#include "CTextMarker.h"

using namespace std;

class Roads
{
    string mRoadName;
    int mNumVerts;
    vector <glm::vec3> mVertices;
    vector <glm::vec4> mVerticesColor;

    // Broad Road
    vector <glm::vec3> mBroadRoad;
    vector <glm::vec4> mColor;
    GLuint             mTextureId;

    friend class CRoadsGL;
};

class CRoadsGL: public IRenderObjects
{
private :
    int mNumRoads;
    vector<Roads> mRoads;

    glm::vec4 mColor;
    float mWidth;
    CTextMarker mTextureMarker;

    bool mb_IsRoad;

public:
    CRoadsGL(string data_path);
    ~CRoadsGL(void);

    void LoadScene();
    void DrawScene(CCamera *cam);
    void DestroyScene();
    bool LoadDataFromFile();

    void setColor(glm::vec4& color);
    void setWidth(float width);
};
