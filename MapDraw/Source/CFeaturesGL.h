#pragma once
#include "IRenderObjects.h"

using namespace std;

class CFeatures
{
    string mFeatureName;
    int mNumVerts;
    vector <glm::vec3> mVertices;
    vector <glm::vec4> mColor;

    friend class CFeaturesGL;
};

class CFeaturesGL  : public IRenderObjects
{
private :
    int mNumFeatures;
    vector<CFeatures> mFeatures;

public:
    int draw_feature;
    CFeaturesGL(string data_path);
    ~CFeaturesGL(void);

    void LoadScene();
    void DrawScene(CCamera *cam);
    void DestroyScene();
    bool LoadDataFromFile();

    void increaseCount()
    {
        draw_feature = min((draw_feature + 1), mNumFeatures);
        printf("draw_feature %d\n",draw_feature );
    }
    void decreaseCount()
    {
        draw_feature = max((draw_feature - 1), 1);
        printf("draw_feature %d\n",draw_feature );
    }

};
