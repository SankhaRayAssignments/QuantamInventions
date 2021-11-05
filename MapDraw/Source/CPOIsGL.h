#pragma once
#include "IRenderObjects.h"

using namespace std;

class POI
{
    vector <glm::vec3> mVertices;
    vector <glm::vec3> mPyramidVertices;
    vector <glm::vec4> mColor;

    friend class CPOIsGL;
};

class CPOIsGL : public IRenderObjects
{
private :
    string mFilePath;

    int mNumPOIs;
    vector <POI> mPOIs;

public:
    CPOIsGL(string data_path);
    ~CPOIsGL(void);

    void LoadScene();
    void DrawScene(CCamera *cam);
    void DestroyScene();

    bool LoadDataFromFile();
};
