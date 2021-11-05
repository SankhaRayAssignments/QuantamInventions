#pragma once
#include "IRenderObjects.h"

class CQuad :
    public IRenderObjects
{
public:
    CQuad(glm::vec3 min, glm::vec3 max);
    CQuad(float width, float ypos);
    ~CQuad();
    void LoadScene();
    void DrawScene(CCamera *cam);
    void DestroyScene();

    bool LoadDataFromFile();

private:
    glm::vec3 mMin;
    glm::vec3 mMax;

    vector <glm::vec3> mVertices;
    vector <glm::vec4> mColor;

};
